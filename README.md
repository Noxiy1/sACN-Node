
<!-- goto url to see the website ttps://noxiy1.github.io/sACN-Node/main.html -->







<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>sACN Node — WT32-ETH01 DMX Konverter</title>
<link rel="preconnect" href="https://fonts.googleapis.com/">
<link href="./main_files/css2" rel="stylesheet">
<style>
:root {
  --bg:        #0b0e13;
  --bg2:       #111620;
  --bg3:       #181e2a;
  --border:    #1e2840;
  --border2:   #2a3550;
  --accent:    #00d4ff;
  --accent2:   #0090bb;
  --green:     #00e5a0;
  --amber:     #f5a623;
  --red:       #ff4d6d;
  --purple:    #9b6dff;
  --text:      #cdd6f4;
  --text2:     #7a8aaa;
  --text3:     #3d4f70;
  --mono:      'IBM Plex Mono', monospace;
  --sans:      'IBM Plex Sans', sans-serif;
}

*, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

html { scroll-behavior: smooth; }

body {
  background: var(--bg);
  color: var(--text);
  font-family: var(--mono);
  font-size: 13px;
  line-height: 1.7;
  min-height: 100vh;
  overflow-x: hidden;
}

/* ── SCAN LINES ── */
body::before {
  content: '';
  position: fixed;
  inset: 0;
  background: repeating-linear-gradient(0deg, transparent, transparent 2px, rgba(0,0,0,.18) 2px, rgba(0,0,0,.18) 4px);
  pointer-events: none;
  z-index: 9999;
  opacity: .35;
}

/* ── HEADER ── */
header {
  border-bottom: 1px solid var(--border);
  padding: 0 2rem;
  position: sticky;
  top: 0;
  z-index: 100;
  background: rgba(11,14,19,.92);
  backdrop-filter: blur(12px);
  display: flex;
  align-items: center;
  gap: 2rem;
  height: 52px;
}
.logo {
  font-size: 11px;
  font-weight: 600;
  letter-spacing: .12em;
  color: var(--accent);
  text-transform: uppercase;
  white-space: nowrap;
}
.logo span { color: var(--text2); font-weight: 300; }
nav { display: flex; gap: 0; margin-left: auto; }
nav a {
  color: var(--text2);
  text-decoration: none;
  font-size: 11px;
  letter-spacing: .08em;
  padding: 0 1rem;
  height: 52px;
  display: flex;
  align-items: center;
  border-left: 1px solid var(--border);
  transition: color .15s, background .15s;
}
nav a:hover { color: var(--accent); background: rgba(0,212,255,.04); }

/* ── HERO ── */
.hero {
  padding: 5rem 2rem 4rem;
  max-width: 960px;
  margin: 0 auto;
  position: relative;
}
.hero-tag {
  font-size: 10px;
  letter-spacing: .2em;
  color: var(--accent);
  text-transform: uppercase;
  margin-bottom: 1.2rem;
  display: flex;
  align-items: center;
  gap: .6rem;
}
.hero-tag::before {
  content: '';
  display: inline-block;
  width: 24px;
  height: 1px;
  background: var(--accent);
}
h1 {
  font-family: var(--mono);
  font-size: clamp(1.8rem, 4vw, 3.2rem);
  font-weight: 300;
  color: #fff;
  line-height: 1.15;
  letter-spacing: -.01em;
  margin-bottom: 1.2rem;
}
h1 strong { font-weight: 600; color: var(--accent); }
.hero-sub {
  font-size: 13px;
  color: var(--text2);
  max-width: 560px;
  line-height: 1.8;
  margin-bottom: 2.5rem;
}
.hero-chips { display: flex; gap: .6rem; flex-wrap: wrap; }
.chip {
  font-size: 10px;
  letter-spacing: .1em;
  text-transform: uppercase;
  padding: .3rem .8rem;
  border: 1px solid var(--border2);
  border-radius: 2px;
  color: var(--text2);
  background: var(--bg2);
}
.chip.hi { border-color: var(--accent); color: var(--accent); background: rgba(0,212,255,.06); }

/* ── SECTION ── */
section {
  max-width: 960px;
  margin: 0 auto;
  padding: 0 2rem 4rem;
}
.sec-head {
  display: flex;
  align-items: baseline;
  gap: 1rem;
  margin-bottom: 2rem;
  padding-bottom: .8rem;
  border-bottom: 1px solid var(--border);
}
.sec-num {
  font-size: 10px;
  color: var(--text3);
  letter-spacing: .15em;
  min-width: 28px;
}
h2 {
  font-family: var(--mono);
  font-size: 11px;
  font-weight: 500;
  letter-spacing: .18em;
  text-transform: uppercase;
  color: var(--text);
}
.sec-line { flex: 1; height: 1px; background: var(--border); }

/* ── BLOCK DIAGRAM SVG WRAPPER ── */
.diagram-wrap {
  background: var(--bg2);
  border: 1px solid var(--border);
  border-radius: 3px;
  padding: 2rem;
  margin-bottom: 2rem;
  overflow-x: auto;
}

/* ── GRID ── */
.grid2 { display: grid; grid-template-columns: 1fr 1fr; gap: 1px; background: var(--border); border: 1px solid var(--border); border-radius: 3px; overflow: hidden; margin-bottom: 1.5rem; }
.grid3 { display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 1px; background: var(--border); border: 1px solid var(--border); border-radius: 3px; overflow: hidden; margin-bottom: 1.5rem; }
.grid4 { display: grid; grid-template-columns: repeat(4,1fr); gap: 1px; background: var(--border); border: 1px solid var(--border); border-radius: 3px; overflow: hidden; margin-bottom: 1.5rem; }
@media(max-width:640px) { .grid2,.grid3,.grid4 { grid-template-columns: 1fr; } }
.card {
  background: var(--bg2);
  padding: 1.4rem;
}
.card-label {
  font-size: 9px;
  letter-spacing: .18em;
  text-transform: uppercase;
  color: var(--text3);
  margin-bottom: .6rem;
}
.card-title {
  font-size: 13px;
  font-weight: 500;
  color: var(--text);
  margin-bottom: .4rem;
}
.card-val {
  font-size: 11px;
  color: var(--text2);
  line-height: 1.6;
}
.card-val .hl { color: var(--accent); }
.card-val .hl2 { color: var(--green); }
.card-val .hl3 { color: var(--amber); }

/* ── TABLE ── */
.tbl-wrap { border: 1px solid var(--border); border-radius: 3px; overflow: hidden; margin-bottom: 1.5rem; }
table { width: 100%; border-collapse: collapse; }
thead tr { background: var(--bg3); }
th {
  font-size: 9px;
  letter-spacing: .16em;
  text-transform: uppercase;
  color: var(--text3);
  padding: .75rem 1rem;
  text-align: left;
  font-weight: 500;
  border-bottom: 1px solid var(--border);
}
td {
  padding: .65rem 1rem;
  font-size: 12px;
  border-bottom: 1px solid var(--border);
  color: var(--text2);
  vertical-align: top;
}
tr:last-child td { border-bottom: none; }
tbody tr:hover { background: rgba(255,255,255,.02); }
.td-mono { font-family: var(--mono); color: var(--accent); font-size: 11.5px; }
.td-green { color: var(--green); }
.td-amber { color: var(--amber); }
.td-red   { color: var(--red); }
.td-dim   { color: var(--text3); }

/* ── CODE BLOCK ── */
.code-block {
  background: var(--bg2);
  border: 1px solid var(--border);
  border-left: 3px solid var(--accent);
  border-radius: 3px;
  padding: 1.4rem 1.6rem;
  margin-bottom: 1.5rem;
  overflow-x: auto;
  white-space: pre;
  font-size: 11.5px;
  line-height: 1.75;
  color: var(--text);
}
.code-block .c  { color: var(--text3); }
.code-block .kw { color: var(--purple); }
.code-block .st { color: var(--green); }
.code-block .nm { color: var(--amber); }
.code-block .cm { color: var(--text3); font-style: italic; }

/* ── INFO / WARNING ── */
.info {
  border: 1px solid var(--border2);
  border-left: 3px solid var(--accent);
  border-radius: 3px;
  padding: 1rem 1.2rem;
  margin-bottom: 1.2rem;
  font-size: 12px;
  color: var(--text2);
  background: rgba(0,212,255,.04);
}
.warn {
  border: 1px solid #3a2000;
  border-left: 3px solid var(--amber);
  border-radius: 3px;
  padding: 1rem 1.2rem;
  margin-bottom: 1.2rem;
  font-size: 12px;
  color: var(--text2);
  background: rgba(245,166,35,.04);
}
.warn strong, .info strong { color: var(--text); }
.info ul, .warn ul { margin: .5rem 0 0 1rem; }
.info li, .warn li { margin-bottom: .2rem; }

/* ── DIVIDER ── */
hr { border: none; border-top: 1px solid var(--border); margin: 3rem 0; }

/* ── SVG DIAGRAM STYLES ── */
.svg-diagram { width: 100%; height: auto; display: block; }
text { font-family: 'IBM Plex Mono', monospace; }

/* ── STATUS DOT ── */
.dot { display: inline-block; width: 7px; height: 7px; border-radius: 50%; margin-right: .4rem; vertical-align: middle; }
.dot-green { background: var(--green); box-shadow: 0 0 6px var(--green); }
.dot-amber { background: var(--amber); box-shadow: 0 0 6px var(--amber); }

/* ── FOOTER ── */
footer {
  border-top: 1px solid var(--border);
  padding: 1.5rem 2rem;
  display: flex;
  justify-content: space-between;
  align-items: center;
  font-size: 10px;
  color: var(--text3);
  letter-spacing: .08em;
  max-width: 100%;
}
</style>



<header>
  <div class="logo">sACN<span>/</span>NODE &nbsp;·&nbsp; WT32-ETH01</div>
  <nav>
    <a href="https://noxiy1.github.io/sACN-Node/main.html#uebersicht">Übersicht</a>
    <a href="https://noxiy1.github.io/sACN-Node/main.html#pins">Pins</a>
    <a href="https://noxiy1.github.io/sACN-Node/main.html#max485">MAX485</a>
    <a href="https://noxiy1.github.io/sACN-Node/main.html#oled">OLED</a>
    <a href="https://noxiy1.github.io/sACN-Node/main.html#sacn">sACN</a>
    <a href="https://noxiy1.github.io/sACN-Node/main.html#strom">Strom</a>
    <a href="https://noxiy1.github.io/sACN-Node/main.html#xlr">XLR</a>
  </nav>
</header>

<!-- ══ HERO ══════════════════════════════════════════════════════ -->
<div class="hero">
  <div class="hero-tag">Technische Dokumentation</div>
  <h1>sACN → <strong>DMX512</strong><br>Konverter-Node</h1>
  <p class="hero-sub">
    Ethernet-basierter sACN E1.31 Empfänger mit 3 unabhängigen DMX-Universen über Hardware-UARTs.
    Basierend auf dem WT32-ETH01 (ESP32 + LAN8720A PHY).
  </p>
  <div class="hero-chips">
    <span class="chip hi">WT32-ETH01</span>
    <span class="chip hi">3× MAX485</span>
    <span class="chip hi">SSD1306 OLED</span>
    <span class="chip">sACN E1.31</span>
    <span class="chip">3× UART HW</span>
    <span class="chip">250 kBaud DMX512</span>
    <span class="chip">UDP Multicast</span>
  </div>
</div>

<!-- ══ 01 ÜBERSICHT ══════════════════════════════════════════════ -->
<section id="uebersicht">
  <div class="sec-head">
    <span class="sec-num">01</span>
    <h2>Systemübersicht</h2>
    <div class="sec-line"></div>
  </div>

  <!-- BLOCK DIAGRAM -->
  <div class="diagram-wrap">
    <svg class="svg-diagram" viewBox="0 0 880 220" xmlns="http://www.w3.org/2000/svg">
      <defs>
        <marker id="arr" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="6" markerHeight="6" orient="auto">
          <path d="M1 2L9 5L1 8" fill="none" stroke="#00d4ff" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"></path>
        </marker>
        <marker id="arrg" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="6" markerHeight="6" orient="auto">
          <path d="M1 2L9 5L1 8" fill="none" stroke="#00e5a0" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"></path>
        </marker>
      </defs>

      <!-- Netzwerk -->
      <rect x="10" y="70" width="110" height="60" rx="3" fill="#111620" stroke="#1e2840" stroke-width="1"></rect>
      <text x="65" y="96" text-anchor="middle" font-size="10" fill="#7a8aaa" letter-spacing="1">NETZWERK</text>
      <text x="65" y="113" text-anchor="middle" font-size="9" fill="#3d4f70">sACN E1.31</text>
      <text x="65" y="125" text-anchor="middle" font-size="9" fill="#3d4f70">UDP Multicast</text>

      <!-- Pfeil ETH -->
      <line x1="120" y1="100" x2="170" y2="100" stroke="#00d4ff" stroke-width="1.5" marker-end="url(#arr)"></line>
      <text x="145" y="93" text-anchor="middle" font-size="8" fill="#00d4ff" letter-spacing="1">RJ45</text>

      <!-- WT32-ETH01 -->
      <rect x="170" y="30" width="180" height="140" rx="3" fill="#111620" stroke="#00d4ff" stroke-width="1.5"></rect>
      <text x="260" y="54" text-anchor="middle" font-size="9" fill="#00d4ff" letter-spacing="2">WT32-ETH01</text>
      <line x1="185" y1="60" x2="335" y2="60" stroke="#1e2840" stroke-width="1"></line>
      <text x="260" y="78" text-anchor="middle" font-size="8" fill="#7a8aaa">LAN8720A PHY</text>
      <text x="260" y="93" text-anchor="middle" font-size="8" fill="#7a8aaa">ESP32 · 240 MHz</text>
      <text x="260" y="108" text-anchor="middle" font-size="8" fill="#7a8aaa">3× HW-UART · I²C</text>
      <text x="260" y="128" text-anchor="middle" font-size="8" fill="#3d4f70">UART2 · UART1 · UART0*</text>
      <text x="260" y="143" text-anchor="middle" font-size="8" fill="#3d4f70">GPIO14 · GPIO12 · GPIO33</text>
      <text x="260" y="158" text-anchor="middle" font-size="8" fill="#3d4f70">OLED: SDA4 · SCL2</text>

      <!-- Pfeil OLED -->
      <line x1="200" y1="170" x2="200" y2="195" stroke="#9b6dff" stroke-width="1.2" stroke-dasharray="3 2"></line>
      <text x="210" y="185" font-size="8" fill="#9b6dff" letter-spacing="1">SSD1306 128×32</text>

      <!-- Pfeile zu MAX485 -->
      <line x1="350" y1="72" x2="430" y2="55" stroke="#00e5a0" stroke-width="1.2" marker-end="url(#arrg)"></line>
      <line x1="350" y1="100" x2="430" y2="100" stroke="#00e5a0" stroke-width="1.2" marker-end="url(#arrg)"></line>
      <line x1="350" y1="128" x2="430" y2="145" stroke="#00e5a0" stroke-width="1.2" marker-end="url(#arrg)"></line>
      <text x="388" y="68" font-size="8" fill="#00e5a0">UART2</text>
      <text x="388" y="95" font-size="8" fill="#00e5a0">UART1</text>
      <text x="388" y="138" font-size="8" fill="#00e5a0">UART0*</text>

      <!-- MAX485 boxes -->
      <rect x="430" y="30" width="100" height="44" rx="2" fill="#111620" stroke="#00e5a0" stroke-width="1"></rect>
      <text x="480" y="50" text-anchor="middle" font-size="9" fill="#00e5a0" letter-spacing="1">MAX485 #1</text>
      <text x="480" y="64" text-anchor="middle" font-size="8" fill="#3d4f70">Universum 1</text>

      <rect x="430" y="78" width="100" height="44" rx="2" fill="#111620" stroke="#00e5a0" stroke-width="1"></rect>
      <text x="480" y="98" text-anchor="middle" font-size="9" fill="#00e5a0" letter-spacing="1">MAX485 #2</text>
      <text x="480" y="112" text-anchor="middle" font-size="8" fill="#3d4f70">Universum 2</text>

      <rect x="430" y="126" width="100" height="44" rx="2" fill="#111620" stroke="#00e5a0" stroke-width="1"></rect>
      <text x="480" y="146" text-anchor="middle" font-size="9" fill="#00e5a0" letter-spacing="1">MAX485 #3</text>
      <text x="480" y="160" text-anchor="middle" font-size="8" fill="#3d4f70">Universum 3</text>

      <!-- Pfeile zu XLR -->
      <line x1="530" y1="52" x2="590" y2="52" stroke="#f5a623" stroke-width="1.2" marker-end="url(#arr)"></line>
      <line x1="530" y1="100" x2="590" y2="100" stroke="#f5a623" stroke-width="1.2" marker-end="url(#arr)"></line>
      <line x1="530" y1="148" x2="590" y2="148" stroke="#f5a623" stroke-width="1.2" marker-end="url(#arr)"></line>
      <text x="558" y="48" text-anchor="middle" font-size="7" fill="#f5a623">RS485</text>
      <text x="558" y="96" text-anchor="middle" font-size="7" fill="#f5a623">RS485</text>
      <text x="558" y="144" text-anchor="middle" font-size="7" fill="#f5a623">RS485</text>

      <!-- XLR -->
      <circle cx="613" cy="52" r="18" fill="#181e2a" stroke="#f5a623" stroke-width="1"></circle>
      <text x="613" y="49" text-anchor="middle" font-size="8" fill="#f5a623">XLR</text>
      <text x="613" y="60" text-anchor="middle" font-size="7" fill="#7a8aaa">DMX 1</text>

      <circle cx="613" cy="100" r="18" fill="#181e2a" stroke="#f5a623" stroke-width="1"></circle>
      <text x="613" y="97" text-anchor="middle" font-size="8" fill="#f5a623">XLR</text>
      <text x="613" y="108" text-anchor="middle" font-size="7" fill="#7a8aaa">DMX 2</text>

      <circle cx="613" cy="148" r="18" fill="#181e2a" stroke="#f5a623" stroke-width="1"></circle>
      <text x="613" y="145" text-anchor="middle" font-size="8" fill="#f5a623">XLR</text>
      <text x="613" y="156" text-anchor="middle" font-size="7" fill="#7a8aaa">DMX 3</text>

      <!-- Geräte -->
      <line x1="631" y1="52" x2="700" y2="52" stroke="#3d4f70" stroke-width="1" stroke-dasharray="3 2"></line>
      <line x1="631" y1="100" x2="700" y2="100" stroke="#3d4f70" stroke-width="1" stroke-dasharray="3 2"></line>
      <line x1="631" y1="148" x2="700" y2="148" stroke="#3d4f70" stroke-width="1" stroke-dasharray="3 2"></line>
      <rect x="700" y="37" width="90" height="30" rx="2" fill="#111620" stroke="#1e2840"></rect>
      <text x="745" y="56" text-anchor="middle" font-size="8" fill="#7a8aaa">DMX Geräte</text>
      <rect x="700" y="85" width="90" height="30" rx="2" fill="#111620" stroke="#1e2840"></rect>
      <text x="745" y="104" text-anchor="middle" font-size="8" fill="#7a8aaa">DMX Geräte</text>
      <rect x="700" y="133" width="90" height="30" rx="2" fill="#111620" stroke="#1e2840"></rect>
      <text x="745" y="152" text-anchor="middle" font-size="8" fill="#7a8aaa">DMX Geräte</text>

      <!-- UART0 Hinweis -->
      <text x="10" y="210" font-size="10" fill="#3d4f70">
    *UART0 auf GPIO33 remappt — Debug-Serial (GPIO1/3) bleibt frei</text>
    </svg>
  </div>

  <div class="grid4">
    <div class="card">
      <div class="card-label">Protokoll</div>
      <div class="card-title">sACN E1.31</div>
      <div class="card-val">UDP Multicast<br>Port <span class="hl">5568</span><br>239.255.x.x</div>
    </div>
    <div class="card">
      <div class="card-label">Universen</div>
      <div class="card-title">3 × 512 Ch</div>
      <div class="card-val">UART1 / UART2 / UART3<br>je <span class="hl">512 Kanäle</span><br>DMX512-A</div>
    </div>
    <div class="card">
      <div class="card-label">DMX Rate</div>
      <div class="card-title">~44 Hz</div>
      <div class="card-val">250 kBaud<br>Break <span class="hl">176 µs</span><br>MAB 12 µs</div>
    </div>
    <div class="card">
      <div class="card-label">Timeout</div>
      <div class="card-title">3 Sekunden</div>
      <div class="card-val">Kein sACN →<br>Blackout<br><span class="hl2">automatisch</span></div>
    </div>
  </div>
</section>

<!-- ══ 02 PIN-BELEGUNG ════════════════════════════════════════════ -->
<section id="pins">
  <div class="sec-head">
    <span class="sec-num">02</span>
    <h2>PIN-Belegung WT32-ETH01</h2>
    <div class="sec-line"></div>
  </div>

  <!-- Visual PIN diagram -->
  <div class="diagram-wrap">
    <svg class="svg-diagram" viewBox="0 0 820 320" xmlns="http://www.w3.org/2000/svg">
      <!-- Board outline -->
      <rect x="260" y="20" width="220" height="280" rx="6" fill="#0d1117" stroke="#1e2840" stroke-width="1.5"></rect>
      <!-- Board label -->
      <text x="370" y="48" text-anchor="middle" font-size="10" fill="#00d4ff" letter-spacing="2">WT32-ETH01</text>
      <line x1="270" y1="54" x2="470" y2="54" stroke="#1e2840" stroke-width="1"></line>
      <text x="370" y="68" text-anchor="middle" font-size="8" fill="#3d4f70">ESP32 + LAN8720A</text>

      <!-- ETH port right side -->
      <rect x="466" y="95" width="30" height="22" rx="2" fill="#1e2840" stroke="#3d4f70"></rect>
      <text x="481" y="110" text-anchor="middle" font-size="7" fill="#7a8aaa">ETH</text>

      <!-- Left side pins (dots) -->
      <!-- 3V3 -->
      <circle cx="264" cy="90" r="4" fill="#3d4f70"></circle>
      <!-- GND -->
      <circle cx="264" cy="108" r="4" fill="#3d4f70"></circle>
      <!-- SDA GPIO4 -->
      <circle cx="264" cy="126" r="5" fill="#9b6dff"></circle>
      <!-- SCL GPIO2 -->
      <circle cx="264" cy="144" r="5" fill="#9b6dff"></circle>
      <!-- GPIO14 TX1 -->
      <circle cx="264" cy="162" r="5" fill="#00e5a0"></circle>
      <!-- GPIO15 DE1 -->
      <circle cx="264" cy="180" r="5" fill="#00e5a0"></circle>
      <!-- GPIO12 TX2 -->
      <circle cx="264" cy="198" r="5" fill="#00d4ff"></circle>
      <!-- GPIO13 DE2 -->
      <circle cx="264" cy="216" r="5" fill="#00d4ff"></circle>
      <!-- GPIO33 TX3 -->
      <circle cx="264" cy="234" r="5" fill="#f5a623"></circle>
      <!-- GPIO17 DE3 -->
      <circle cx="264" cy="252" r="5" fill="#f5a623"></circle>
      <!-- GND -->
      <circle cx="264" cy="270" r="4" fill="#3d4f70"></circle>
      <!-- 5V -->
      <circle cx="264" cy="288" r="4" fill="#3d4f70"></circle>

      <!-- Pin labels inside board -->
      <text x="282" y="94" font-size="8" fill="#3d4f70">3V3</text>
      <text x="282" y="112" font-size="8" fill="#3d4f70">GND</text>
      <text x="282" y="130" font-size="8" fill="#9b6dff">GPIO 4 · SDA</text>
      <text x="282" y="148" font-size="8" fill="#9b6dff">GPIO 2 · SCL</text>
      <text x="282" y="166" font-size="8" fill="#00e5a0">GPIO 14 · TX₁</text>
      <text x="282" y="184" font-size="8" fill="#00e5a0">GPIO 15 · DE/RE₁</text>
      <text x="282" y="202" font-size="8" fill="#00d4ff">GPIO 12 · TX₂</text>
      <text x="282" y="220" font-size="8" fill="#00d4ff">GPIO 13 · DE/RE₂</text>
      <text x="282" y="238" font-size="8" fill="#f5a623">GPIO 33 · TX₃*</text>
      <text x="282" y="256" font-size="8" fill="#f5a623">GPIO 17 · DE/RE₃</text>
      <text x="282" y="274" font-size="8" fill="#3d4f70">GND</text>
      <text x="282" y="292" font-size="8" fill="#3d4f70">5V IN</text>

      <!-- ── LEFT SIDE: OLED ── -->
      <rect x="60" y="108" width="110" height="52" rx="3" fill="#0d1117" stroke="#9b6dff" stroke-width="1"></rect>
      <text x="115" y="126" text-anchor="middle" font-size="8" fill="#9b6dff" letter-spacing="1.5">SSD1306</text>
      <text x="115" y="140" text-anchor="middle" font-size="8" fill="#7a8aaa">OLED 128×32</text>
      <text x="115" y="153" text-anchor="middle" font-size="7.5" fill="#3d4f70">VCC GND SCL SDA</text>
      <!-- wires OLED -->
      <line x1="170" y1="126" x2="200" y2="126" stroke="#9b6dff" stroke-width="1" stroke-dasharray="3 2"></line>
      <line x1="200" y1="126" x2="200" y2="144" stroke="#9b6dff" stroke-width="1" stroke-dasharray="3 2"></line>
      <line x1="200" y1="144" x2="264" y2="144" stroke="#9b6dff" stroke-width="1" stroke-dasharray="3 2" marker-end="url(#arr)"></line>
      <line x1="170" y1="140" x2="194" y2="140" stroke="#9b6dff" stroke-width="1" stroke-dasharray="3 2"></line>
      <line x1="194" y1="140" x2="194" y2="126" stroke="#9b6dff" stroke-width="1" stroke-dasharray="3 2"></line>
      <line x1="194" y1="126" x2="264" y2="126" stroke="#9b6dff" stroke-width="1" stroke-dasharray="3 2" marker-end="url(#arr)"></line>
      <text x="70" y="172" font-size="7.5" fill="#9b6dff">I²C: SDA=4 SCL=2</text>
      <text x="70" y="182" font-size="7.5" fill="#9b6dff">Addr: 0x3C</text>

      <!-- ── RIGHT: MAX485 #1 ── -->
      <rect x="570" y="60" width="140" height="80" rx="3" fill="#0d1117" stroke="#00e5a0" stroke-width="1"></rect>
      <text x="640" y="80" text-anchor="middle" font-size="8" fill="#00e5a0" letter-spacing="1.5">MAX485 #1</text>
      <text x="640" y="93" text-anchor="middle" font-size="7.5" fill="#3d4f70">Universum 1</text>
      <text x="584" y="108" font-size="7.5" fill="#7a8aaa">DI (4) ← TX</text>
      <text x="584" y="119" font-size="7.5" fill="#7a8aaa">RE+DE ← DE/RE</text>
      <text x="584" y="130" font-size="7.5" fill="#7a8aaa">A(6) B(7) → XLR</text>
      <!-- wires MAX485 #1 -->
      <line x1="470" y1="162" x2="530" y2="162" stroke="#00e5a0" stroke-width="1"></line>
      <line x1="530" y1="162" x2="530" y2="108" stroke="#00e5a0" stroke-width="1"></line>
      <line x1="530" y1="108" x2="570" y2="108" stroke="#00e5a0" stroke-width="1" marker-end="url(#arrg)"></line>
      <line x1="470" y1="180" x2="540" y2="180" stroke="#00e5a0" stroke-width="1" stroke-dasharray="3 2"></line>
      <line x1="540" y1="180" x2="540" y2="119" stroke="#00e5a0" stroke-width="1" stroke-dasharray="3 2"></line>
      <line x1="540" y1="119" x2="570" y2="119" stroke="#00e5a0" stroke-width="1" stroke-dasharray="3 2" marker-end="url(#arrg)"></line>
      <!-- XLR #1 -->
      <circle cx="790" cy="100" r="22" fill="#0d1117" stroke="#f5a623" stroke-width="1.2"></circle>
      <text x="790" y="97" text-anchor="middle" font-size="8" fill="#f5a623">XLR</text>
      <text x="790" y="110" text-anchor="middle" font-size="7" fill="#7a8aaa">DMX 1</text>
      <line x1="710" y1="100" x2="768" y2="100" stroke="#f5a623" stroke-width="1" marker-end="url(#arr)"></line>

      <!-- ── RIGHT: MAX485 #2 ── -->
      <rect x="570" y="165" width="140" height="80" rx="3" fill="#0d1117" stroke="#00d4ff" stroke-width="1"></rect>
      <text x="640" y="185" text-anchor="middle" font-size="8" fill="#00d4ff" letter-spacing="1.5">MAX485 #2</text>
      <text x="640" y="198" text-anchor="middle" font-size="7.5" fill="#3d4f70">Universum 2</text>
      <text x="584" y="213" font-size="7.5" fill="#7a8aaa">DI (4) ← TX</text>
      <text x="584" y="224" font-size="7.5" fill="#7a8aaa">RE+DE ← DE/RE</text>
      <text x="584" y="235" font-size="7.5" fill="#7a8aaa">A(6) B(7) → XLR</text>
      <line x1="470" y1="198" x2="548" y2="198" stroke="#00d4ff" stroke-width="1"></line>
      <line x1="548" y1="198" x2="548" y2="213" stroke="#00d4ff" stroke-width="1"></line>
      <line x1="548" y1="213" x2="570" y2="213" stroke="#00d4ff" stroke-width="1" marker-end="url(#arr)"></line>
      <line x1="470" y1="216" x2="555" y2="216" stroke="#00d4ff" stroke-width="1" stroke-dasharray="3 2"></line>
      <line x1="555" y1="216" x2="555" y2="224" stroke="#00d4ff" stroke-width="1" stroke-dasharray="3 2"></line>
      <line x1="555" y1="224" x2="570" y2="224" stroke="#00d4ff" stroke-width="1" stroke-dasharray="3 2" marker-end="url(#arr)"></line>
      <!-- XLR #2 -->
      <circle cx="790" cy="205" r="22" fill="#0d1117" stroke="#f5a623" stroke-width="1.2"></circle>
      <text x="790" y="202" text-anchor="middle" font-size="8" fill="#f5a623">XLR</text>
      <text x="790" y="215" text-anchor="middle" font-size="7" fill="#7a8aaa">DMX 2</text>
      <line x1="710" y1="205" x2="768" y2="205" stroke="#f5a623" stroke-width="1" marker-end="url(#arr)"></line>

      <!-- ── RIGHT: MAX485 #3 ── -->
      <rect x="570" y="270" width="140" height="30" rx="3" fill="#0d1117" stroke="#f5a623" stroke-width="1"></rect>
      <text x="640" y="283" text-anchor="middle" font-size="8" fill="#f5a623" letter-spacing="1.5">MAX485 #3</text>
      <text x="640" y="294" text-anchor="middle" font-size="7.5" fill="#3d4f70">Universum 3</text>
      <line x1="470" y1="234" x2="560" y2="234" stroke="#f5a623" stroke-width="1"></line>
      <line x1="560" y1="234" x2="560" y2="278" stroke="#f5a623" stroke-width="1"></line>
      <line x1="560" y1="278" x2="570" y2="278" stroke="#f5a623" stroke-width="1" marker-end="url(#arr)"></line>
      <line x1="470" y1="252" x2="563" y2="252" stroke="#f5a623" stroke-width="1" stroke-dasharray="3 2"></line>
      <line x1="563" y1="252" x2="563" y2="287" stroke="#f5a623" stroke-width="1" stroke-dasharray="3 2"></line>
      <line x1="563" y1="287" x2="570" y2="287" stroke="#f5a623" stroke-width="1" stroke-dasharray="3 2" marker-end="url(#arr)"></line>
      <!-- XLR #3 -->
      <circle cx="790" cy="290" r="22" fill="#0d1117" stroke="#f5a623" stroke-width="1.2"></circle>
      <text x="790" y="287" text-anchor="middle" font-size="8" fill="#f5a623">XLR</text>
      <text x="790" y="300" text-anchor="middle" font-size="7" fill="#7a8aaa">DMX 3</text>
      <line x1="710" y1="285" x2="768" y2="285" stroke="#f5a623" stroke-width="1" marker-end="url(#arr)"></line>

      <!-- Legend -->
      <text x="60" y="258" font-size="7.5" fill="#9b6dff">━━ I²C (OLED)</text>
      <text x="60" y="270" font-size="7.5" fill="#00e5a0">━━ Universum 1</text>
      <text x="60" y="282" font-size="7.5" fill="#00d4ff">━━ Universum 2</text>
      <text x="60" y="294" font-size="7.5" fill="#f5a623">━━ Universum 3</text>
      <text x="60" y="306" font-size="7.5" fill="#3d4f70">╌╌ DE/RE (gebrückt)</text>

      <!-- GPIO36 dummy note -->
      
    </svg>
  </div>

  <div class="tbl-wrap">
    <table>
      <thead>
        <tr>
          <th>Funktion</th>
          <th>GPIO</th>
          <th>UART / Bus</th>
          <th>Richtung</th>
          <th>MAX485 Pin</th>
          <th>Anmerkung</th>
        </tr>
      </thead>
      <tbody>
        <tr>
          <td>OLED SDA</td>
          <td class="td-mono">4</td>
          <td class="td-dim">I²C</td>
          <td class="td-dim">Bidirekt.</td>
          <td class="td-dim">—</td>
          <td>SSD1306, Addr. 0x3C</td>
        </tr>
        <tr>
          <td>OLED SCL</td>
          <td class="td-mono">2</td>
          <td class="td-dim">I²C</td>
          <td class="td-dim">Ausgang</td>
          <td class="td-dim">—</td>
          <td>Clock</td>
        </tr>
        <tr>
          <td>DMX1 TX</td>
          <td class="td-mono">14</td>
          <td>UART2</td>
          <td class="td-green">→ Ausgang</td>
          <td class="td-mono">DI (4)</td>
          <td>Data Input MAX485</td>
        </tr>
        <tr>
          <td>DMX1 DE/RE</td>
          <td class="td-mono">15</td>
          <td class="td-dim">GPIO</td>
          <td class="td-green">→ Ausgang</td>
          <td class="td-mono">DE(3) + RE(2)</td>
          <td>Gebrückt auf einem Pin</td>
        </tr>
        <tr>
          <td>DMX2 TX</td>
          <td class="td-mono">12</td>
          <td>UART1</td>
          <td class="td-green">→ Ausgang</td>
          <td class="td-mono">DI (4)</td>
          <td>Data Input MAX485</td>
        </tr>
        <tr>
          <td>DMX2 DE/RE</td>
          <td class="td-mono">13</td>
          <td class="td-dim">GPIO</td>
          <td class="td-green">→ Ausgang</td>
          <td class="td-mono">DE(3) + RE(2)</td>
          <td>Gebrückt auf einem Pin</td>
        </tr>
        <tr>
          <td>DMX3 TX</td>
          <td class="td-mono">33</td>
          <td>UART0*</td>
          <td class="td-green">→ Ausgang</td>
          <td class="td-mono">DI (4)</td>
          <td>UART0 remappt von GPIO1</td>
        </tr>
        <tr>
          <td>DMX3 DE/RE</td>
          <td class="td-mono">17</td>
          <td class="td-dim">GPIO</td>
          <td class="td-green">→ Ausgang</td>
          <td class="td-mono">DE(3) + RE(2)</td>
          <td>Gebrückt auf einem Pin</td>
        </tr>
        <tr>
          <td>Debug TX</td>
          <td class="td-mono">1</td>
          <td>UART0 default</td>
          <td class="td-green">→ Ausgang</td>
          <td class="td-dim">—</td>
          <td>Serial Monitor, frei nutzbar</td>
        </tr>
        
      </tbody>
    </table>
  </div>

  <div class="warn">
    <strong>* UART0 Remapping:</strong> UART0 liegt standardmäßig auf GPIO1 (TX) / GPIO3 (RX) — das ist der USB-Debug-Serial. Durch <code>dmxSerial3.begin(250000, SERIAL_8N2, 36, 33)</code> mappt der ESP32-Treiber UART0 auf GPIO33 um. GPIO1/3 bleiben danach für <code>Serial.begin(115200)</code> frei.
  </div>

  <h3 style="color:var(--text2);font-size:10px;letter-spacing:.15em;text-transform:uppercase;margin-bottom:1rem;">Ethernet-Pins (fest durch Hardware, nicht nutzbar)</h3>
  <div class="tbl-wrap">
    <table>
      <thead><tr><th>Funktion</th><th>GPIO</th><th>Beschreibung</th></tr></thead>
      <tbody>
        <tr><td>ETH CLK</td><td class="td-mono">0</td><td>Ethernet-Takt (Input vom PHY)</td></tr>
        <tr><td>ETH MDC</td><td class="td-mono">23</td><td>Management Data Clock</td></tr>
        <tr><td>ETH MDIO</td><td class="td-mono">18</td><td>Management Data I/O</td></tr>
        <tr><td>ETH RX/TX</td><td class="td-mono">35–39</td><td>RMII Daten (belegt, Input-Only)</td></tr>
        <tr><td>PHY RESET</td><td class="td-mono">16</td><td class="td-red">NICHT verwenden</td></tr>
      </tbody>
    </table>
  </div>
</section>

<!-- ══ 03 MAX485 ══════════════════════════════════════════════════ -->
<section id="max485">
  <div class="sec-head">
    <span class="sec-num">03</span>
    <h2>MAX485 — RS485 Transceiver</h2>
    <div class="sec-line"></div>
  </div>

  <div class="diagram-wrap">
    <svg class="svg-diagram" viewBox="0 0 640 200" xmlns="http://www.w3.org/2000/svg">
      <!-- IC outline -->
      <rect x="220" y="40" width="200" height="130" rx="4" fill="#0d1117" stroke="#00e5a0" stroke-width="1.5"></rect>
      <text x="320" y="62" text-anchor="middle" font-size="10" fill="#00e5a0" letter-spacing="2">MAX485</text>
      <line x1="230" y1="68" x2="410" y2="68" stroke="#1e2840" stroke-width="1"></line>
      
      

      <!-- Left pins -->
      <line x1="180" y1="80" x2="220" y2="80" stroke="#9b6dff" stroke-width="1.5"></line>
      <line x1="180" y1="100" x2="220" y2="100" stroke="#9b6dff" stroke-width="1.5" stroke-dasharray="4 2"></line>
      <line x1="180" y1="120" x2="220" y2="120" stroke="#9b6dff" stroke-width="1.5" stroke-dasharray="4 2"></line>
      <line x1="180" y1="140" x2="220" y2="140" stroke="#00e5a0" stroke-width="1.5"></line>

      <!-- Left pin labels (IC side) -->
      <text x="228" y="84" font-size="8" fill="#7a8aaa">RO (1)  Receiver Out</text>
      <text x="228" y="104" font-size="8" fill="#7a8aaa">RE (2)  Receiver En.</text>
      <text x="228" y="124" font-size="8" fill="#7a8aaa">DE (3)  Driver Enable</text>
      <text x="228" y="144" font-size="8" fill="#7a8aaa">DI (4)  Data Input</text>

      <!-- Right pins -->
      <line x1="420" y1="80" x2="460" y2="80" stroke="#3d4f70" stroke-width="1.5"></line>
      <line x1="420" y1="100" x2="460" y2="100" stroke="#3d4f70" stroke-width="1.5"></line>
      <line x1="420" y1="120" x2="460" y2="120" stroke="#f5a623" stroke-width="2"></line>
      <line x1="420" y1="140" x2="460" y2="140" stroke="#f5a623" stroke-width="2"></line>

      <!-- Right pin labels (IC side) -->
      <text x="412" y="84" text-anchor="end" font-size="8" fill="#7a8aaa">GND (5)</text>
      <text x="412" y="104" text-anchor="end" font-size="8" fill="#7a8aaa">VCC (8)</text>
      <text x="412" y="124" text-anchor="end" font-size="8" fill="#f5a623">A   (6)  DMX+</text>
      <text x="412" y="144" text-anchor="end" font-size="8" fill="#f5a623">B   (7)  DMX−</text>

      <!-- Left: ESP32 connections -->
      <rect x="10" y="60" width="130" height="100" rx="3" fill="#111620" stroke="#1e2840" stroke-width="1"></rect>
      <text x="75" y="80" text-anchor="middle" font-size="8" fill="#9b6dff" letter-spacing="1">ESP32</text>
      <text x="75" y="95" text-anchor="middle" font-size="7.5" fill="#7a8aaa">TX → DI (4)</text>
      <text x="75" y="108" text-anchor="middle" font-size="7.5" fill="#7a8aaa">GPIO → DE+RE</text>
      <text x="75" y="121" text-anchor="middle" font-size="7.5" fill="#3d4f70">(gebrückt,</text>
      <text x="75" y="133" text-anchor="middle" font-size="7.5" fill="#3d4f70">1 Pin steuert beide)</text>
      <text x="75" y="150" text-anchor="middle" font-size="7.5" fill="#3d4f70">RO → nicht genutzt</text>

      <line x1="140" y1="95" x2="180" y2="140" stroke="#00e5a0" stroke-width="1.2"></line>
      <line x1="140" y1="108" x2="180" y2="100" stroke="#9b6dff" stroke-width="1.2" stroke-dasharray="3 2"></line>
      <line x1="140" y1="108" x2="180" y2="120" stroke="#9b6dff" stroke-width="1.2" stroke-dasharray="3 2"></line>

      <!-- Right: XLR connections -->
      <rect x="490" y="100" width="130" height="60" rx="3" fill="#111620" stroke="#f5a623" stroke-width="1"></rect>
      <text x="555" y="120" text-anchor="middle" font-size="8" fill="#f5a623" letter-spacing="1">XLR Pin 3 → A</text>
      <text x="555" y="135" text-anchor="middle" font-size="8" fill="#f5a623" letter-spacing="1">XLR Pin 2 → B</text>
      <text x="555" y="150" text-anchor="middle" font-size="7" fill="#3d4f70">XLR Pin 1 → GND</text>

      <!-- DE/RE bridge annotation -->
      <text x="75" y="175" text-anchor="middle" font-size="8" fill="#3d4f70">HIGH = Senden · LOW = Empfangen</text>
      <text x="320" y="182" text-anchor="middle" font-size="7.5" fill="#3d4f70">Wir senden immer → DE/RE dauerhaft HIGH nach Break</text>
    </svg>
  </div>

  <div class="tbl-wrap">
    <table>
      <thead>
        <tr><th>Pin</th><th>Signal</th><th>Verbindung</th><th>Beschreibung</th></tr>
      </thead>
      <tbody>
        <tr><td class="td-mono">1</td><td>RO</td><td class="td-dim">Nicht verbunden</td><td>Receiver Output — wir empfangen nie</td></tr>
        <tr><td class="td-mono">2</td><td>RE̅</td><td class="td-mono">DE/RE GPIO</td><td>Receiver Enable, active LOW — mit DE brücken</td></tr>
        <tr><td class="td-mono">3</td><td>DE</td><td class="td-mono">DE/RE GPIO</td><td>Driver Enable, active HIGH — mit RE brücken</td></tr>
        <tr><td class="td-mono">4</td><td>DI</td><td class="td-mono">UART TX</td><td>Data Input vom ESP32 UART</td></tr>
        <tr><td class="td-mono">5</td><td>GND</td><td class="td-mono">GND</td><td>Masse — mit ESP32 GND verbinden</td></tr>
        <tr><td class="td-mono">6</td><td>A</td><td class="td-mono">XLR Pin 3</td><td>RS485 A = DMX+ (nicht-invertierend)</td></tr>
        <tr><td class="td-mono">7</td><td>B</td><td class="td-mono">XLR Pin 2</td><td>RS485 B = DMX− (invertierend)</td></tr>
        <tr><td class="td-mono">8</td><td>VCC</td><td class="td-mono">3,3 V</td><td>Versorgung — WT32-ETH01 ist 3,3V-only!</td></tr>
      </tbody>
    </table>
  </div>

  <div class="info">
    <strong>DE/RE brücken:</strong> Pin 2 (RE̅) und Pin 3 (DE) direkt auf dem IC mit einem kurzen Drahtbogen verbinden, dann nur eine Leitung zum GPIO. HIGH = Sender aktiv, LOW = Empfänger aktiv. Da wir nur senden, schalten wir vor dem Break auf HIGH und lassen es dort.
  </div>

  <div class="warn">
    <strong>Spannung:</strong> Der WT32-ETH01 ist ein 3,3V-System. MAX485 funktioniert mit 3,3V VCC korrekt — kein Level-Shifter nötig. <strong>Kein 5V anlegen</strong>, das würde den ESP32 beschädigen.
  </div>
</section>

<!-- ══ 04 OLED ════════════════════════════════════════════════════ -->
<section id="oled">
  <div class="sec-head">
    <span class="sec-num">04</span>
    <h2>SSD1306 OLED 128×32</h2>
    <div class="sec-line"></div>
  </div>

  <div class="grid2">
    <div class="card">
      <div class="card-label">Verbindung</div>
      <div class="card-title">I²C Bus</div>
      <div class="card-val">
        SDA → GPIO <span class="hl">4</span><br>
        SCL → GPIO <span class="hl">2</span><br>
        VCC → <span class="hl">3,3 V</span><br>
        GND → GND<br>
        Addr: <span class="hl">0x3C</span>
      </div>
    </div>
    <div class="card">
      <div class="card-label">Anzeige</div>
      <div class="card-title">3 Zeilen</div>
      <div class="card-val">
        <span class="hl">Zeile 1</span> — IP-Adresse<br>
        <span class="hl">Zeile 2</span> — U1:OK U2:OK U3:--<br>
        <span class="hl">Zeile 3</span> — ETH: verbunden<br>
        Update-Rate: <span class="hl2">1 Hz</span>
      </div>
    </div>
  </div>

  <!-- OLED mockup -->
  <div class="diagram-wrap">
    <svg class="svg-diagram" viewBox="0 0 400 120" xmlns="http://www.w3.org/2000/svg">
      <!-- OLED housing -->
      <rect x="80" y="10" width="240" height="100" rx="5" fill="#0a0a0a" stroke="#333" stroke-width="2"></rect>
      <!-- Screen bezel -->
      <rect x="88" y="18" width="224" height="84" rx="3" fill="#050505" stroke="#1a1a1a" stroke-width="1"></rect>
      <!-- Screen active area (128:32 aspect approx) -->
      <rect x="92" y="22" width="216" height="72" rx="2" fill="#0a0e14"></rect>
      <!-- OLED pixel sim -->
      <text x="96" y="40" font-size="11" fill="#00d4ff" font-family="IBM Plex Mono,monospace" letter-spacing=".5">IP: 192.168.1.47</text>
      <text x="96" y="58" font-size="11" fill="#00e5a0" font-family="IBM Plex Mono,monospace" letter-spacing=".5">U1:OK  U2:OK  U3:--</text>
      <text x="96" y="76" font-size="11" fill="#7a8aaa" font-family="IBM Plex Mono,monospace" letter-spacing=".5">ETH: verbunden</text>
      <!-- Label -->
      <text x="200" y="108" text-anchor="middle" font-size="8" fill="#3d4f70">SSD1306 · 128×32 px · I²C</text>
    </svg>
  </div>

  <div class="info">
    Beim Boot zeigt das OLED <strong>"Warte auf ETH..."</strong>. Sobald eine IP per DHCP vergeben wird, erscheint die Adresse. Universen ohne aktives sACN-Signal zeigen <code>--</code> statt <code>OK</code>. Nach 3 Sekunden ohne Paket wechselt der Status und der DMX-Ausgang geht auf Blackout.
  </div>
</section>

<!-- ══ 05 sACN E1.31 ══════════════════════════════════════════════ -->
<section id="sacn">
  <div class="sec-head">
    <span class="sec-num">05</span>
    <h2>sACN E1.31 Protokoll</h2>
    <div class="sec-line"></div>
  </div>

  <div class="grid3">
    <div class="card">
      <div class="card-label">Transport</div>
      <div class="card-title">UDP Multicast</div>
      <div class="card-val">Port <span class="hl">5568</span><br>239.255.U.hi / U.lo<br>U1 = 239.255.0.1</div>
    </div>
    <div class="card">
      <div class="card-label">Paketstruktur</div>
      <div class="card-title">126 Byte Header</div>
      <div class="card-val">ACN Identifier<br>Root / Framing<br>DMP Layer</div>
    </div>
    <div class="card">
      <div class="card-label">Payload</div>
      <div class="card-title">513 Byte</div>
      <div class="card-val">Startcode <span class="hl">0x00</span><br>+ 512 Byte DMX<br>= 639 Byte gesamt</div>
    </div>
  </div>

  <div class="tbl-wrap">
    <table>
      <thead><tr><th>Offset</th><th>Länge</th><th>Wert / Feld</th><th>Beschreibung</th></tr></thead>
      <tbody>
        <tr><td class="td-mono">0–1</td><td>2</td><td class="td-mono">0x00 0x10</td><td>Preamble Size</td></tr>
        <tr><td class="td-mono">4–15</td><td>12</td><td class="td-mono">ACN-E1.17...</td><td>ACN Packet Identifier</td></tr>
        <tr><td class="td-mono">18–21</td><td>4</td><td class="td-mono">0x00000004</td><td>Root Vector</td></tr>
        <tr><td class="td-mono">40–43</td><td>4</td><td class="td-mono">0x00000002</td><td>Framing Vector</td></tr>
        <tr><td class="td-mono">108</td><td>1</td><td>Priority</td><td>Priorität (0–200, Standard: 100)</td></tr>
        <tr><td class="td-mono">113–114</td><td>2</td><td>Universe</td><td>Universum (1-basiert, Big-Endian)</td></tr>
        <tr><td class="td-mono">117</td><td>1</td><td class="td-mono">0x02</td><td>DMP Vector</td></tr>
        <tr><td class="td-mono">123–124</td><td>2</td><td>Property Count</td><td>Anzahl Eigenschaften (= DMX-Len + 1)</td></tr>
        <tr><td class="td-mono">125</td><td>1</td><td class="td-mono">0x00</td><td>DMX Startcode</td></tr>
        <tr><td class="td-mono">126–637</td><td>512</td><td>DMX Data</td><td>Kanal 1–512</td></tr>
      </tbody>
    </table>
  </div>

  <div class="info">
    <strong>Multicast-Gruppen:</strong> Der Node tritt allen 3 Gruppen gleichzeitig bei. Gruppe 1 über <code>WiFiUDP::beginMulticast()</code>, Gruppen 2 und 3 direkt über LwIP IGMP (<code>igmp_joingroup_netif()</code>). Ein Unicast-Sender funktioniert ebenfalls, sofern er an Port 5568 sendet.
  </div>
</section>

<!-- ══ 06 STROMVERSORGUNG ══════════════════════════════════════════ -->
<section id="strom">
  <div class="sec-head">
    <span class="sec-num">06</span>
    <h2>Stromversorgung</h2>
    <div class="sec-line"></div>
  </div>

  <div class="diagram-wrap">
    <svg class="svg-diagram" viewBox="0 0 640 180" xmlns="http://www.w3.org/2000/svg">
      <!-- PSU -->
      <rect x="20" y="50" width="80" height="80" rx="3" fill="#111620" stroke="#f5a623" stroke-width="1.5"></rect>
      <text x="60" y="84" text-anchor="middle" font-size="9" fill="#f5a623" letter-spacing="1">5V PSU</text>
      <text x="60" y="98" text-anchor="middle" font-size="8" fill="#7a8aaa">≥ 500 mA</text>
      <text x="60" y="112" text-anchor="middle" font-size="7.5" fill="#3d4f70">z.B. USB-C PD</text>

      <!-- +5V rail -->
      <line x1="100" y1="72" x2="150" y2="72" stroke="#f5a623" stroke-width="2"></line>
      <line x1="150" y1="38" x2="545" y2="38" stroke="#f5a623" stroke-width="1"></line><line x1="150" y1="38" x2="150" y2="50" stroke="#f5a623" stroke-width="1"></line><line x1="400" y1="38" x2="400" y2="50" stroke="#f5a623" stroke-width="1"></line><line x1="545" y1="38" x2="545" y2="50" stroke="#f5a623" stroke-width="1"></line><line x1="300" y1="38" x2="300" y2="50" stroke="#f5a623" stroke-width="1"></line><text x="150" y="35" font-size="8" fill="#f5a623">+3,3 V (vom WT32-ETH01 3V3-Pin)</text>

      <!-- GND rail -->
      <line x1="100" y1="120" x2="545" y2="120" stroke="#3d4f70" stroke-width="2" stroke-dasharray="5 3"></line>
      <line x1="545" y1="100" x2="545" y2="120" stroke="#3d4f70" stroke-width="2" stroke-dasharray="5 3"></line><text x="106" y="135" font-size="8" fill="#3d4f70">GND (gemeinsame Masse)</text>

      <!-- Verbraucher: WT32 -->
      <rect x="140" y="50" width="90" height="40" rx="2" fill="#111620" stroke="#00d4ff" stroke-width="1"></rect>
      <text x="185" y="66" text-anchor="middle" font-size="8" fill="#00d4ff">WT32-ETH01</text>
      <text x="185" y="79" text-anchor="middle" font-size="7.5" fill="#3d4f70">5V → 3V3 intern</text>
      <text x="185" y="89" text-anchor="middle" font-size="7" fill="#3d4f70">~150 mA</text>
      <line x1="185" y1="90" x2="185" y2="120" stroke="#3d4f70" stroke-width="1" stroke-dasharray="3 2"></line>

      <!-- OLED -->
      <rect x="255" y="50" width="80" height="40" rx="2" fill="#111620" stroke="#9b6dff" stroke-width="1"></rect>
      <text x="295" y="66" text-anchor="middle" font-size="8" fill="#9b6dff">OLED</text>
      <text x="295" y="77" text-anchor="middle" font-size="7.5" fill="#3d4f70">3,3 V</text>
      <text x="295" y="88" text-anchor="middle" font-size="7" fill="#3d4f70">~10 mA</text>
      
      <line x1="295" y1="90" x2="295" y2="120" stroke="#3d4f70" stroke-width="1" stroke-dasharray="3 2"></line>

      <!-- MAX485 #1 -->
      <rect x="360" y="50" width="70" height="40" rx="2" fill="#111620" stroke="#00e5a0" stroke-width="1"></rect>
      <text x="395" y="66" text-anchor="middle" font-size="8" fill="#00e5a0">MAX485 ×3</text>
      <text x="395" y="77" text-anchor="middle" font-size="7.5" fill="#3d4f70">3,3 V je</text>
      <text x="395" y="88" text-anchor="middle" font-size="7" fill="#3d4f70">~60 mA</text>
      
      <line x1="395" y1="90" x2="395" y2="120" stroke="#3d4f70" stroke-width="1" stroke-dasharray="3 2"></line>

      <!-- Total -->
      <rect x="480" y="44" width="120" height="52" rx="3" fill="#181e2a" stroke="#f5a623" stroke-width="1"></rect>
      <text x="540" y="62" text-anchor="middle" font-size="8" fill="#f5a623" letter-spacing="1">GESAMT</text>
      <text x="540" y="76" text-anchor="middle" font-size="9" fill="#fff">~220 mA</text>
      <text x="540" y="89" text-anchor="middle" font-size="7.5" fill="#7a8aaa">Empf.: ≥ 500 mA</text>
    </svg>
  </div>

  <div class="grid4">
    <div class="card">
      <div class="card-label">WT32-ETH01</div>
      <div class="card-title">~150 mA</div>
      <div class="card-val">ESP32 + ETH PHY<br>5V am VIN-Pin</div>
    </div>
    <div class="card">
      <div class="card-label">OLED SSD1306</div>
      <div class="card-title">~10 mA</div>
      <div class="card-val">3,3V<br>Vom 3V3-Pin</div>
    </div>
    <div class="card">
      <div class="card-label">MAX485 × 3</div>
      <div class="card-title">~60 mA</div>
      <div class="card-val">3,3V je IC<br>Vom 3V3-Pin</div>
    </div>
    <div class="card" style="border-left:2px solid var(--amber)">
      <div class="card-label">Gesamt</div>
      <div class="card-title" style="color:var(--amber)">~220 mA</div>
      <div class="card-val"><span class="hl3">500 mA</span> empfohlen<br>Sicherheitspuffer</div>
    </div>
  </div>

  <div class="warn">
    <strong>Spannungsversorgung:</strong>
    <ul>
      <li>5V an den <strong>5V-Pin</strong> (oder VIN) des WT32-ETH01 anlegen — <em>nicht</em> an 3V3</li>
      <li>OLED und MAX485 an den <strong>3V3-Pin</strong> des WT32-ETH01 hängen — der interne LDO liefert genug</li>
      <li>Alle GND-Pins auf einer gemeinsamen Masseschiene verbinden</li>
      <li>Kurze, dicke Drähte für GND und VCC — besonders auf dem Breadboard</li>
    </ul>
  </div>
</section>

<!-- ══ 07 XLR ════════════════════════════════════════════════════ -->
<section id="xlr">
  <div class="sec-head">
    <span class="sec-num">07</span>
    <h2>XLR-3 Ausgänge &amp; DMX-Timing</h2>
    <div class="sec-line"></div>
  </div>

  <div class="grid2">
    <div>
      <div class="diagram-wrap">
        <svg class="svg-diagram" viewBox="0 0 300 180" xmlns="http://www.w3.org/2000/svg">
          <!-- XLR female -->
          <ellipse cx="150" cy="90" rx="75" ry="75" fill="#111620" stroke="#f5a623" stroke-width="2"></ellipse>
          <ellipse cx="150" cy="90" rx="68" ry="68" fill="none" stroke="#1e2840" stroke-width="1"></ellipse>
          <!-- Pins -->
          <circle cx="150" cy="48" r="12" fill="#0d1117" stroke="#7a8aaa" stroke-width="1.5"></circle>
          <text x="150" y="53" text-anchor="middle" font-size="9" fill="#7a8aaa" font-weight="500">1</text>
          <circle cx="110" cy="118" r="12" fill="#0d1117" stroke="#f5a623" stroke-width="1.5"></circle>
          <text x="110" y="123" text-anchor="middle" font-size="9" fill="#f5a623" font-weight="500">2</text>
          <circle cx="190" cy="118" r="12" fill="#0d1117" stroke="#f5a623" stroke-width="1.5"></circle>
          <text x="190" y="123" text-anchor="middle" font-size="9" fill="#f5a623" font-weight="500">3</text>

          <!-- Labels -->
          <text x="150" y="26" text-anchor="middle" font-size="8" fill="#7a8aaa">Pin 1 · GND / Schirm</text>
          <text x="64" y="134" text-anchor="middle" font-size="7.5" fill="#f5a623">2 · DMX−</text>
          <text x="236" y="134" text-anchor="middle" font-size="7.5" fill="#f5a623">3 · DMX+</text>
          <text x="150" y="162" text-anchor="middle" font-size="7.5" fill="#3d4f70">Female XLR · Blick von vorne</text>

          <!-- MAX485 labels -->
          <text x="64" y="148" text-anchor="middle" font-size="7" fill="#3d4f70">B (Pin 7)</text>
          <text x="236" y="148" text-anchor="middle" font-size="7" fill="#3d4f70">A (Pin 6)</text>
        </svg>
      </div>
    </div>

    
  </div>

  <div class="tbl-wrap">
    <table>
      <thead><tr><th>XLR Pin</th><th>Signal</th><th>MAX485 Pin</th><th>Farbe (Empfehlung)</th></tr></thead>
      <tbody>
        <tr><td class="td-mono">1</td><td>GND / Schirm</td><td class="td-dim">GND</td><td class="td-dim">Schwarz</td></tr>
        <tr><td class="td-mono">2</td><td>DMX− (Data−)</td><td class="td-mono">B (7)</td><td>Blau / Grün</td></tr>
        <tr><td class="td-mono">3</td><td>DMX+ (Data+)</td><td class="td-mono">A (6)</td><td>Weiß / Rot</td></tr>
        
      </tbody>
    </table>
  </div>

  <div class="info">
    <strong>Leitungslänge:</strong> Kurze Verbindung von MAX485 zum XLR-Stecker, max. 15 cm auf dem Breadboard. Für längere Kabelwege an echten Geräten ist der XLR-Stecker der Ausgangspunkt — das DMX-Kabel vom XLR aus kann bis zu 300 m lang sein.
  </div>

  <hr>

  <div class="sec-head" style="margin-top:0;">
    <span class="sec-num">→</span>
    <h2>Benötigte Bibliotheken</h2>
    <div class="sec-line"></div>
  </div>

  <div class="tbl-wrap">
    <table>
      <thead><tr><th>Bibliothek</th><th>Quelle</th><th>Zweck</th></tr></thead>
      <tbody>
        <tr>
          <td><span class="td-mono">ETH.h</span></td>
          <td class="td-dim">ESP32 Arduino Core</td>
          <td>Ethernet, LAN8720A PHY, DHCP</td>
        </tr>
        <tr>
          <td><span class="td-mono">WiFiUdp.h</span></td>
          <td class="td-dim">ESP32 Arduino Core</td>
          <td>UDP Multicast Empfang</td>
        </tr>
        <tr>
          <td><span class="td-mono">Adafruit SSD1306</span></td>
          <td class="td-green"><span class="dot dot-green"></span>Arduino Library Manager</td>
          <td>OLED Display Treiber</td>
        </tr>
        <tr>
          <td><span class="td-mono">Adafruit GFX</span></td>
          <td class="td-green"><span class="dot dot-green"></span>Arduino Library Manager</td>
          <td>Grafik-Basis für SSD1306</td>
        </tr>
        <tr>
          <td><span class="td-mono">Wire.h</span></td>
          <td class="td-dim">ESP32 Arduino Core</td>
          <td>I²C für OLED</td>
        </tr>
      </tbody>
    </table>
  </div>

  <div class="code-block"><span class="cm">// Board: "ESP32 Dev Module" oder "WT32-ETH01"
// CPU: 240 MHz · Flash: QIO 80 MHz · PSRAM: disabled
// Partition: default (4MB)</span>

<span class="cm">// Arduino IDE: Werkzeuge → Board → ESP32 Arduino → ESP32 Dev Module</span>
<span class="cm">// Platform.io: platform = espressif32, board = wt32-eth01</span></div>
</section>

<footer>
  <span>sACN Node · WT32-ETH01 · 3× DMX512 · sACN E1.31</span>
  <span>Hardware-UART · kein SoftwareSerial · 250 kBaud</span>
</footer>



</body></html>
