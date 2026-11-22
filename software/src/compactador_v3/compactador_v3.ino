/**
 * Proyecto: Recypress - Compactador automático de latas (TEI201 - Taller de Diseño, UAI)
 * Versión:  v3.0 (Iteración 3, prototipo final)
 * Placa:    Arduino UNO
 *
 * Qué hace:
 *   1. En reposo espera que se presione el pulsador verde (COMPACTAR).
 *   2. AVANCE: activa el relé 1 y la placa baja a compactar las latas.
 *      Mientras baja, mide la corriente del actuador con el sensor de corriente.
 *   3. El avance termina por cualquiera de estas causas:
 *        a) La corriente llega a 3 A (≈ 700 N): las latas ya están compactadas o algo
 *           opone demasiada resistencia. Es el límite que soporta la estructura de madera.
 *        b) La corriente cae casi a 0: el fin de carrera interno del actuador lo detuvo.
 *        c) Se cumple el tiempo máximo de seguridad.
 *   4. RETORNO: activa el relé 2 y la placa sube hasta su fin de carrera superior.
 *   5. El pulsador rojo (PARO DE EMERGENCIA) apaga ambos relés en cualquier momento.
 *      Después de un paro, el pulsador verde solo hace SUBIR la placa (retorno seguro).
 *
 * Por qué se limita la corriente:
 *   En un motor DC la fuerza es proporcional a la corriente. Con la estructura de madera,
 *   sobre 3 A (≈ 700 N) los techos y el piso empezaban a ceder, por eso el programa nunca
 *   deja que el actuador empuje más que eso. Con este límite el prototipo compacta como
 *   máximo 5 latas paradas; las latas acostadas no se deben usar.
 *
 * Tabla de pines (igual a la simulación en Wokwi):
 *   Pin 6  <- Pulsador verde COMPACTAR      (el otro borne a GND, INPUT_PULLUP)
 *   Pin 3  <- Pulsador rojo PARO            (el otro borne a GND, INPUT_PULLUP)
 *   Pin 4  -> IN del relé 1 (AVANCE)
 *   Pin 5  -> IN del relé 2 (RETORNO)
 *   A0     <- OUT del sensor de corriente (tipo ACS712)
 *   NO USAR EL PIN 2: quedó dañado en la iteración 1.
 *
 * Librerías: ninguna externa.
 * Monitor Serie: 9600 baudios (muestra estado y corriente).
 */

// ----------------------------------------------------------------- Pines
const int PIN_BOTON_COMPACTAR = 6;
const int PIN_BOTON_PARO      = 3;
const int PIN_RELE_AVANCE     = 4;
const int PIN_RELE_RETORNO    = 5;
const int PIN_SENSOR          = A0;

// ----------------------------------------------------------------- Relés
// Los módulos usados se ACTIVAN con LOW y se apagan con HIGH (lógica inversa).
const int RELE_ENCENDIDO = LOW;
const int RELE_APAGADO   = HIGH;

// ----------------------------------------------------------------- Sensor de corriente
// Sensibilidad del ACS712 según su versión (está impresa en el chip):
//   ACS712-05B -> 0.185 V/A   ACS712-20A -> 0.100 V/A   ACS712-30A -> 0.066 V/A
const float SENSIBILIDAD_V_POR_A = 0.100;   // [verificar versión del módulo]
const float VOLTAJE_REFERENCIA   = 5.0;     // el ADC del UNO mide de 0 a 5 V
const int   MUESTRAS_PROMEDIO    = 20;      // promediar reduce el ruido del motor

// ----------------------------------------------------------------- Límites del ciclo
const float CORRIENTE_LIMITE_A       = 3.0;    // ≈ 700 N: máximo que soporta la estructura
const float CORRIENTE_FIN_CARRERA_A  = 0.30;   // bajo esto el motor está detenido
const unsigned long T_IGNORAR_ARRANQUE_MS = 600;   // el pico de arranque no cuenta como límite
const unsigned long T_CONFIRMAR_LIMITE_MS = 150;   // la corriente alta debe mantenerse este tiempo
const unsigned long T_CONFIRMAR_FIN_MS    = 800;   // la corriente baja debe mantenerse este tiempo
const unsigned long T_AVANCE_MAX_MS       = 60000; // 300 mm a 6 mm/s = 50 s, más margen
const unsigned long T_RETORNO_MAX_MS      = 60000;
const unsigned long T_PAUSA_INVERSION_MS  = 300;   // pausa antes de invertir el sentido
const unsigned long T_ANTIRREBOTE_MS      = 30;

// ----------------------------------------------------------------- Estados
enum Estado { REPOSO, AVANCE, PAUSA, RETORNO, EMERGENCIA };
Estado estado = REPOSO;
unsigned long inicioEstado = 0;      // millis() al entrar al estado actual

// Temporizador para exigir que una condición se mantenga cierto tiempo seguido
// (así un pico breve de corriente no detiene el ciclo por error).
struct Temporizador {
  bool activo;
  unsigned long desde;
};
Temporizador tCorrienteAlta = {false, 0};
Temporizador tCorrienteBaja = {false, 0};

float voltajeCero = 2.5;             // salida del sensor sin corriente (se mide al encender)
float corrienteA = 0.0;

// Antirrebote del pulsador verde (solo cuenta el momento en que se presiona)
bool verdeEstable = false;
bool verdeLecturaAnterior = false;
unsigned long verdeCambio = 0;

float fuerzaEstimadaN(float corriente);   // definida al final del archivo

// ----------------------------------------------------------------- Relés
void detener() {
  digitalWrite(PIN_RELE_AVANCE, RELE_APAGADO);
  digitalWrite(PIN_RELE_RETORNO, RELE_APAGADO);
}

void activarAvance() {
  digitalWrite(PIN_RELE_RETORNO, RELE_APAGADO);   // primero se apaga el contrario
  digitalWrite(PIN_RELE_AVANCE, RELE_ENCENDIDO);
}

void activarRetorno() {
  digitalWrite(PIN_RELE_AVANCE, RELE_APAGADO);
  digitalWrite(PIN_RELE_RETORNO, RELE_ENCENDIDO);
}

// ----------------------------------------------------------------- Sensor
float leerVoltajeSensor() {
  long suma = 0;
  for (int i = 0; i < MUESTRAS_PROMEDIO; i++) {
    suma += analogRead(PIN_SENSOR);
  }
  return (suma / (float)MUESTRAS_PROMEDIO) * VOLTAJE_REFERENCIA / 1023.0;
}

float leerCorriente() {
  // El sentido de giro cambia el signo; para el límite importa solo el tamaño.
  float corriente = (leerVoltajeSensor() - voltajeCero) / SENSIBILIDAD_V_POR_A;
  return corriente < 0 ? -corriente : corriente;
}

void calibrarCero() {
  // Con los relés apagados no circula corriente: lo que mide el sensor es su "cero".
  float suma = 0;
  for (int i = 0; i < 25; i++) {
    suma += leerVoltajeSensor();
    delay(4);
  }
  voltajeCero = suma / 25.0;
}

// ----------------------------------------------------------------- Utilidades
void cambiarEstado(Estado nuevo, const char *motivo) {
  estado = nuevo;
  inicioEstado = millis();
  tCorrienteAlta.activo = false;
  tCorrienteBaja.activo = false;
  Serial.print(F(">> "));
  Serial.println(motivo);
}

// Devuelve true cuando "condicion" lleva "duracionMs" milisegundos seguidos cumpliéndose.
bool mantenida(bool condicion, Temporizador &t, unsigned long duracionMs) {
  if (!condicion) {
    t.activo = false;
    return false;
  }
  if (!t.activo) {
    t.activo = true;
    t.desde = millis();
  }
  return millis() - t.desde >= duracionMs;
}

bool verdeRecienPresionado() {
  bool lectura = (digitalRead(PIN_BOTON_COMPACTAR) == LOW);
  if (lectura != verdeLecturaAnterior) {
    verdeCambio = millis();
    verdeLecturaAnterior = lectura;
  }
  if (millis() - verdeCambio >= T_ANTIRREBOTE_MS && lectura != verdeEstable) {
    verdeEstable = lectura;
    return verdeEstable;          // true solo en el instante en que se presiona
  }
  return false;
}

// ----------------------------------------------------------------- Programa
void setup() {
  // Se fija "apagado" ANTES de declarar las salidas, para que los relés
  // no se activen ni por un instante al encender.
  digitalWrite(PIN_RELE_AVANCE, RELE_APAGADO);
  digitalWrite(PIN_RELE_RETORNO, RELE_APAGADO);
  pinMode(PIN_RELE_AVANCE, OUTPUT);
  pinMode(PIN_RELE_RETORNO, OUTPUT);

  // Pulsadores con la resistencia interna: suelto = HIGH, presionado = LOW.
  // Nunca configurar como OUTPUT un pin con pulsador (así se dañó el pin 2 en la v1).
  pinMode(PIN_BOTON_COMPACTAR, INPUT_PULLUP);
  pinMode(PIN_BOTON_PARO, INPUT_PULLUP);

  Serial.begin(9600);
  calibrarCero();
  Serial.print(F("Compactador v3 listo. Cero del sensor: "));
  Serial.print(voltajeCero, 3);
  Serial.println(F(" V"));
  inicioEstado = millis();
}

void loop() {
  corrienteA = leerCorriente();
  bool paro = (digitalRead(PIN_BOTON_PARO) == LOW);
  bool verde = verdeRecienPresionado();
  unsigned long enEstado = millis() - inicioEstado;

  // 1) El paro de emergencia tiene prioridad sobre todo lo demás.
  if (paro && estado != EMERGENCIA) {
    detener();
    cambiarEstado(EMERGENCIA, "PARO DE EMERGENCIA: reles apagados");
  }

  switch (estado) {
    case REPOSO:
      detener();
      if (verde) {
        activarAvance();
        cambiarEstado(AVANCE, "AVANCE: la placa baja a compactar");
      }
      break;

    case AVANCE:
      if (enEstado >= T_IGNORAR_ARRANQUE_MS) {
        bool alta = mantenida(corrienteA >= CORRIENTE_LIMITE_A, tCorrienteAlta, T_CONFIRMAR_LIMITE_MS);
        bool baja = mantenida(corrienteA < CORRIENTE_FIN_CARRERA_A, tCorrienteBaja, T_CONFIRMAR_FIN_MS);
        if (alta) {
          detener();
          cambiarEstado(PAUSA, "Limite de 3 A (~700 N): fin de la compactacion");
        } else if (baja) {
          // Corriente casi nula: el fin de carrera interno del actuador cortó el motor.
          detener();
          cambiarEstado(PAUSA, "Fin de carrera inferior");
        }
      }
      if (estado == AVANCE && enEstado >= T_AVANCE_MAX_MS) {
        detener();
        cambiarEstado(PAUSA, "Tiempo maximo de avance");
      }
      break;

    case PAUSA:
      // Pausa corta con ambos relés apagados antes de invertir el sentido.
      if (enEstado >= T_PAUSA_INVERSION_MS) {
        activarRetorno();
        cambiarEstado(RETORNO, "RETORNO: la placa sube");
      }
      break;

    case RETORNO:
      if (enEstado >= T_IGNORAR_ARRANQUE_MS) {
        bool alta = mantenida(corrienteA >= CORRIENTE_LIMITE_A, tCorrienteAlta, T_CONFIRMAR_LIMITE_MS);
        bool baja = mantenida(corrienteA < CORRIENTE_FIN_CARRERA_A, tCorrienteBaja, T_CONFIRMAR_FIN_MS);
        if (alta) {
          // Algo traba la subida: se detiene todo y se espera al operador.
          detener();
          cambiarEstado(EMERGENCIA, "Sobrecorriente al subir: revisar la placa");
        } else if (baja) {
          detener();
          cambiarEstado(REPOSO, "Fin de carrera superior: listo para otro ciclo");
        }
      }
      if (estado == RETORNO && enEstado >= T_RETORNO_MAX_MS) {
        detener();
        cambiarEstado(REPOSO, "Tiempo maximo de retorno");
      }
      break;

    case EMERGENCIA:
      detener();
      // Con el rojo suelto, el verde solo hace SUBIR la placa (nunca bajar).
      if (!paro && verde) {
        activarRetorno();
        cambiarEstado(RETORNO, "Retorno despues del paro");
      }
      break;
  }

  // Registro en el Monitor Serie cada 250 ms
  static unsigned long ultimoRegistro = 0;
  if (millis() - ultimoRegistro >= 250) {
    ultimoRegistro = millis();
    const char *nombres[] = {"REPOSO", "AVANCE", "PAUSA", "RETORNO", "EMERGENCIA"};
    Serial.print(nombres[estado]);
    Serial.print(F("  I = "));
    Serial.print(corrienteA, 2);
    Serial.print(F(" A  F~ "));
    Serial.print(fuerzaEstimadaN(corrienteA), 0);
    Serial.println(F(" N"));
  }
}

// Estimación de fuerza a partir de la corriente (ver testing/reportes/pruebas_carga_v3.md):
// F ≈ 350 N/A · (I − 1 A). Con 3 A da ≈ 700 N. Es una aproximación, no una medición.
float fuerzaEstimadaN(float corriente) {
  float f = 350.0 * (corriente - 1.0);
  return f < 0 ? 0 : f;
}
