# Historial de Iteraciones

## Propósito

Esta carpeta documenta la evolución del prototipo a través de las versiones desarrolladas durante el curso. El historial de git también se conserva completo: la etiqueta `v1.0` marca el estado del repositorio al cerrar la iteración 1 (importado de `proyecto_final_grupo_4`) y `v3.0` el estado final.

```
iteraciones/
├── iteracion_1/    # Control manual del actuador en banco de pruebas (v1.0)
├── iteracion_2/    # Ciclo automático simulado en Wokwi y diseño en madera (v2.0)
└── iteracion_3/    # Prototipo final construido (v3.0)
```

---

## Tabla comparativa de iteraciones

| Aspecto | v1 | v2 | v3 |
|---------|----|----|----|
| **Fecha** | Ago - oct 2026 | Oct - nov 2026 | Nov 2026 |
| **Control** | Manual: se mueve mientras se mantiene presionado un botón | Automático de un toque (simulado) | Automático de un toque (real) |
| **Sensores** | Ninguno | HC-SR04 y potenciómetro como sensor de corriente (simulados) | Sensor de corriente tipo ACS712 |
| **Actuadores** | Actuador lineal 12 V, 1500 N, 300 mm | LED en lugar del actuador | Actuador lineal 12 V, 1500 N, 300 mm |
| **Seguridad** | Ninguna | Paro de emergencia | Paro de emergencia + límite de 3 A |
| **Alimentación** | Transformador 12 V 2 A (defectuoso) → fuente de reemplazo | — | Fuente 12 V 200 W + cargador USB 5 V |
| **Estructura** | Modelo de acero en Fusion 360 (450 × 450 × 1000 mm) | Diseño en madera | Mueble de madera de 380 × 370 × 850 mm con escuadras de acero |
| **Conectividad** | Ninguna | Ninguna | Ninguna |
| **Encapsulado** | ❌ Banco de pruebas | Diseño | ✅ Mueble construido |
| **Testing** | Solo equipo | Simulación | Equipo: pruebas de carga · usuarios: pendiente |
| **Capacidad** | — | — | Máximo 5 latas paradas |

---

## Evolución del diseño

### Versión 1 (Iteración 1) - Concepto inicial
**Estado:** prueba de concepto en banco de pruebas.
- Arduino UNO + 2 relés en puente H + actuador lineal de 12 V, controlado con 2 botones.
- **Problemas principales:** código que no correspondía al circuito, pin 2 dañado, puentes de 12 V mal ubicados y transformador defectuoso.
- **Aprendizaje clave:** comprobar por partes (5 V en seco, luego la fuente sola, luego 12 V).

### Versión 2 (Iteración 2) - Ciclo automático simulado
**Estado:** lógica del ciclo validada en Wokwi; estructura rediseñada en madera.
- **Mejoras:** ciclo de un toque, paro de emergencia, medición de corriente (potenciómetro).
- **Pendiente:** probar en el prototipo real y conocer la resistencia de la madera.

### Versión 3 (Iteración 3) - Versión final
**Estado:** prototipo construido y funcionando.
- **Mejoras:** sensor de corriente real, fuente de 200 W, pulsadores Ø30 en la tapa, refuerzo con escuadras de acero.
- **Limitación:** la estructura de madera cede sobre ≈ 700 N (3 A): máximo 5 latas paradas y sin latas acostadas.

---

## Justificación de cambios

### Hardware
**De v1 a v2:**
- Cambio: medición de corriente y paro de emergencia.
- Razón: un compactador automático necesita saber cuándo detenerse y poder pararse en cualquier momento.

**De v2 a v3:**
- Cambio: potenciómetro → sensor de corriente real; fuente de 200 W; escuadras de acero.
- Razón: llevar la simulación al prototipo y resolver la falta de resistencia de la estructura.
- Impacto: el prototipo funciona de forma segura con hasta 5 latas paradas.

### Software
**De v1 a v2:** de "mover mientras se presiona" a un ciclo automático con paro de emergencia.  
**De v2 a v3:** corte a 3 A, detección de fin de recorrido por corriente, filtro del pico de arranque y pruebas en PC.

### Diseño 3D
**De v1 a v2:** del modelo en acero (135 kg) a un mueble de madera fabricable.  
**De v2 a v3:** planos definitivos de fabricación (3 hojas) y refuerzos de acero agregados después de las pruebas.

---

## Lecciones aprendidas

### Técnicas
- Verificar que los pines del código coincidan con el cableado antes de subirlo.
- La corriente del motor permite estimar y limitar la fuerza sin un sensor de fuerza.
- La estructura que sostiene al actuador recibe la misma fuerza que las latas: el material de la estructura define el límite del sistema.

### De proceso
- Simular la lógica antes de construir ahorra tiempo y protege los componentes.
- Probar el circuito completo fuera del mueble antes de integrarlo.

### De trabajo en equipo
- Documentar cada falla y su solución en el repositorio evitó repetir errores entre iteraciones.
