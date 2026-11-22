# Testing y Validación

## Objetivo
Validar el funcionamiento del prototipo, conocer sus límites reales y medir su aporte a la problemática del ODS 12 (producción y consumo responsables).

---

## Carpetas

| Carpeta | Contenido |
|---|---|
| `reportes/` | Reporte de pruebas de carga del prototipo v3 (`pruebas_carga_v3.md`) |
| `datos/` | Estimación de fuerza según la corriente (CSV y gráfico) |
| `evidencias/` | Video del diagnóstico de la fuente de la v1; las fotos del prototipo final están en `hardware/fotos/` |

---

## Pruebas realizadas

| Prueba | Iteración | Resultado | Dónde está |
|---|---|---|---|
| Control manual del actuador con 2 relés | v1 | ✅ Sale y entra con los botones (tras reemplazar la fuente defectuosa) | `iteraciones/iteracion_1/README.md` |
| Lógica del ciclo automático | v3 | ✅ 5 escenarios correctos; los dos relés nunca activos a la vez | `software/pruebas_pc/README.md` |
| Carga en el prototipo real | v3 | ⚠️ Funciona con **máximo 5 latas paradas**; sobre 3 A (≈ 700 N) la estructura de madera cede; latas acostadas no permitidas | `reportes/pruebas_carga_v3.md` |
| Refuerzo con escuadras de acero | v3 | ✅ Techo y piso reforzados | `hardware/fotos/v3_08_*.jpg`, `v3_09_*.jpg` |
| Testing con usuarios (mín. 5) | — | Pendiente | — |

---

## Metodología para el testing con usuarios (pendiente)

### Grupo objetivo
- **Perfil:** comunidad de la Universidad Adolfo Ibáñez (estudiantes y funcionarios que reciclan latas).
- **Cantidad mínima:** 5 usuarios.
- **Ubicación:** campus UAI, junto a un punto de reciclaje.

### Protocolo
1. **Explicación breve (5 min):** propósito del compactador y reglas de uso (máximo 5 latas paradas).
2. **Uso sin ayuda (10 min):** el usuario carga latas, cierra la puerta y presiona el pulsador verde. Se observa sin intervenir.
3. **Entrevista (5 min):** qué entendió, qué le costó, qué cambiaría.
4. **Encuesta (2 min):** facilidad de uso, claridad y satisfacción, de 1 a 5.

### Métricas
- **Usabilidad:** facilidad de uso (1-5), claridad de las instrucciones (1-5), satisfacción (1-5), errores de uso por sesión (por ejemplo, latas acostadas o más de 5).
- **Impacto ODS 12:** reducción de altura de cada lata (%), latas que caben en el contenedor antes y después de compactar.
- **Desempeño técnico:** duración del ciclo (s), corriente máxima por ciclo (A), ciclos sin falla.

---

## Validación de impacto ODS

**ODS seleccionado:** 12 - Producción y consumo responsables.

**Meta del proyecto:** aprovechar mejor la capacidad de los contenedores de reciclaje compactando las latas al momento de depositarlas.

**Indicador principal:** latas almacenadas por volumen de contenedor. Según el planteamiento del proyecto, una lata compactada hasta cerca del 30 % de su altura permite guardar unas tres veces más latas en el mismo espacio. Este valor debe confirmarse midiendo las latas compactadas por el prototipo v3, considerando que el límite de 700 N puede dejarlas menos compactadas que lo planteado.
