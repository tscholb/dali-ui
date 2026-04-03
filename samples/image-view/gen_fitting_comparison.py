from PIL import Image, ImageDraw, ImageFont

def make_src(w, h):
    img = Image.new("RGB", (w, h))
    pix = img.load()
    for y in range(h):
        for x in range(w):
            pix[x, y] = (int(30 + 40*x/w), int(80 + 120*y/h), int(180 - 100*x/w))
    d = ImageDraw.Draw(img)
    for x in range(0, w, 30):
        d.line([(x, 0), (x, h)], fill=(200, 200, 200), width=1)
    for y in range(0, h, 30):
        d.line([(0, y), (w, y)], fill=(200, 200, 200), width=1)
    s = 18
    d.rectangle([0, 0, s, s],             fill="#FF4444")
    d.rectangle([w-s-1, 0, w-1, s],       fill="#FFCC00")
    d.rectangle([0, h-s-1, s, h-1],       fill="#44FF44")
    d.rectangle([w-s-1, h-s-1, w-1, h-1], fill="#44AAFF")
    cx, cy, r = w//2, h//2, 28
    d.ellipse([cx-r, cy-r, cx+r, cy+r], outline="#FFFFFF", width=3)
    d.ellipse([cx-4, cy-4, cx+4, cy+4],  fill="#FFFFFF")
    d.rectangle([0, 0, w-1, h-1], outline="#FFFFFF", width=2)
    return img

src = make_src(120, 360)   # portrait 1:3 — dramatic difference in every mode
SW, SH = src.size
CELL_BG = (38, 50, 56)

def fit_keep(s, cw, ch):
    sc = min(cw/s.width, ch/s.height)
    nw, nh = int(s.width*sc), int(s.height*sc)
    r = s.resize((nw, nh), Image.LANCZOS)
    o = Image.new("RGB", (cw, ch), CELL_BG)
    o.paste(r, ((cw-nw)//2, (ch-nh)//2))
    return o

def fill_mode(s, cw, ch):
    return s.resize((cw, ch), Image.LANCZOS)

def over_fit(s, cw, ch):
    sc = max(cw/s.width, ch/s.height)
    nw, nh = int(s.width*sc), int(s.height*sc)
    r = s.resize((nw, nh), Image.LANCZOS)
    return r.crop(((nw-cw)//2, (nh-ch)//2, (nw-cw)//2+cw, (nh-ch)//2+ch))

def center_mode(s, cw, ch):
    o = Image.new("RGB", (cw, ch), CELL_BG)
    px_off = (cw - s.width) // 2
    py_off = (ch - s.height) // 2
    x1 = max(0, -px_off); y1 = max(0, -py_off)
    x2 = min(s.width, x1+cw); y2 = min(s.height, y1+ch)
    o.paste(s.crop((x1, y1, x2, y2)), (max(0, px_off), max(0, py_off)))
    return o

CW, CH   = 1100, 720
TITLE_H  = 60
SRC_W    = 200
GAP      = 10
LH       = 46
M        = 14

gx0  = SRC_W + GAP*2
celw = (CW - gx0 - M - GAP) // 2
celh = (CH - TITLE_H - M - GAP) // 2
iah  = celh - LH

MODES = [
    ("FIT_KEEP_ASPECT_RATIO",     fit_keep,    "#1565C0"),
    ("FILL",                       fill_mode,   "#AD1457"),
    ("OVER_FIT_KEEP_ASPECT_RATIO", over_fit,    "#00695C"),
    ("CENTER  (original size)",    center_mode, "#6A1B9A"),
]

canvas = Image.new("RGB", (CW, CH), (18, 18, 18))
d = ImageDraw.Draw(canvas)

try:
    ft = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 26)
    fl = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 15)
    fs = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 12)
except Exception:
    ft = fl = fs = ImageFont.load_default()

# Title
d.rectangle([0, 0, CW, TITLE_H], fill="#263238")
d.text((CW//2, TITLE_H//2), "ImageView  FittingMode  Comparison",
       font=ft, fill="#ECEFF1", anchor="mm")

# Source panel
px, py = M, TITLE_H + M
pw = SRC_W - M
d.rectangle([px, py, px+pw, CH-M], fill="#1C2526", outline="#455A64", width=1)
d.text((px+pw//2, py+18), "Source Image", font=fl, fill="#90A4AE", anchor="mm")

disp = src.copy()
disp.thumbnail((pw-20, pw-20), Image.LANCZOS)
sx = px + (pw - disp.width)//2
sy = py + 36
canvas.paste(disp, (sx, sy))
d.rectangle([sx-1, sy-1, sx+disp.width, sy+disp.height], outline="#FFFFFF", width=1)

ty = sy + disp.height + 14
for txt, col in [
    (f"{SW} x {SH} px", "#78909C"),
    ("(portrait 1:3)", "#546E7A"),
    ("", ""),
    ("Cell area:", "#90A4AE"),
    (f"{celw} x {iah} px", "#78909C"),
    ("(roughly square)", "#546E7A"),
]:
    if col:
        d.text((px+pw//2, ty), txt, font=fs, fill=col, anchor="mm")
    ty += 18

# Grid cells
for idx, (name, fn, hexcol) in enumerate(MODES):
    row, col = idx // 2, idx % 2
    cx = gx0 + col * (celw + GAP)
    cy = TITLE_H + M//2 + row * (celh + GAP)
    rc = tuple(int(hexcol[i:i+2], 16) for i in (1, 3, 5))

    d.rectangle([cx, cy, cx+celw, cy+celh], fill="#1A1A1A", outline="#37474F", width=1)
    d.rectangle([cx, cy, cx+celw, cy+LH],   fill=rc)
    d.text((cx+celw//2, cy+LH//2), name, font=fl, fill="#FFFFFF", anchor="mm")

    iy = cy + LH
    d.rectangle([cx, iy, cx+celw, cy+celh], fill=CELL_BG)
    canvas.paste(fn(src, celw, iah), (cx, iy))
    d.rectangle([cx, iy, cx+celw-1, cy+celh-1], outline="#546E7A", width=1)

out = "res/fitting-mode-comparison.png"
canvas.save(out, "PNG")
print("Saved:", out, f"({CW}x{CH})")
