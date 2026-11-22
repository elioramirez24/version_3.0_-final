# Software - Documentación

## Contenido

| Carpeta | Contenido |
|---|---|
| `src/` | Programa definitivo `compactador_v3.ino` y programa de diagnóstico |
| `pruebas_pc/` | Pruebas de la lógica del programa en un computador, sin hardware |
| `docs/` | Manual de operación, diagrama de estados y solución de problemas |
| `librerias/` | El proyecto no usa librerías externas |

---

## Stack tecnológico

**Plataforma:** Arduino UNO (ATmega328P, 5 V)  
**Lenguaje:** C/C++ (Arduino)  
**IDE:** Arduino IDE 2.x  
**Librerías externas:** ninguna  
**Conectividad:** ninguna en esta versión (el prototipo funciona de forma autónoma)

---

## Instalación y carga del programa

1. Instalar Arduino IDE 2.x desde arduino.cc.
2. Abrir `software/src/compactador_v3/compactador_v3.ino`.
3. Revisar la constante `SENSIBILIDAD_V_POR_A` según la versión del sensor de corriente (está impresa en el chip: 05B, 20A o 30A).
4. En **Herramientas → Placa** elegir **Arduino UNO** y en **Puerto** el del Arduino.
5. Presionar ✓ (Verificar) y luego → (Subir).
6. Abrir el Monitor Serie a **9600 baudios**: debe aparecer `Compactador v3 listo. Umbral: 2900 mA` y el voltaje de cero del sensor (cerca de 2,5 V).

> Antes de la primera prueba con 12 V, subir `src/diagnostico/diagnostico.ino` y hacer la prueba en seco (ver `hardware/instrucciones_ensamble.md`).

---

## Arquitectura del programa

El programa es una **máquina de estados**: en cada momento está en uno solo de cinco estados (REPOSO, AVANCE, INVERSION, RETORNO, EMERGENCIA) y cambia de uno a otro según los pulsadores y la corriente medida. El diagrama y la tabla completa están en `src/README.md`.

### Módulos principales

1. **Inicialización** (`setup`): deja los relés apagados antes de declararlos como salidas, configura los pulsadores con resistencia interna y mide el "cero" del sensor de corriente.
2. **Lectura de entradas** (`leerCorrienteMA`, `verdeRecienPresionado`): promedia 20 lecturas del sensor y detecta el instante en que se presiona el verde (con antirrebote).
3. **Decisión** (`switch (estado)` en `loop`): aplica las reglas de cada estado; el paro de emergencia se revisa antes que todo lo demás.
4. **Actuación** (`activarAvance`, `activarRetorno`, `detener`): enciende un relé solo después de apagar el otro.
5. **Registro** (Monitor Serie): cada 250 ms muestra el estado, la corriente y la fuerza estimada.

---

## Configuración de parámetros

Las constantes que se pueden ajustar (límite de corriente, tiempos, sensibilidad del sensor) están al inicio de `compactador_v3.ino` y se explican en `src/README.md`.

**Importante:** `UMBRAL_CORRIENTE_MA = 2900` (2,9 A) protege la estructura de madera, que se daña desde 3 A. No debe aumentarse sin reforzar antes el mueble.

---

## Historial de versiones del software

| Versión | Archivo | Comportamiento |
|---|---|---|
| v1.0 | `iteraciones/iteracion_1/codigo/main/main.ino` | Control manual: el actuador se mueve solo mientras se mantiene presionado un botón |
| v2.0 | Simulación en Wokwi (`hardware/esquemas/simulacion_wokwi_v3.png`) | Ciclo automático con sensor de presencia, sensor de corriente simulado con un potenciómetro y LED en lugar del actuador |
| v3.0 | `src/compactador_v3/compactador_v3.ino` | Ciclo automático en el prototipo real, umbral de 2900 mA con retroceso inmediato, detección de fin de carrera por corriente y paro de emergencia |
