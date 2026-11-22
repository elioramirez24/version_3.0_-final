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
| A | 5 latas paradas: la placa las toca a 180 mm y la corriente sube 0,1 A por mm | Se detiene al llegar a 3 A, sube y queda en reposo |
| B | Cámara vacía | Baja 300 mm hasta el fin de carrera interno (la corriente cae a 0), sube y queda en reposo |
| C | Paro de emergencia a los 10 s | Ambos relés se apagan en la misma vuelta del programa; el verde no hace nada mientras el rojo está presionado; luego el verde solo hace subir la placa |
| D | Pico de arranque de 6 A durante 200 ms | No se confunde con el límite de 3 A |
| E | Algo traba la placa al subir | Se detiene por sobrecorriente y queda en emergencia |

En todas las pruebas se verifica además que **los dos relés nunca estén activos a la vez** (eso pondría en cortocircuito la fuente de 12 V a través de los relés).

## Resultado (22-11-2026)

```
A) 5 latas: se detuvo en x = 201.2 mm (I ~ 3.12 A), volvio arriba. OK
   [   0.13s] >> AVANCE: la placa baja a compactar
   [  33.66s] >> Limite de 3 A (~700 N): fin de la compactacion
   [  33.96s] >> RETORNO: la placa sube
   [  68.30s] >> Fin de carrera superior: listo para otro ciclo
B) sin latas: fin de carrera inferior -> retorno -> reposo. OK
C) paro de emergencia en x = 61.3 mm: reles apagados en el mismo ciclo; retorno con verde. OK
D) pico de arranque ignorado. OK
E) sobrecorriente al subir -> detenido en x = 100.5 mm. OK

Todas las pruebas pasaron. Ambos reles activos a la vez: 0 veces.
```

**Límite de estas pruebas:** el modelo del actuador es una simplificación (velocidad constante de 6 mm/s y corriente que sube en forma lineal al comprimir). Confirman la lógica del programa, no reemplazan la prueba en el prototipo real.
