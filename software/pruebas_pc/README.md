# Pruebas del programa en un PC

Permiten comprobar la lógica de `software/src/compactador_v3/compactador_v3.ino` sin el Arduino ni el actuador, con un modelo simple del actuador y del sensor de corriente.

## Cómo ejecutarlas

Se necesita un compilador de C++ (`g++` en Linux o macOS):

```bash
cd software/pruebas_pc
g++ -std=c++17 -o pruebas pruebas_ciclo.cpp && ./pruebas
```

`pruebas_ciclo.cpp` incluye directamente el archivo `.ino`, así que siempre se prueba el programa real. `Arduino.h` es una versión mínima que imita `millis()`, `digitalRead()`, `analogRead()`, los relés y el Monitor Serie.

## Qué se comprueba

| Prueba | Situación simulada | Resultado esperado |
|---|---|---|
| A | 5 latas paradas: la placa las toca a 120 mm y la corriente sube 0,1 A por mm | Corta al llegar a 2900 mA, la corriente real nunca alcanza 3 A y el retroceso empieza a los ≈ 50 ms; sube y queda en reposo |
| B | Cámara vacía | A los 29 s se cumple el tiempo máximo de bajada; la placa sube y queda en reposo |
| C | Paro de emergencia a los 10 s | Ambos relés se apagan en la misma vuelta del programa; el verde no hace nada mientras el rojo está presionado; luego el verde solo hace subir la placa |
| D | Pico de arranque de 6 A durante 200 ms | No se confunde con el umbral de 2900 mA |
| E | Algo traba la placa al subir | Se detiene por sobrecorriente y queda en emergencia |

En todas las pruebas se verifica además que **los dos relés nunca estén activos a la vez** (el actuador quedaría con sus dos cables en +12 V y no se movería).

## Resultado (22-11-2026)

```
A) 5 latas: corte en x = 139.338 mm; corriente real maxima = 2.932 A (< 3 A); retroceso a los 51 ms del corte. OK
   [   0.13s] >> AVANCE: la placa baja a compactar
   [  23.35s] >> Umbral de 2900 mA: retroceso inmediato
   [  23.40s] >> RETORNO: la placa sube
   [  47.43s] Corriente maxima del ciclo: 2933 mA
   [  47.43s] >> Fin de carrera superior: listo para otro ciclo
B) sin latas: a los 29 s se detiene por tiempo (x = 174.006 mm) -> retorno -> reposo. OK
C) paro de emergencia en x = 61.344 mm: reles apagados en el mismo ciclo; retorno con verde. OK
D) pico de arranque ignorado. OK
E) sobrecorriente al subir -> detenido en x = 100.494 mm. OK

Todas las pruebas pasaron. Ambos reles activos a la vez: 0 veces.
```
En la prueba A el corte ocurre a 2,93 A reales y no exactamente a 2,90 A: el Arduino mide en escalones de unos 49 mA con el ACS712-20A (ver `software/src/README.md`). Aun así, la corriente nunca llega a 3 A.

**Límite de estas pruebas:** el modelo del actuador es una simplificación (velocidad constante de 6 mm/s y corriente que sube en forma lineal al comprimir). Con esa velocidad, en 29 s la placa recorre unos 174 mm; la distancia real hasta las latas depende del montaje. Confirman la lógica del programa, no reemplazan la prueba en el prototipo real.
