# Genera circuito_real_v3.html con el montaje físico del compactador v3.
# Luego se convierte a PNG con render_png.py (Chrome sin interfaz + Playwright).
import html

W, H = 1748, 900          # lienzo del circuito (unidades SVG)
BG = "#2b2b2b"            # fondo del lienzo (también se usa para el efecto de cruce de cables)

svg = []
def add(s): svg.append(s)

# ---------------------------------------------------------------- utilidades
def wire(points, color, width=4, gap=True):
    """Cable ortogonal. Primero se traza un contorno del color del fondo: así, cuando
    un cable cruza a otro, se ve pasando por encima (en los cruces NO hay conexión)."""
    d = "M" + " L".join(f"{x},{y}" for x, y in points)
    if gap:
        add(f'<path d="{d}" fill="none" stroke="{BG}" stroke-width="{width+7}" stroke-linejoin="round" stroke-linecap="butt"/>')
    add(f'<path d="{d}" fill="none" stroke="{color}" stroke-width="{width}" stroke-linejoin="round" stroke-linecap="round"/>')

def dot(x, y, color="#f8fafc", r=5.5):
    add(f'<circle cx="{x}" cy="{y}" r="{r}" fill="{color}" stroke="#111" stroke-width="1.5"/>')

def text(x, y, s, size=13, color="#e8e8e8", anchor="start", weight="400", rotate=None, ls=0):
    rot = f' transform="rotate({rotate} {x} {y})"' if rotate is not None else ""
    add(f'<text x="{x}" y="{y}" font-family="Inter, DejaVu Sans, sans-serif" font-size="{size}" fill="{color}" '
        f'text-anchor="{anchor}" font-weight="{weight}" letter-spacing="{ls}"{rot}>{html.escape(s)}</text>')

def tag(x, y, s, fg, bg, anchor="start"):
    """Etiqueta pequeña con el nombre del pin junto a un cable."""
    w = 7.2 * len(s) + 10
    x0 = x if anchor == "start" else x - w
    add(f'<rect x="{x0}" y="{y-11}" width="{w}" height="17" rx="4" fill="{bg}" opacity=".95"/>')
    text(x0 + w / 2, y + 2, s, size=11, color=fg, anchor="middle", weight="700")

callouts = []
def callout(n, cx, cy, ax, ay):
    callouts.append((n, cx, cy, ax, ay))

C = {1: "#2563eb", 2: "#16a34a", 3: "#dc2626", 4: "#7c3aed", 5: "#ea580c",
     6: "#be185d", 7: "#0891b2", 8: "#ca8a04", 9: "#64748b", 10: "#4f46e5"}

# colores de cables
R12, N12 = "#e11d1d", "#0a0a0a"          # potencia 12 V
M1C, M2C = "#f59e0b", "#a16207"          # cables del actuador
V5, GND = "#f87171", "#000000"           # control 5 V / tierra
S_VERDE, S_ROJO, S_R1, S_R2, S_A0 = "#22c55e", "#fb923c", "#c084fc", "#38bdf8", "#facc15"

add(f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {W} {H}" style="width:100%;height:auto;display:block">')
add('<defs>'
    '<linearGradient id="alu" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#eef0f3"/>'
    '<stop offset=".5" stop-color="#bfc4ca"/><stop offset="1" stop-color="#8d939b"/></linearGradient>'
    '<linearGradient id="psu" x1="0" y1="0" x2="1" y2="1"><stop offset="0" stop-color="#a3a9b1"/>'
    '<stop offset="1" stop-color="#5d636b"/></linearGradient>'
    '<linearGradient id="rod" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#ffffff"/>'
    '<stop offset="1" stop-color="#a3a8ae"/></linearGradient>'
    '<pattern id="hex" width="26" height="22" patternUnits="userSpaceOnUse">'
    '<polygon points="7,2 19,2 24,11 19,20 7,20 2,11" fill="#2b2f35"/></pattern>'
    '</defs>')
add(f'<rect x="0" y="0" width="{W}" height="{H}" rx="14" fill="{BG}"/>')

# ================================================================ COMPONENTES
# ---- TAPA + PULSADORES Ø30 (2 = verde, 3 = rojo). Rojo a la izquierda, como en la tapa real.
add('<rect x="210" y="28" width="360" height="160" rx="8" fill="#161616" stroke="#4b4b4b" stroke-width="2"/>')
text(222, 50, "TAPA DEL MUEBLE (vista superior)", size=12, color="#9ca3af", weight="600", ls=0.5)
for x in (230, 552):
    for y in (40, 176):
        add(f'<circle cx="{x}" cy="{y}" r="3" fill="#777"/>')
def boton(cx, cy, cap, ring):
    add(f'<circle cx="{cx}" cy="{cy}" r="38" fill="#c8a25a" stroke="#7a6232" stroke-width="2"/>')
    add(f'<circle cx="{cx}" cy="{cy}" r="32" fill="#d4d7db" stroke="#8a8f96" stroke-width="3"/>')
    add(f'<circle cx="{cx}" cy="{cy}" r="25" fill="{ring}"/>')
    if cap != ring:
        add(f'<circle cx="{cx}" cy="{cy}" r="17" fill="{cap}"/>')
boton(330, 104, "#e53935", "#2e9e6b")
boton(452, 104, "#2fae74", "#2fae74")
text(330, 160, "PARO", size=11, color="#fca5a5", anchor="middle", weight="700")
text(452, 160, "COMPACTAR", size=11, color="#86efac", anchor="middle", weight="700")
BT = {"rojo_gnd": (310, 178), "rojo_sig": (350, 178), "verde_sig": (432, 178), "verde_gnd": (472, 178)}
for (x, y) in BT.values():
    add(f'<rect x="{x-6}" y="{y-6}" width="12" height="11" fill="#c9a227" stroke="#6b5713"/>')

# ---- PROTOBOARD (9)
add('<rect x="140" y="222" width="720" height="70" rx="6" fill="#f1efe8" stroke="#cfcac0" stroke-width="2"/>')
add('<line x1="160" y1="238" x2="840" y2="238" stroke="#e05a5a" stroke-width="2"/>')
add('<line x1="160" y1="276" x2="840" y2="276" stroke="#4a7fd6" stroke-width="2"/>')
for i in range(46):
    x = 165 + i * 15
    add(f'<rect x="{x-2}" y="{249}" width="4" height="4" fill="#9b968c"/>')
    add(f'<rect x="{x-2}" y="{262}" width="4" height="4" fill="#9b968c"/>')
text(148, 243, "+", size=15, color="#d33", weight="700")
text(148, 282, "−", size=17, color="#2c64c9", weight="700")
RAIL_P, RAIL_N = 238, 276

# ---- ARDUINO UNO (1)
AX, AY, AW, AH = 150, 340, 420, 285
add(f'<rect x="{AX}" y="{AY}" width="{AW}" height="{AH}" rx="12" fill="#0b7a82" stroke="#06535a" stroke-width="3"/>')
add(f'<rect x="{AX-34}" y="{AY+30}" width="64" height="62" rx="4" fill="url(#alu)" stroke="#6b7078"/>')   # USB-B
add(f'<rect x="{AX-24}" y="{AY+196}" width="52" height="58" rx="5" fill="#141414" stroke="#000"/>')     # jack DC
add(f'<rect x="{AX+150}" y="{AY+176}" width="230" height="44" rx="3" fill="#1c1c1c"/>')                # ATmega328P
for i in range(14):
    add(f'<rect x="{AX+158+i*16}" y="{AY+170}" width="7" height="6" fill="#cfcfcf"/>')
    add(f'<rect x="{AX+158+i*16}" y="{AY+220}" width="7" height="6" fill="#cfcfcf"/>')
text(AX+150, AY+128, "UNO", size=34, color="#ffffff", weight="700")
text(AX+238, AY+128, "ARDUINO", size=18, color="#d1fae5", weight="700", ls=1)
add(f'<circle cx="{AX+384}" cy="{AY+118}" r="6" fill="#4ade80"/>')
text(AX+396, AY+123, "ON", size=11, color="#ecfeff")
# cabezal digital (arriba)
dig = {k: AX + 390 - 18 * k for k in range(8)}
dig.update({k: AX + 390 - 18 * 7 - 26 - 18 * (k - 8) for k in range(8, 14)})
HY = AY + 15
add(f'<rect x="{dig[13]-10}" y="{HY-8}" width="{dig[8]-dig[13]+20}" height="16" fill="#111"/>')
add(f'<rect x="{dig[7]-10}" y="{HY-8}" width="{dig[0]-dig[7]+20}" height="16" fill="#111"/>')
for k, x in dig.items():
    add(f'<rect x="{x-4}" y="{HY-4}" width="8" height="8" fill="#5b5b5b"/>')
    text(x, HY+24, str(k), size=10, color="#e0f2f1", anchor="middle")
text(AX+300, HY+42, "DIGITAL", size=10, color="#bfe3e3", anchor="middle", ls=1)
# cabezal de potencia y analógico (abajo)
BY = AY + AH - 15
pw = {"IOREF": AX+150, "RESET": AX+168, "3.3V": AX+186, "5V": AX+204, "GND": AX+222, "GND ": AX+240, "Vin": AX+258}
an = {f"A{i}": AX+300+18*i for i in range(6)}
add(f'<rect x="{AX+140}" y="{BY-8}" width="128" height="16" fill="#111"/>')
add(f'<rect x="{AX+290}" y="{BY-8}" width="110" height="16" fill="#111"/>')
for name, x in list(pw.items()) + list(an.items()):
    add(f'<rect x="{x-4}" y="{BY-4}" width="8" height="8" fill="#5b5b5b"/>')
    text(x+4, BY-14, name.strip(), size=9, color="#e0f2f1", rotate=-90)
text(AX+345, BY-58, "ANALOG IN", size=10, color="#bfe3e3", anchor="middle", ls=1)

# ---- RELÉS (5, 6): pines de control a la izquierda, borneras a la derecha
def rele(x, y, nombre):
    add(f'<rect x="{x}" y="{y}" width="230" height="130" rx="8" fill="#b91c1c" stroke="#7f1d1d" stroke-width="2"/>')
    add(f'<rect x="{x+62}" y="{y+16}" width="112" height="98" rx="4" fill="#1d4ed8" stroke="#1e3a8a" stroke-width="2"/>')
    text(x+118, y+56, "SONGLE", size=12, color="#dbeafe", anchor="middle", weight="700")
    text(x+118, y+74, "SRD-05VDC", size=9, color="#dbeafe", anchor="middle")
    text(x+118, y+92, "10 A · 30 VDC", size=9, color="#dbeafe", anchor="middle")
    add(f'<rect x="{x+184}" y="{y+12}" width="40" height="106" rx="3" fill="#2563eb" stroke="#1e3a8a"/>')
    for i, t in enumerate(("NO", "COM", "NC")):
        cy = y + 30 + 35 * i
        add(f'<circle cx="{x+204}" cy="{cy}" r="10" fill="#cbd5e1" stroke="#475569" stroke-width="2"/>')
        add(f'<line x1="{x+197}" y1="{cy}" x2="{x+211}" y2="{cy}" stroke="#475569" stroke-width="2"/>')
        text(x+180, cy+4, t, size=10, color="#fff", anchor="end", weight="700")
    for i, t in enumerate(("VCC", "GND", "IN")):
        cy = y + 30 + 35 * i
        add(f'<rect x="{x-6}" y="{cy-5}" width="12" height="10" fill="#d4af37" stroke="#7a5f12"/>')
        text(x+12, cy+4, t, size=10, color="#fff", weight="700")
    add(f'<circle cx="{x+40}" cy="{y+118}" r="4" fill="#ff4d4d"/>')
    add(f'<circle cx="{x+53}" cy="{y+118}" r="4" fill="#7ee787"/>')
    add(f'<rect x="{x+42}" y="{y+8}" width="10" height="14" fill="#facc15"/>')
    text(x+115, y+152, nombre, size=13, color="#fde68a", anchor="middle", weight="700")
    return {"VCC": (x-6, y+30), "GND": (x-6, y+65), "IN": (x-6, y+100),
            "NO": (x+214, y+30), "COM": (x+214, y+65), "NC": (x+214, y+100)}
r1 = rele(910, 340, "Relé 1 · AVANCE (baja la placa)")
r2 = rele(910, 560, "Relé 2 · RETORNO (sube la placa)")

# ---- SENSOR DE CORRIENTE (4)
SX, SY = 1010, 100
add(f'<rect x="{SX}" y="{SY}" width="125" height="104" rx="6" fill="#1e3a8a" stroke="#172554" stroke-width="2"/>')
add(f'<rect x="{SX+92}" y="{SY+10}" width="40" height="84" rx="3" fill="#22c55e" stroke="#14532d" stroke-width="2"/>')
for i in range(2):
    cy = SY + 30 + 44 * i
    add(f'<circle cx="{SX+112}" cy="{cy}" r="10" fill="#cbd5e1" stroke="#475569" stroke-width="2"/>')
    add(f'<line x1="{SX+105}" y1="{cy}" x2="{SX+119}" y2="{cy}" stroke="#475569" stroke-width="2"/>')
add(f'<rect x="{SX+42}" y="{SY+34}" width="34" height="26" rx="2" fill="#111"/>')
text(SX+58, SY+82, "ACS712", size=11, color="#dbeafe", anchor="middle", weight="700")
add(f'<circle cx="{SX+26}" cy="{SY+92}" r="4" fill="#ff4d4d"/>')
for i, t in enumerate(("VCC", "OUT", "GND")):
    cy = SY + 22 + 28 * i
    add(f'<rect x="{SX-6}" y="{cy-5}" width="12" height="10" fill="#d4af37" stroke="#7a5f12"/>')
    text(SX+10, cy+4, t, size=10, color="#fff", weight="700")
text(SX+112, SY+4, "IP+", size=10, color="#bbf7d0", anchor="middle", weight="700")
text(SX+112, SY+112, "IP−", size=10, color="#bbf7d0", anchor="middle", weight="700")
S = {"VCC": (SX-6, SY+22), "OUT": (SX-6, SY+50), "GND": (SX-6, SY+78),
     "IP+": (SX+122, SY+30), "IP-": (SX+122, SY+74)}

# ---- FUENTE 12 V 200 W (7): bornera en el costado izquierdo
PX, PY, PWD, PHT = 1395, 60, 280, 200
add(f'<rect x="{PX}" y="{PY}" width="{PWD}" height="{PHT}" rx="8" fill="url(#psu)" stroke="#3f444a" stroke-width="2"/>')
add(f'<rect x="{PX+70}" y="{PY+14}" width="{PWD-84}" height="{PHT-28}" fill="url(#hex)" opacity=".55"/>')
add(f'<rect x="{PX+88}" y="{PY+66}" width="176" height="70" rx="4" fill="#e5e7eb" opacity=".95"/>')
text(PX+176, PY+86, "LED POWER SUPPLY", size=11, color="#111", anchor="middle", weight="700")
text(PX+176, PY+105, "12 V DC · 200 W", size=14, color="#111", anchor="middle", weight="700")
text(PX+176, PY+124, "salida máx. 16,7 A", size=11, color="#111", anchor="middle")
add(f'<rect x="{PX-14}" y="{PY+8}" width="58" height="{PHT-16}" rx="4" fill="#2563eb" stroke="#1e3a8a" stroke-width="2"/>')
pt = []
for i, t in enumerate(("L", "N", "⏚", "V−", "V−", "V+", "V+")):
    cy = PY + 26 + 25 * i
    add(f'<circle cx="{PX+4}" cy="{cy}" r="8" fill="#cbd5e1" stroke="#475569" stroke-width="2"/>')
    add(f'<line x1="{PX-1}" y1="{cy-4}" x2="{PX+9}" y2="{cy+4}" stroke="#475569" stroke-width="2"/>')
    text(PX+28, cy+4, t, size=11, color="#fff", anchor="middle", weight="700")
    pt.append((PX-4, cy))
P_L, P_N, P_VN, P_VP = pt[0], pt[1], pt[3], pt[6]
# cable de red 220 V
wire([P_L, (1372, P_L[1]), (1372, 24)], "#8b5a2b", 4, gap=False)
wire([P_N, (1356, P_N[1]), (1356, 24)], "#3b82f6", 4, gap=False)
add('<rect x="1338" y="8" width="52" height="18" rx="4" fill="#3a3a3a" stroke="#888"/>')
text(1330, 22, "a 220 V AC (enchufe)", size=11, color="#d1d5db", anchor="end", weight="600")

# ---- ACTUADOR LINEAL (8), horizontal; el vástago real baja la placa compactadora
TX, TY = 1250, 735
add(f'<rect x="{TX}" y="{TY}" width="112" height="104" rx="10" fill="url(#alu)" stroke="#6b7078" stroke-width="2"/>')     # motor
add(f'<rect x="{TX+112}" y="{TY+14}" width="58" height="76" rx="6" fill="url(#alu)" stroke="#6b7078" stroke-width="2"/>')  # reductora
add(f'<rect x="{TX+170}" y="{TY+26}" width="230" height="52" rx="6" fill="url(#alu)" stroke="#6b7078" stroke-width="2"/>') # tubo
add(f'<rect x="{TX+400}" y="{TY+40}" width="38" height="24" rx="3" fill="url(#rod)" stroke="#7b8087"/>')                 # vástago
add(f'<rect x="{TX+400}" y="{TY+40}" width="80" height="24" rx="3" fill="none" stroke="#cbd5e1" stroke-dasharray="5 4"/>')
add(f'<path d="M{TX+438},{TY+34} h20 a18,18 0 0 1 0,36 h-20 z" fill="url(#alu)" stroke="#6b7078"/>')                      # horquilla
add(f'<circle cx="{TX+458}" cy="{TY+52}" r="6" fill="#2b2b2b"/>')
add(f'<path d="M{TX+18},{TY+52} h-16 a14,14 0 0 0 0,0" fill="none"/>')
text(TX+285, TY+48, "12 V · 1500 N máx.", size=12, color="#1f2937", anchor="middle", weight="700")
text(TX+285, TY+66, "carrera 300 mm · 6 mm/s", size=11, color="#1f2937", anchor="middle")
text(TX+56, TY+58, "MOTOR", size=11, color="#374151", anchor="middle", weight="700")
text(TX+462, TY+100, "+300 mm", size=10, color="#cbd5e1", anchor="middle")
ACT_A, ACT_B = (TX, TY+30), (TX, TY+74)
for (x, y) in (ACT_A, ACT_B):
    add(f'<rect x="{x-8}" y="{y-5}" width="10" height="10" fill="#d4af37" stroke="#7a5f12"/>')
text(TX+8, TY+22, "cable 2", size=10, color="#374151", weight="700")
text(TX+8, TY+94, "cable 1", size=10, color="#374151", weight="700")

# ---- CARGADOR USB 5 V (10)
add('<rect x="22" y="690" width="92" height="92" rx="16" fill="#f5f5f4" stroke="#a8a29e" stroke-width="2"/>')
add('<rect x="44" y="676" width="10" height="16" fill="#a8a29e"/><rect x="82" y="676" width="10" height="16" fill="#a8a29e"/>')
text(68, 735, "USB", size=13, color="#44403c", anchor="middle", weight="700")
text(68, 752, "5 V", size=13, color="#44403c", anchor="middle", weight="700")

# ================================================================ CABLES
# -- Cable USB (cargador -> Arduino)
wire([(68, 690), (68, AY+61), (AX-34, AY+61)], "#6b7280", 7, gap=False)

# -- 5 V y GND del Arduino a los rieles de la protoboard
P5V, PGND, PGND2 = (pw["5V"], BY+4), (pw["GND"], BY+4), (pw["GND "], BY+4)
wire([P5V, (P5V[0], 652), (112, 652), (112, RAIL_P), (165, RAIL_P)], V5, 3)
wire([PGND, (PGND[0], 666), (98, 666), (98, RAIL_N), (165, RAIL_N)], GND, 3)
dot(165, RAIL_P); dot(165, RAIL_N)

# -- Pulsadores: señal a los pines 6 (verde) y 3 (rojo); el otro borne a GND (INPUT_PULLUP)
wire([BT["rojo_gnd"], (310, RAIL_N)], GND, 3); dot(310, RAIL_N)
wire([BT["verde_gnd"], (472, RAIL_N)], GND, 3); dot(472, RAIL_N)
wire([BT["verde_sig"], (432, 200), (dig[6], 200), (dig[6], HY)], S_VERDE, 3)
wire([BT["rojo_sig"], (350, 208), (dig[3], 208), (dig[3], HY)], S_ROJO, 3)

# -- Alimentación de relés y sensor desde los rieles
wire([(840, RAIL_P), (876, RAIL_P), (876, r2["VCC"][1]), r2["VCC"]], V5, 3)
wire([(876, r1["VCC"][1]), r1["VCC"]], V5, 3); dot(876, r1["VCC"][1])
wire([(840, RAIL_N), (890, RAIL_N), (890, r2["GND"][1]), r2["GND"]], GND, 3)
wire([(890, r1["GND"][1]), r1["GND"]], GND, 3); dot(890, r1["GND"][1])
dot(840, RAIL_P); dot(840, RAIL_N)
wire([(876, RAIL_P), (876, S["VCC"][1]), S["VCC"]], V5, 3); dot(876, RAIL_P)
wire([(890, RAIL_N), (980, RAIL_N), (980, S["GND"][1]), S["GND"]], GND, 3); dot(890, RAIL_N)

# -- Señales a los relés (pin 4 -> relé 1, pin 5 -> relé 2)
wire([(dig[5], HY), (dig[5], 306), (848, 306), (848, r2["IN"][1]), r2["IN"]], S_R2, 3)
wire([(dig[4], HY), (dig[4], 318), (836, 318), (836, r1["IN"][1]), r1["IN"]], S_R1, 3)

# -- Salida del sensor a A0 (por arriba de la protoboard y bajando por la izquierda)
wire([S["OUT"], (960, S["OUT"][1]), (960, 206), (604, 206), (604, 700), (an["A0"], 700), (an["A0"], BY+4)], S_A0, 3)

# -- POTENCIA 12 V: puente H con dos relés y sensor en serie con V−
NC_X, NO_X = 1158, 1188
# V− -> sensor -> NC1 y NC2
wire([P_VN, (1215, P_VN[1]), (1215, S["IP+"][1]), S["IP+"]], N12, 6)
wire([S["IP-"], (NC_X, S["IP-"][1]), (NC_X, r2["NC"][1]), r2["NC"]], N12, 6)
wire([(NC_X, r1["NC"][1]), r1["NC"]], N12, 6)
# V+ -> NO1 y NO2
wire([P_VP, (1240, P_VP[1]), (1240, 300), (NO_X, 300), (NO_X, r2["NO"][1]), r2["NO"]], R12, 6)
wire([(NO_X, r1["NO"][1]), r1["NO"]], R12, 6)
dot(NC_X, r1["NC"][1]); dot(NO_X, r1["NO"][1])
# COM -> actuador
wire([r1["COM"], (1214, r1["COM"][1]), (1214, ACT_B[1]), (ACT_B[0]-8, ACT_B[1])], M1C, 6)
wire([r2["COM"], (1232, r2["COM"][1]), (1232, ACT_A[1]), (ACT_A[0]-8, ACT_A[1])], M2C, 6)

# etiquetas de pines en el Arduino
tag(dig[6]-8, 192, "pin 6", "#052e16", S_VERDE, anchor="end")
tag(dig[3]+8, 200, "pin 3", "#431407", S_ROJO)
tag(dig[5]+58, 300, "pin 5", "#082f49", S_R2)
tag(dig[4]+120, 330, "pin 4", "#3b0764", S_R1)
tag(an["A0"]+20, 692, "A0", "#422006", S_A0)
tag(P5V[0]-30, 645, "5V", "#450a0a", V5, anchor="end")
tag(1226, 186, "V−", "#f8fafc", "#3f3f46")
text(SX+50, SY+128, "en serie con la línea V−", size=11, color="#c4b5fd", anchor="middle", weight="600")
tag(1248, 284, "V+", "#fff", R12)

# ---- leyenda de cables (abajo, centro)
LX, LY = 630, 764
add(f'<rect x="{LX}" y="{LY}" width="580" height="124" rx="8" fill="#1f1f1f" stroke="#3f3f46"/>')
text(LX+14, LY+22, "CABLES", size=11, color="#a1a1aa", weight="700", ls=1)
items = [(R12, 6, "+12 V (potencia)"), (N12, 6, "−12 V (pasa por el sensor)"), (M1C, 6, "Cables del actuador"),
         (V5, 3, "+5 V (control)"), ("#000", 3, "GND (control)"), (S_A0, 3, "Señales al Arduino")]
for i, (col, w, lbl) in enumerate(items):
    cx = LX + 16 + (i % 2) * 280
    cy = LY + 42 + (i // 2) * 22
    add(f'<line x1="{cx}" y1="{cy}" x2="{cx+38}" y2="{cy}" stroke="{col}" stroke-width="{w}" stroke-linecap="round"/>')
    if col in ("#000", N12):
        add(f'<line x1="{cx}" y1="{cy}" x2="{cx+38}" y2="{cy}" stroke="#52525b" stroke-width="{w+2}" stroke-linecap="round" opacity=".5"/>')
        add(f'<line x1="{cx}" y1="{cy}" x2="{cx+38}" y2="{cy}" stroke="{col}" stroke-width="{w}" stroke-linecap="round"/>')
    text(cx+50, cy+4, lbl, size=12, color="#e4e4e7")
add(f'<circle cx="{LX+22}" cy="{LY+112}" r="5" fill="#f8fafc" stroke="#111"/>')
text(LX+34, LY+116, "punto = conexión  ·  cables que se cruzan sin punto NO están conectados", size=11, color="#a1a1aa")

# ================================================================ NUMERACIÓN
callout(1, 92, 460, AX+6, AY+150)
callout(2, 520, 70, 476, 88)
callout(3, 182, 104, 294, 104)
callout(4, 930, 52, SX+58, SY+40)
callout(5, 1300, 420, 1084, 470)
callout(6, 1300, 640, 1084, 690)
callout(7, 1530, 300, 1500, PY+PHT-6)
callout(8, 1560, 690, 1500, TY+28)
callout(9, 600, 160, 640, 230)
callout(10, 68, 840, 68, 782)
for n, cx, cy, ax, ay in callouts:
    col = C[n]
    add(f'<line x1="{cx}" y1="{cy}" x2="{ax}" y2="{ay}" stroke="{col}" stroke-width="3"/>')
    add(f'<circle cx="{ax}" cy="{ay}" r="6" fill="#fff" stroke="{col}" stroke-width="3"/>')
    add(f'<circle cx="{cx}" cy="{cy}" r="27" fill="{col}" stroke="#fff" stroke-width="4"/>')
    text(cx, cy + 9, str(n), size=26, color="#fff", anchor="middle", weight="700")

add('</svg>')
SVG = "\n".join(svg)

# ================================================================ PÁGINA (título + referencia)
REF = [
 (1, "Arduino UNO", "Lee los pulsadores y el sensor de corriente; activa los relés. Se alimenta por USB (5 V).",
     "Controlador: decide cuándo bajar, cuándo parar y cuándo subir la placa."),
 (2, "Pulsador verde Ø30", "Entrada digital, pin 6 (INPUT_PULLUP): al presionar, el pin pasa a 0 V.",
     "Botón COMPACTAR en la tapa: inicia el ciclo automático."),
 (3, "Pulsador rojo Ø30", "Entrada digital, pin 3 (INPUT_PULLUP). Se revisa en cada vuelta del programa.",
     "PARO DE EMERGENCIA en la tapa: corta ambos relés al instante."),
 (4, "Sensor de corriente (tipo ACS712)", "En serie con la línea V− de 12 V; entrega un voltaje proporcional a la corriente en A0.",
     "Mide el esfuerzo del actuador. Al llegar a 2900 mA (2,9 A) la placa retrocede de inmediato."),
 (5, "Módulo relé 1", "Interruptor de 10 A con bobina de 5 V; entrada IN en el pin 4 (se activa con LOW).",
     "AVANCE: conecta +12 V al cable 1 del actuador y la placa baja a compactar."),
 (6, "Módulo relé 2", "Interruptor de 10 A con bobina de 5 V; entrada IN en el pin 5 (se activa con LOW).",
     "RETORNO: invierte la polaridad y la placa sube a su posición inicial."),
 (7, "Fuente 12 V DC · 200 W", "Fuente de potencia separada del control; capacidad máxima 16,7 A.",
     "Alimenta solo el actuador, a través de los relés. Se conecta a 220 V."),
 (8, "Actuador lineal 12 V", "Motor DC con tornillo: 1500 N máx., carrera 300 mm, 6 mm/s, fines de carrera internos.",
     "Empuja la placa por los 2 tubos guía. Máx. ≈ 665 N (2,9 A): desde 3 A la madera se daña."),
 (9, "Protoboard", "Rieles + (5 V) y − (GND) que reparten la alimentación de control.",
     "Une la tierra de los pulsadores, los relés y el sensor con el Arduino."),
 (10, "Cargador USB 5 V", "Alimentación del lado de control, aislada de los 12 V por los relés.",
     "Mantiene encendido el Arduino sin computador."),
]
cards = []
for n, nombre, circ, real in REF:
    col = C[n]
    cards.append(f'''
    <div class="card" style="border-left-color:{col}">
      <div class="num" style="background:{col}">{n}</div>
      <div class="body">
        <div class="name">{html.escape(nombre)}</div>
        <div class="row"><span class="k">Circuito:</span><span>{html.escape(circ)}</span></div>
        <div class="row"><span class="k" style="color:{col}">En el prototipo:</span><span>{html.escape(real)}</span></div>
      </div>
    </div>''')

PAGE = f'''<!DOCTYPE html>
<html lang="es"><head><meta charset="utf-8">
<style>
  body {{ margin:0; background:#f6f7f9; font-family: Inter, "DejaVu Sans", sans-serif; color:#111827; }}
  .wrap {{ width:1760px; padding:34px 40px 30px; }}
  h1 {{ font-size:40px; margin:0 0 8px; letter-spacing:-0.5px; }}
  .sub {{ font-size:19px; color:#4b5563; margin:0 0 22px; }}
  .canvas {{ border:6px solid #111827; border-radius:14px; overflow:hidden; line-height:0; }}
  h2 {{ font-size:26px; margin:26px 0 14px; }}
  .grid {{ display:grid; grid-template-columns:1fr 1fr; gap:14px 22px; }}
  .card {{ display:flex; gap:18px; align-items:center; background:#fff; border:1px solid #e5e7eb; border-left:7px solid; border-radius:10px; padding:14px 18px; }}
  .num {{ flex:0 0 52px; height:52px; border-radius:50%; color:#fff; font-weight:700; font-size:24px; display:flex; align-items:center; justify-content:center; }}
  .name {{ font-weight:700; font-size:21px; margin-bottom:5px; }}
  .row {{ display:grid; grid-template-columns:150px 1fr; font-size:16px; line-height:1.4; color:#374151; }}
  .k {{ font-weight:700; color:#111827; }}
  .notes {{ margin-top:20px; font-size:15px; color:#4b5563; line-height:1.55; }}
  .notes b {{ color:#111827; }}
</style></head><body><div class="wrap">
  <h1>Compactador de latas: circuito real del prototipo (v3)</h1>
  <p class="sub">Montaje físico definitivo, con el actuador lineal. Cada número indica el componente, su función en el circuito y qué hace en el prototipo.</p>
  <div class="canvas">{SVG}</div>
  <h2>Referencia</h2>
  <div class="grid">{"".join(cards)}</div>
  <div class="notes">
    <b>Puente H con dos relés:</b> en reposo los dos COM quedan unidos a V− y el actuador no se mueve. Al activar el relé 1, su COM pasa a +12 V y la placa baja; al activar el relé 2, la corriente circula al revés y la placa sube.
    <b>Umbral de corriente:</b> cuando el sensor marca 2900 mA (2,9 A ≈ 665 N) el programa corta el avance y la placa retrocede de inmediato; nunca se llega a 3 A (≈ 700 N), donde la estructura de madera se daña. Por eso el prototipo admite como máximo 5 latas paradas.
    <b>Pines</b> iguales a la simulación en Wokwi (pulsadores 6 y 3, relés 4 y 5, sensor A0); el pin 2 no se usa porque quedó dañado en la iteración 1.
  </div>
</div></body></html>'''

open("circuito_real_v3.html", "w", encoding="utf-8").write(PAGE)
print("ok", len(PAGE))
