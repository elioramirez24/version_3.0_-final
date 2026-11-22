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
4. La placa se detiene sola cuando la fuerza llega al límite (3 A ≈ 700 N) o al final de su recorrido, y vuelve a subir.
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
- **Ciclo de trabajo del actuador: 10 %.** Por cada minuto de movimiento, dejar unos 9 minutos de descanso. Un ciclo completo dura cerca de 1 a 1,7 minutos.
- No aumentar el límite de corriente del programa: la estructura de madera cede sobre ≈ 700 N.

---

## Funciones principales de `compactador_v3.ino`

**`leerCorriente()`**
- **Propósito:** medir la corriente del actuador en amperes.
- **Retorna:** `float`, corriente en A, siempre positiva (el signo depende del sentido de giro).
- **Cómo funciona:** promedia 20 lecturas de A0, resta el voltaje de cero medido al encender y divide por la sensibilidad del sensor.

**`mantenida(condicion, temporizador, duracionMs)`**
- **Propósito:** confirmar que una condición se cumple durante un tiempo mínimo seguido.
- **Retorna:** `true` cuando la condición lleva `duracionMs` milisegundos cumpliéndose.
- **Ejemplo:**
```cpp
// Detiene el avance solo si la corriente supera 3 A durante 150 ms seguidos
if (mantenida(corrienteA >= CORRIENTE_LIMITE_A, tCorrienteAlta, T_CONFIRMAR_LIMITE_MS)) { ... }
```

**`activarAvance()` / `activarRetorno()` / `detener()`**
- **Propósito:** mover o detener el actuador.
- **Regla:** antes de encender un relé siempre se apaga el otro.

**`fuerzaEstimadaN(corriente)`**
- **Propósito:** mostrar en el Monitor Serie una estimación de la fuerza.
- **Fórmula:** F ≈ 350 N/A · (I − 1 A). Es una aproximación (ver `testing/reportes/pruebas_carga_v3.md`), no una medición.

---

## Mensajes del Monitor Serie

| Mensaje | Significado |
|---|---|
| `Compactador v3 listo. Cero del sensor: 2.5xx V` | Encendido correcto. Si el cero está lejos de 2,5 V, revisar el sensor |
| `>> AVANCE: la placa baja a compactar` | Comenzó el ciclo |
| `>> Limite de 3 A (~700 N): fin de la compactacion` | Se alcanzó la fuerza máxima permitida |
| `>> Fin de carrera inferior` | La placa llegó al fondo sin alcanzar el límite (cámara vacía o latas ya compactadas) |
| `>> RETORNO: la placa sube` | La placa vuelve a su posición inicial |
| `>> Fin de carrera superior: listo para otro ciclo` | Ciclo terminado |
| `>> PARO DE EMERGENCIA: reles apagados` | Se presionó el rojo |
| `>> Sobrecorriente al subir: revisar la placa` | Algo traba la subida; el sistema queda detenido |
| `>> Tiempo maximo de avance` / `de retorno` | El movimiento duró más de 60 s: revisar el sensor de corriente |
| `AVANCE  I = 1.20 A  F~ 70 N` | Registro periódico: estado, corriente y fuerza estimada |

---

## Solución de problemas

| Problema | Causa probable | Solución |
|---|---|---|
| La placa se detiene apenas parte | El cero del sensor quedó mal medido o la sensibilidad no corresponde | Encender con los relés apagados; revisar `SENSIBILIDAD_V_POR_A` |
| La placa nunca se detiene por el límite y sigue hasta el fondo | El sensor no está en serie con la línea de 12 V, o su salida no llega a A0 | Revisar con `diagnostico.ino` que la corriente marque cerca de 1 A al moverse |
| Siempre termina por "Tiempo maximo" | La corriente no cae a 0 en los extremos (lectura con ruido u offset) | Revisar el cero del sensor y la tierra común |
| El verde no hace nada | El sistema está en EMERGENCIA con el rojo presionado, o el pulsador está mal cableado | Soltar el rojo; revisar el pulsador con `diagnostico.ino` |
| Los relés hacen "clic" pero el actuador no se mueve | Fuente desenchufada, o puentes cruzados entre relés | Ver la tabla de fallas de `hardware/README.md` |
| La estructura cruje o se deforma al compactar | Demasiadas latas o latas acostadas | Máximo 5 latas paradas; presionar el rojo si se escucha crujir |
