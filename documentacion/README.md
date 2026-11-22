# Documentación del Proyecto

## Contenido

| Documento | Estado | Ubicación |
|---|---|---|
| Actividad de ideación y Avance N°1 (informe y presentación) | Entregados | Retirados del árbol en la v1; se conservan en el historial de git (commit `cc6a7ca`, carpeta `documentacion/avances/`) |
| Reporte final (máx. 15 páginas) | [completar] | `reporte_final/` |
| Presentación final (10 min) | [completar] | `presentacion/` |
| Manual de usuario | ✅ | `software/docs/README.md` (operación) y `hardware/instrucciones_ensamble.md` (armado) |

---

## Dónde está cada contenido técnico del reporte final

| Sección del reporte | Fuente en el repositorio |
|---|---|
| Contexto y problemática ODS 12 | `README.md` (raíz) |
| Solución propuesta | `README.md`, `hardware/README.md`, `diseno_3d/planos/` |
| Proceso de diseño e iteraciones | `iteraciones/` |
| Documentación técnica | `hardware/esquemas/`, `hardware/bom/`, `software/src/` |
| Resultados de testing | `testing/reportes/pruebas_carga_v3.md`, `software/pruebas_pc/README.md` |
| **Limitaciones** | `testing/reportes/pruebas_carga_v3.md` (estructura de madera, 5 latas paradas, umbral de 2,9 A, daño desde 3 A ≈ 700 N) |
| Conclusiones y trabajo futuro | `iteraciones/iteracion_3/README.md` |

## Guion sugerido para la presentación (10 min)
1. Problema y contexto ODS 12 (2 min).
2. Solución técnica: circuito real y ciclo automático (3 min) — usar `hardware/esquemas/circuito_real_v3.png`.
3. Iteraciones y refinamiento: v1 manual → v2 simulada → v3 construida (2 min).
4. Limitación de la estructura y cómo se resolvió (1 min) — usar `testing/datos/grafico_fuerza_corriente.png`.
5. Demostración con 5 latas paradas (1 min).
6. Conclusiones y trabajo futuro (1 min).
