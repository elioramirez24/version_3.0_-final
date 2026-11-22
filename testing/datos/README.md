# Datos de Testing

## Archivos

| Archivo | Contenido |
|---|---|
| `estimacion_fuerza_corriente.csv` | Fuerza estimada del actuador según la corriente, con el modelo F ≈ 350 N/A · (I − 1 A). Incluye la masa equivalente y la situación de cada valor (permitido, límite, no permitido) |
| `grafico_fuerza_corriente.png` | Gráfico del modelo, con el límite de 3 A y la zona donde la estructura de madera cede |

El cálculo y sus supuestos están en `testing/reportes/pruebas_carga_v3.md`.

### Formato de `estimacion_fuerza_corriente.csv`

```csv
corriente_A,fuerza_estimada_N,masa_equivalente_kg,observacion
3.0,700,71,LÍMITE DEL PROGRAMA: máximo que soporta la estructura de madera (estimación del equipo ≈ 700 N)
```

- Separador: coma. Decimales con punto.
- `masa_equivalente_kg` = fuerza / 9,81: la masa cuyo peso equivale a esa fuerza.

---

## Datos pendientes

| Dato | Cómo obtenerlo |
|---|---|
| Corriente real en cada ciclo | Copiar las líneas `I = … A` del Monitor Serie (el programa las imprime cada 250 ms) a un `.csv` con columnas `tiempo_s, estado, corriente_A` |
| Cantidad de latas vs. corriente máxima | Registrar la corriente máxima de ciclos con 1, 2, 3, 4 y 5 latas paradas |
| Altura de las latas antes y después | Medir con regla para calcular el porcentaje de reducción de volumen |
| Testing con usuarios | Encuesta de 5 o más usuarios (facilidad de uso y satisfacción, escala 1-5) |
