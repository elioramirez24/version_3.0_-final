# Documentación de Software

## Manual de operación

### Encendido del sistema
1. Verificar que la placa compactadora esté arriba y que no haya manos ni objetos dentro de la cámara.
2. Conectar el cargador USB del Arduino (5 V). Las luces rojas de los relés y del sensor se encienden.
3. Enchufar la fuente de 12 V a 220 V. **El actuador debe quedar quieto.** Si se mueve solo, desenchufar la fuente de inmediato y revisar el cableado de los relés.

### Operación normal
1. Abrir la puerta de carga y dejar **como máximo 5 latas, siempre paradas** (de pie). No poner latas acostadas.
2. Cerrar la puerta de carga.
3. Presionar una vez el **pulsador verde (COMPACTAR)**. La placa baja.
4. Cuando la corriente llega al umbral de **2900 mA (2,9 A ≈ 665 N)** o la placa llega al final de su recorrido, la placa retrocede de inmediato y vuelve a subir sola.
5. Esperar a que la placa termine de subir (el Monitor Serie muestra `Fin de carrera superior: listo para otro ciclo`).
6. Retirar las latas compactadas por la puerta frontal inferior.

### Paro de emergencia
- Presionar el **pulsador rojo (PARO)** detiene el actuador al instante, en cualquier parte del ciclo.
- Para continuar: soltar el rojo y presionar el verde. Después de un paro, el verde **solo hace subir** la placa; para un nuevo ciclo hay que volver a presionar el verde cuando la placa ya esté arriba.

### Apagado del sistema
1. Esperar a que la placa esté arriba.
2. Desenchufar la fuente de 12 V.
3. Desconectar el cargador USB.

### Cuidados de uso
- **Ciclo de trabajo del actuador: 10 %.** Por cada minuto de movimiento, dejar unos 9 minutos de descanso. Un ciclo completo dura como máximo cerca de 1 minuto (hasta 29 s de bajada y lo mismo de subida).
- No aumentar el umbral de 2900 mA del programa: la estructura de madera se daña desde 3 A (≈ 700 N).

---

## Funciones principales de `compactador_v3.ino`

**`leerCorrienteMA()`**
- **Propósito:** medir la corriente del actuador en miliamperes (mA).
- **Retorna:** `long`, corriente en mA, siempre positiva (el signo depende del sentido de giro).
- **Cómo funciona:** promedia 20 lecturas de A0, resta el voltaje de cero medido al encender y divide por la sensibilidad del sensor.

**Umbral de 2900 mA (dentro de `loop`, estado AVANCE)**
- **Propósito:** retroceder antes de llegar a 3 A, donde la estructura se daña.
- **Cómo funciona:** la primera lectura igual o mayor a 2900 mA apaga el relé de avance en esa misma vuelta del programa; a los 50 ms se enciende el relé de retorno.
```cpp
if (corrienteMA >= UMBRAL_CORRIENTE_MA) {
  detener();
  cambiarEstado(INVERSION, "Umbral de 2900 mA: retroceso inmediato");
}
```

**`mantenida(condicion, temporizador, duracionMs)`**
- **Propósito:** confirmar que la corriente baja (fin de carrera) se mantiene un tiempo mínimo seguido.
- **Retorna:** `true` cuando la condición lleva `duracionMs` milisegundos cumpliéndose.

**`activarAvance()` / `activarRetorno()` / `detener()`**
- **Propósito:** mover o detener el actuador.
- **Regla:** antes de encender un relé siempre se apaga el otro.

**`fuerzaEstimadaN(mA)`**
- **Propósito:** mostrar en el Monitor Serie una estimación de la fuerza.
- **Fórmula:** F ≈ 350 N/A · (I − 1 A). Es una aproximación (ver `testing/reportes/pruebas_carga_v3.md`), no una medición.

---

## Mensajes del Monitor Serie

| Mensaje | Significado |
|---|---|
| `Compactador v3 listo. Umbral: 2900 mA. Cero del sensor: 2.5xx V` | Encendido correcto. Si el cero está lejos de 2,5 V, revisar el sensor |
| `>> AVANCE: la placa baja a compactar` | Comenzó el ciclo |
| `>> Umbral de 2900 mA: retroceso inmediato` | Se alcanzó la fuerza máxima permitida; la placa sube |
| `Corriente maxima del ciclo: 2915 mA` | La mayor corriente del ciclo; debe quedar siempre bajo 3000 mA |
| `>> Fin de carrera inferior` | La placa llegó al fondo sin alcanzar el límite (cámara vacía o latas ya compactadas) |
| `>> RETORNO: la placa sube` | La placa vuelve a su posición inicial |
| `>> Fin de carrera superior: listo para otro ciclo` | Ciclo terminado |
| `>> PARO DE EMERGENCIA: reles apagados` | Se presionó el rojo |
| `>> Sobrecorriente al subir: revisar la placa` | Algo traba la subida; el sistema queda detenido |
| `>> Tiempo maximo de avance` | Pasaron 29 s bajando sin llegar a 2900 mA: la placa retrocede igual. Si pasa siempre y las latas quedan sin compactar, la placa no alcanza a llegar a ellas en 29 s |
| `>> Tiempo maximo de retorno` | La subida duró más de 60 s: revisar el sensor de corriente |
| `AVANCE  I = 1200 mA  F~ 70 N` | Registro periódico: estado, corriente y fuerza estimada |

---

## Solución de problemas

| Problema | Causa probable | Solución |
|---|---|---|
| La placa se detiene apenas parte | El cero del sensor quedó mal medido o la sensibilidad no corresponde | Encender con los relés apagados; revisar `SENSIBILIDAD_V_POR_A` |
| La placa nunca se detiene por el límite y sigue hasta el fondo | El sensor no está en serie con la línea de 12 V, o su salida no llega a A0 | Revisar con `diagnostico.ino` que la corriente marque cerca de 1 A (1000 mA) al moverse |
| Siempre termina por "Tiempo maximo" | La corriente no cae a 0 en los extremos (lectura con ruido u offset) | Revisar el cero del sensor y la tierra común |
| El verde no hace nada | El sistema está en EMERGENCIA con el rojo presionado, o el pulsador está mal cableado | Soltar el rojo; revisar el pulsador con `diagnostico.ino` |
| Los relés hacen "clic" pero el actuador no se mueve | Fuente desenchufada, o puentes cruzados entre relés | Ver la tabla de fallas de `hardware/README.md` |
| La estructura cruje o se deforma al compactar | Demasiadas latas o latas acostadas | Máximo 5 latas paradas; presionar el rojo si se escucha crujir |
