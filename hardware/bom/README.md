# Bill of Materials (BOM) - v3.0

Los precios y proveedores quedan para completar por el equipo (`[completar]`).

## 1. Electrónica

| Cant. | Componente | Especificación | Código / modelo | Proveedor | Precio unit. | Precio total |
|---|---|---|---|---|---|---|
| 1 | Arduino UNO | ATmega328P, 5 V | UNO R3 | [completar] | [CLP] | [CLP] |
| 2 | Módulo relé 1 canal | Bobina 5 V, contactos 10 A / 30 VDC, disparo alto/bajo (jumper), activo con LOW | Songle SRD-05VDC-SL-C | [completar] | [CLP] | [CLP] |
| 1 | Sensor de corriente | Efecto Hall, salida analógica, bornera de 2 tornillos | Tipo ACS712 — [verificar versión: 05B, 20A o 30A] | [completar] | [CLP] | [CLP] |
| 1 | Actuador lineal | 12 V DC, carrera 300 mm, 1500 N, 6 mm/s, ciclo de trabajo 10 %, fines de carrera internos | ARF03-12-A1-405-300-C11 | [completar] | [CLP] | [CLP] |
| 1 | Fuente de poder | Entrada 220 V AC; salida 12 V DC, 200 W, 16,7 A | LED Power Supply …-200W-12V | [completar] | [CLP] | [CLP] |
| 1 | Pulsador verde | Ø30 mm, momentáneo, montaje en panel | — | [completar] | [CLP] | [CLP] |
| 1 | Pulsador rojo | Ø30 mm, momentáneo, montaje en panel | — | [completar] | [CLP] | [CLP] |
| 1 | Protoboard | 830 puntos | — | [completar] | [CLP] | [CLP] |
| — | Cables jumper | Macho-macho y macho-hembra | — | [completar] | [CLP] | [CLP] |
| — | Cable de potencia | Rojo/negro y amarillo, para 12 V | — | [completar] | [CLP] | [CLP] |
| 1 | Cable de poder 220 V | Con enchufe, para la fuente | — | [completar] | [CLP] | [CLP] |
| 1 | Cargador USB 5 V + cable USB A-B | Alimentación del Arduino | — | [completar] | [CLP] | [CLP] |
| — | Conectores y cinta aislante | Uniones de 12 V y aislación de borneras | — | [completar] | [CLP] | [CLP] |

## 2. Estructura (piezas usadas, según `diseno_3d/planos/planos_v3.pdf`, hoja 3)

| N° | Pieza | Material | Cant. | Medidas (mm) |
|---|---|---|---|---|
| 1 | Placa compactadora (2 tablas) | Madera | 1 | 300 × 300 × 50 |
| 2 | Placa guía + 2 topes | Madera | 1 | 360 × 150 × 20 / 80 × 150 × 30 |
| 3 | Repisa intermedia | Madera | 1 | 310 × 310 × 15 |
| 4 | Viga soporte del actuador + 2 colgadores | Madera | 1 | 260 × 50 / 40 × 90 |
| 5 | Tubo guía | Tubo Ø32 | 2 | Ø32 × 470 |
| 6 | Puerta de carga | Acrílico 3 mm | 1 | 200 × 150 |
| 7 | Actuador lineal 12 V | Aluminio | 1 | Carrera 300 |
| 8 | Mueble (2 módulos) | Madera + acrílico | 1 | 380 × 370 × 850 |
| — | Rieles laterales / listones de fondo | Madera | 2 + 2 | 40 × 780 / 50 × 220 |
| — | Pestillos magnéticos | — | 2 | — |
| 9 | Pulsador paro de emergencia (rojo) | — | 1 | Ø30, en la tapa |
| 10 | Pulsador compactar (verde) | — | 1 | Ø30, en la tapa |
| — | Bisagras, pomo y tornillos | Bronce / madera | var. | — |
| — | Perno pivote del actuador | Acero | 1 | Ø8 |

**No se usaron:** los 2 fines de carrera que figuran en los planos. El programa v3 detecta los extremos del recorrido por la caída de corriente que produce el corte de los fines de carrera internos del actuador.

## 3. Refuerzos agregados en la iteración 3

| Cant. | Material | Uso | Motivo |
|---|---|---|---|
| [completar] | Escuadras de acero galvanizado con tornillos | Unión del techo y del piso con las paredes del mueble | Con la fuerza del actuador, la madera del techo y del piso cedía. Las escuadras reparten la carga en más tornillos (ver `testing/reportes/pruebas_carga_v3.md`) |

---

## Presupuesto total

**Costo total estimado:** $[completar] CLP

| Categoría | Costo |
|---|---|
| Microcontrolador y sensor | $[completar] |
| Relés, pulsadores y protoboard | $[completar] |
| Actuador | $[completar] |
| Fuente de alimentación | $[completar] |
| Estructura (madera, acrílico, tubos, herrajes) | $[completar] |
| Refuerzos (escuadras de acero) | $[completar] |
| Otros (cables, conectores) | $[completar] |

---

## Cambios respecto de la lista de materiales v1

| Cambio | Antes (v1) | Ahora (v3) | Motivo |
|---|---|---|---|
| Fuente | Transformador 12 V 2 A (defectuoso) y fuente de reemplazo | Fuente LED 12 V 200 W (16,7 A) | Tener capacidad de corriente de sobra para compactar |
| Sensor | Ninguno | Sensor de corriente tipo ACS712 | Limitar la fuerza y detectar los extremos del recorrido |
| Pulsadores | Táctiles de 6 × 6 mm en la protoboard | Ø30 mm en la tapa | Uso real por personas, botón de paro visible |
| Estructura | Modelo en acero de 450 × 450 × 1000 mm (Fusion 360) | Mueble de madera de 380 × 370 × 850 mm | La construcción en madera ya estaba definida en la v1; el modelo usaba acero solo como material asignado |
| Refuerzos | — | Escuadras de acero en techo y piso | La madera cedía con la fuerza del actuador |
| Alimentación del Arduino | USB del computador | Cargador USB 5 V | Funcionamiento sin computador |

---

## Componentes recomendados para una versión siguiente

ℹ **[INFORMACIÓN COMPLEMENTARIA AGREGADA]**
- Marco de acero o aluminio (o tirantes metálicos entre techo y piso) para aprovechar los 1500 N del actuador.
- Fusible de 5 A en la salida de 12 V e interruptor general.
- Placa perforada soldada en lugar de la protoboard.
- Celda de carga con módulo HX711 para medir la fuerza real.
- Microinterruptor en la puerta de carga para que el ciclo solo funcione con la puerta cerrada.
