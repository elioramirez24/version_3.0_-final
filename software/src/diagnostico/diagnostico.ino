/**
 * Código de DIAGNÓSTICO - Compactador de latas (v3)
 *
 * Uso: subir con la fuente de 12 V DESENCHUFADA y abrir el Monitor Serie a 9600 baudios.
 *   1) Los relés se activan solos, uno a la vez, cada 2 segundos
 *      -> comprueba relés, cables IN y pines 4 y 5 (se oye el "clic" y se enciende la luz verde).
 *   2) El Monitor Serie muestra el estado de cada pulsador
 *      -> comprueba pulsadores (pines 6 y 3) y la tierra común.
 *   3) El Monitor Serie muestra el voltaje de salida del sensor de corriente (A0)
 *      -> sin 12 V debe quedar cerca de 2,5 V. Si marca 0 V o 5 V, revisar VCC, GND y OUT del sensor.
 *
 * Opcional, con la fuente ENCHUFADA y la placa sin latas: al activarse el relé 1 o 2 el actuador
 * se mueve 2 s y la corriente debe marcar cerca de 1 A (actuador en vacío).
 *
 * Al terminar, volver a subir software/src/compactador_v3/compactador_v3.ino.
 */

const int PIN_BOTON_COMPACTAR = 6;
const int PIN_BOTON_PARO      = 3;
const int PIN_RELE_AVANCE     = 4;
const int PIN_RELE_RETORNO    = 5;
const int PIN_SENSOR          = A0;

const float SENSIBILIDAD_V_POR_A = 0.100;   // 0.185 (5 A), 0.100 (20 A), 0.066 (30 A)

unsigned long ultimoCambio = 0;
int paso = 0;
float voltajeCero = 2.5;

float leerVoltaje() {
  long suma = 0;
  for (int i = 0; i < 20; i++) suma += analogRead(PIN_SENSOR);
  return (suma / 20.0) * 5.0 / 1023.0;
}

void setup() {
  Serial.begin(9600);
  digitalWrite(PIN_RELE_AVANCE, HIGH);    // HIGH = apagado en estos módulos
  digitalWrite(PIN_RELE_RETORNO, HIGH);
  pinMode(PIN_RELE_AVANCE, OUTPUT);
  pinMode(PIN_RELE_RETORNO, OUTPUT);
  pinMode(PIN_BOTON_COMPACTAR, INPUT_PULLUP);
  pinMode(PIN_BOTON_PARO, INPUT_PULLUP);
  delay(200);
  voltajeCero = leerVoltaje();            // con los relés apagados no circula corriente
}

void loop() {
  // Parte 1: alternar relés cada 2 segundos (apagados -> relé 1 -> relé 2)
  if (millis() - ultimoCambio > 2000) {
    ultimoCambio = millis();
    paso = (paso + 1) % 3;
    digitalWrite(PIN_RELE_AVANCE,  paso == 1 ? LOW : HIGH);
    digitalWrite(PIN_RELE_RETORNO, paso == 2 ? LOW : HIGH);
  }

  // Parte 2 y 3: pulsadores y sensor
  float v = leerVoltaje();
  float corriente = (v - voltajeCero) / SENSIBILIDAD_V_POR_A;
  const char *reles[] = {"apagados", "RELE 1 (avance)", "RELE 2 (retorno)"};

  Serial.print("Reles: ");
  Serial.print(reles[paso]);
  Serial.print("  | Verde: ");
  Serial.print(digitalRead(PIN_BOTON_COMPACTAR) == LOW ? "PRESIONADO" : "suelto");
  Serial.print("  | Rojo: ");
  Serial.print(digitalRead(PIN_BOTON_PARO) == LOW ? "PRESIONADO" : "suelto");
  Serial.print("  | Sensor: ");
  Serial.print(v, 3);
  Serial.print(" V  (");
  Serial.print(corriente, 2);
  Serial.println(" A)");
  delay(200);
}
