/**
 * Proyecto: Recypress - Compactador automático de latas (TEI201 - Taller de Diseño, UAI)
 * Versión:  v3.0 (Iteración 3, prototipo final) - CÓDIGO OFICIAL
 * Placa:    Arduino UNO
 *
 * Qué hace:
 *   1. En reposo espera que se presione el pulsador verde (COMPACTAR).
 *   2. AVANCE: activa el relé 1 y la placa baja a compactar las latas.
 *      Mientras baja, mide la corriente del actuador con el sensor de corriente.
 *   3. UMBRAL: apenas la corriente llega a 2900 mA (2,9 A ≈ 665 N), el actuador se
 *      detiene y RETROCEDE DE INMEDIATO. Nunca debe llegar a 3 A: desde ahí la fuerza
 *      supera lo que resiste la estructura de madera y la máquina se daña.
 *      El avance también termina si la corriente cae casi a 0 (fin de carrera interno
 *      del actuador) o si se cumplen 29 segundos de bajada (tiempo máximo de avance).
 *   4. RETORNO: activa el relé 2 y la placa sube hasta su fin de carrera superior.
 *   5. El pulsador rojo (PARO DE EMERGENCIA) apaga ambos relés en cualquier momento.
 *      Después de un paro, el pulsador verde solo hace SUBIR la placa (retorno seguro).
 *
 * Por qué se usa la corriente:
 *   En un motor DC la fuerza es proporcional a la corriente. Medir la corriente es una
 *   forma barata de vigilar la fuerza. El umbral de 2,9 A deja un margen de 0,1 A antes
 *   de los 3 A en que la estructura empezaba a romperse. Con este límite el prototipo
 *   compacta como máximo 5 latas paradas; las latas acostadas no se deben usar.
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
 * Monitor Serie: 9600 baudios (muestra estado y corriente en mA).
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

// ----------------------------------------------------------------- UMBRAL DE CORRIENTE
// 2900 mA = 2,9 A ≈ 665 N. Al llegar aquí la placa retrocede de inmediato.
// NO SUBIR: desde 3000 mA (3 A ≈ 700 N) la estructura de madera se daña.
const long UMBRAL_CORRIENTE_MA = 2900;

// ----------------------------------------------------------------- Sensor de corriente
// Sensibilidad del ACS712 según su versión (está impresa en el chip):
//   ACS712-05B -> 0.185 V/A   ACS712-20A -> 0.100 V/A   ACS712-30A -> 0.066 V/A
const float SENSIBILIDAD_V_POR_A = 0.100;   // [verificar versión del módulo]
const float VOLTAJE_REFERENCIA   = 5.0;     // el ADC del UNO mide de 0 a 5 V
const int   MUESTRAS_PROMEDIO    = 20;      // promediar reduce el ruido del motor (~2 ms)

// ----------------------------------------------------------------- Tiempos y otros límites
const long CORRIENTE_FIN_CARRERA_MA = 300;          // bajo esto el motor está detenido
const unsigned long T_IGNORAR_ARRANQUE_MS = 400;    // pico de arranque del motor (placa arriba, sin carga)
const unsigned long T_CONFIRMAR_FIN_MS    = 800;    // la corriente baja debe mantenerse este tiempo
const unsigned long T_INVERSION_MS        = 50;     // mínimo con ambos relés apagados antes de retroceder
const unsigned long T_AVANCE_MAX_MS       = 29000;  // 29 s: tiempo máximo de bajada (valor usado en el prototipo)
const unsigned long T_RETORNO_MAX_MS      = 60000;  // la subida se corta antes, al llegar arriba
const unsigned long T_ANTIRREBOTE_MS      = 30;

// ----------------------------------------------------------------- Estados
enum Estado { REPOSO, AVANCE, INVERSION, RETORNO, EMERGENCIA };
Estado estado = REPOSO;
unsigned long inicioEstado = 0;      // millis() al entrar al estado actual

// Temporizador para exigir que la corriente BAJA (fin de carrera) se mantenga un tiempo.
// La corriente ALTA no usa temporizador: el retroceso es inmediato.
struct Temporizador {
  bool activo;
  unsigned long desde;
};
Temporizador tCorrienteBaja = {false, 0};

float voltajeCero = 2.5;             // salida del sensor sin corriente (se mide al encender)
long corrienteMA = 0;
long corrienteMaximaCicloMA = 0;     // la mayor corriente del ciclo actual (para el registro)

// Antirrebote del pulsador verde (solo cuenta el momento en que se presiona)
bool verdeEstable = false;
bool verdeLecturaAnterior = false;
unsigned long verdeCambio = 0;

long fuerzaEstimadaN(long mA);   // definida al final del archivo

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

long leerCorrienteMA() {
  // El sentido de giro cambia el signo; para el umbral importa solo el tamaño.
  float amperes = (leerVoltajeSensor() - voltajeCero) / SENSIBILIDAD_V_POR_A;
  if (amperes < 0) amperes = -amperes;
  return (long)(amperes * 1000.0 + 0.5);
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
  Serial.print(F("Compactador v3 listo. Umbral: "));
  Serial.print(UMBRAL_CORRIENTE_MA);
  Serial.print(F(" mA. Cero del sensor: "));
  Serial.print(voltajeCero, 3);
  Serial.println(F(" V"));
  inicioEstado = millis();
}

void loop() {
  corrienteMA = leerCorrienteMA();
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
        corrienteMaximaCicloMA = 0;
        activarAvance();
        cambiarEstado(AVANCE, "AVANCE: la placa baja a compactar");
      }
      break;

    case AVANCE:
      if (enEstado >= T_IGNORAR_ARRANQUE_MS) {
        if (corrienteMA > corrienteMaximaCicloMA) corrienteMaximaCicloMA = corrienteMA;

        if (corrienteMA >= UMBRAL_CORRIENTE_MA) {
          // UMBRAL ALCANZADO: se corta en esta misma vuelta del programa (pocos ms),
          // antes de que la corriente alcance a subir de 2,9 A a 3 A.
          detener();
          cambiarEstado(INVERSION, "Umbral de 2900 mA: retroceso inmediato");
        } else if (mantenida(corrienteMA < CORRIENTE_FIN_CARRERA_MA, tCorrienteBaja, T_CONFIRMAR_FIN_MS)) {
          // Corriente casi nula: el fin de carrera interno del actuador cortó el motor.
          detener();
          cambiarEstado(INVERSION, "Fin de carrera inferior");
        }
      }
      if (estado == AVANCE && enEstado >= T_AVANCE_MAX_MS) {
        detener();
        cambiarEstado(INVERSION, "Tiempo maximo de avance");
      }
      break;

    case INVERSION:
      // 50 ms con ambos relés apagados: lo justo para que el relé 1 se suelte antes de
      // que cierre el relé 2. Así no salta chispa en los contactos al invertir el motor.
      if (enEstado >= T_INVERSION_MS) {
        activarRetorno();
        cambiarEstado(RETORNO, "RETORNO: la placa sube");
      }
      break;

    case RETORNO:
      if (enEstado >= T_IGNORAR_ARRANQUE_MS) {
        if (corrienteMA >= UMBRAL_CORRIENTE_MA) {
          // Algo traba la subida: se detiene todo y se espera al operador.
          detener();
          cambiarEstado(EMERGENCIA, "Sobrecorriente al subir: revisar la placa");
        } else if (mantenida(corrienteMA < CORRIENTE_FIN_CARRERA_MA, tCorrienteBaja, T_CONFIRMAR_FIN_MS)) {
          detener();
          Serial.print(F("Corriente maxima del ciclo: "));
          Serial.print(corrienteMaximaCicloMA);
          Serial.println(F(" mA"));
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
    const char *nombres[] = {"REPOSO", "AVANCE", "INVERSION", "RETORNO", "EMERGENCIA"};
    Serial.print(nombres[estado]);
    Serial.print(F("  I = "));
    Serial.print(corrienteMA);
    Serial.print(F(" mA  F~ "));
    Serial.print(fuerzaEstimadaN(corrienteMA));
    Serial.println(F(" N"));
  }
}

// Estimación de fuerza a partir de la corriente (ver testing/reportes/pruebas_carga_v3.md):
// F ≈ 350 N/A · (I − 1 A). Con 2,9 A da ≈ 665 N; con 3 A, ≈ 700 N.
// Es una aproximación, no una medición.
long fuerzaEstimadaN(long mA) {
  long f = (350L * (mA - 1000L)) / 1000L;
  return f < 0 ? 0 : f;
}
