# Instrucciones de ensamble - Compactador de latas (v3.0)

**Resultado esperado:** al presionar una vez el pulsador verde, la placa baja y retrocede de inmediato al llegar a 2900 mA (2,9 A) o al final del recorrido. El pulsador rojo detiene todo en cualquier momento.

Las instrucciones de la v1 (control manual en protoboard) están en `iteraciones/iteracion_1/instrucciones_ensamble_v1.md`.

---

## Materiales
- Electrónica y estructura: ver `hardware/bom/README.md`.
- Planos con medidas: `diseno_3d/planos/planos_v3.pdf`.
- Herramientas: destornilladores, taladro con brocas para madera (Ø8, Ø14, Ø30, Ø34, Ø50), multímetro, cinta aislante.

## Reglas antes de empezar
1. **Desenchufar la fuente de 12 V y el USB** antes de mover cualquier cable.
2. Los 12 V nunca deben tocar la protoboard ni el Arduino.
3. **No usar el pin 2 del Arduino** (quedó dañado en la iteración 1).
4. La fuente se conecta a 220 V: armar y revisar su bornera **siempre desenchufada**.

---

## Parte A: estructura

### Paso A1: mueble y refuerzos
1. Armar los dos módulos del mueble (inferior de 470 mm y superior de 380 mm) y unirlos con tornillos.
2. Colocar los rieles laterales y los 2 listones del fondo.
3. **Atornillar las escuadras de acero** que unen el techo (tapa) y el piso con las paredes. Son obligatorias: sin ellas la madera cede con la fuerza del actuador.

### Paso A2: guías y placa
1. Fijar los 2 tubos guía de Ø32 al piso (z = 30 mm) y a la placa guía (z = 500 mm).
2. Perforar la placa compactadora con 2 agujeros de Ø34 (holgura de 1 mm) y deslizarla sobre los tubos.

**Verificación:** la placa debe bajar y subir a mano por los tubos sin trabarse.

### Paso A3: actuador
1. Colgar el actuador de la viga del techo con el perno pivote de Ø8.
2. Unir el extremo del vástago al soporte de horquilla de la placa compactadora.

**Verificación:** con el actuador retraído, la placa queda arriba y la cámara libre (300 mm de alto).

### Paso A4: pulsadores en la tapa
1. Perforar dos agujeros de Ø30 en la tapa, separados 60 mm y a 40 mm del borde frontal: **rojo a la izquierda (PARO)** y **verde a la derecha (COMPACTAR)**.
2. Perforar el paso de cables de Ø14 y bajar por ahí los cables de los pulsadores.

---

## Parte B: circuito de control (5 V)

### Paso B1: rieles de la protoboard
- **5V** del Arduino → riel rojo (+). **GND** del Arduino → riel azul (−).

**Verificación:** confirmar con la serigrafía del Arduino que el cable está en **5V** y no en el GND vecino.

### Paso B2: pulsadores
| Desde | Hasta |
|---|---|
| Pulsador verde, borne 1 | **Pin 6** |
| Pulsador verde, borne 2 | Riel (−) |
| Pulsador rojo, borne 1 | **Pin 3** |
| Pulsador rojo, borne 2 | Riel (−) |

**Por qué no llevan resistencia:** el programa activa la resistencia interna del Arduino (`INPUT_PULLUP`). Suelto, el pin queda en 5 V; al presionar, el pulsador lo une a tierra y el pin lee 0 V.

### Paso B3: relés
| Desde | Hasta |
|---|---|
| VCC de ambos relés | Riel (+) |
| GND de ambos relés | Riel (−) |
| IN del relé 1 (AVANCE) | **Pin 4** |
| IN del relé 2 (RETORNO) | **Pin 5** |

**Verificación:** con el USB conectado se enciende la luz roja de ambos relés.

### Paso B4: sensor de corriente
| Desde | Hasta |
|---|---|
| VCC del sensor | Riel (+) |
| GND del sensor | Riel (−) |
| OUT del sensor | **A0** |

**Verificación:** se enciende la luz roja del sensor.

---

## Parte C: circuito de potencia (12 V), con la fuente DESENCHUFADA

Cada relé tiene tres tornillos: **NO**, **COM** (el del medio) y **NC**.

| Paso | Conexión |
|---|---|
| C1 | Fuente **V+** → **NO** del relé 1; cable corto (puente) de **NO** del relé 1 a **NO** del relé 2 |
| C2 | Fuente **V−** → borne **IP+** del sensor de corriente |
| C3 | Borne **IP−** del sensor → **NC** del relé 1; puente de **NC** del relé 1 a **NC** del relé 2 |
| C4 | Cable 1 del actuador, **solo**, en el **COM** del relé 1 |
| C5 | Cable 2 del actuador, **solo**, en el **COM** del relé 2 |
| C6 | Cable de 220 V a los bornes **L** y **N** de la fuente; cerrar la tapa transparente de la bornera |

**Verificaciones:**
- **Misma posición, mismo polo en ambos relés.** Si el positivo queda en NO en un relé y en NC en el otro (puentes cruzados), en reposo los dos cables del actuador quedan a distinto voltaje: el actuador se mueve solo al enchufar la fuente y se detiene al presionar un pulsador.
- El tornillo debe apretar el **cobre**, no el plástico del cable. Pelar unos 5 mm.
- No deben quedar hilos de cobre sueltos que toquen el tornillo vecino.
- Tirar suavemente de cada cable: no debe salirse.

---

## Parte D: programa y pruebas

### Paso D1: prueba en seco (sin 12 V)
1. Subir `software/src/diagnostico/diagnostico.ino` y abrir el Monitor Serie a 9600 baudios.
2. Los relés deben hacer "clic" uno a la vez, cada 2 segundos.
3. Al presionar cada pulsador, el Monitor Serie debe mostrar `PRESIONADO`.
4. El sensor debe marcar cerca de **2,5 V (0,00 A)**.

### Paso D2: programa definitivo
1. Revisar `SENSIBILIDAD_V_POR_A` según la versión del sensor (impresa en el chip).
2. Subir `software/src/compactador_v3/compactador_v3.ino`.
3. En reposo: luz roja encendida y luz verde apagada en ambos relés.

### Paso D3: prueba con 12 V, sin latas
1. Alejar las manos de la cámara. Enchufar la fuente **sin presionar nada**: el actuador debe quedar **quieto**. Si se mueve solo, desenchufar de inmediato y revisar la Parte C.
2. Presionar una vez el verde: la placa debe **bajar**. Si sube, desenchufar e intercambiar entre sí los dos cables del actuador en los COM.
3. El Monitor Serie debe mostrar cerca de **1000 mA** mientras baja.
4. Al llegar al fondo debe aparecer `Fin de carrera inferior` y la placa debe subir sola.
5. Repetir y presionar el rojo a mitad de camino: el actuador debe detenerse al instante.

**Comprobación con multímetro** (modo V⎓, rango 20 V, puntas sobre las cabezas de los tornillos COM de ambos relés):

| Situación | Lectura correcta |
|---|---|
| Reposo | 0 V |
| Placa bajando (relé 1) | ≈ 12 V |
| Placa subiendo (relé 2) | ≈ −12 V |

### Paso D4: prueba con latas
1. Cargar **como máximo 5 latas paradas**. Nunca latas acostadas.
2. Presionar el verde y observar la corriente en el Monitor Serie: la compactación debe terminar con el mensaje `Umbral de 2900 mA: retroceso inmediato`, y al final debe aparecer la corriente máxima del ciclo (bajo 3000 mA).
3. Si la madera cruje o se deforma, presionar el rojo y revisar las escuadras de acero.

---

## Uso y cuidados
- Ciclo de trabajo del actuador: 10 %. Unos 9 minutos de descanso por cada minuto de uso.
- No aumentar el umbral de 2900 mA del programa mientras la estructura sea de madera.
- Manual de operación completo: `software/docs/README.md`.
