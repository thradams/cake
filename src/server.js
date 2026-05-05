const API = "http://localhost:8080";

let currentPath = "";
let currentFile = "";
let lastMessages = [];   // list of { line, text } — rebuilt whenever a file loads or compiles
let diagIndex = -1;      // index into lastMessages for Prev/Next navigation
let isDirty = false;     // true after any edit; annotations are hidden until save/compile

// ── Status bar helpers ────────────────────────────────────────────
// Defined here (before any async call) so catch blocks always have them,
// regardless of which <script> tag the status bar CSS/HTML lives in.
let _statusTimer = null;
function setStatus(msg, ms)
{
    const el = document.getElementById("status-msg");
    if (!el) return;
    el.textContent = msg;
    el.className = "visible";
    el.style.cssText = "opacity:0.85; color:inherit; font-weight:normal; transition:opacity 1s;";
    clearTimeout(_statusTimer);
    if (ms) _statusTimer = setTimeout(() =>
    {
        el.style.opacity = "0";
    }, ms);
}
function setError(msg)
{
    const el = document.getElementById("status-msg");
    if (!el) { console.error(msg); return; }
    el.textContent = "\u2716 " + msg;
    el.className = "error";
    el.style.cssText = "opacity:1 !important; color:#ff5f5f; font-weight:600; transition:none;";
    clearTimeout(_statusTimer);
}

function setDiagMessages(messages)
{
    lastMessages = messages;
    diagIndex = messages.length > 0 ? 0 : -1;
    updateDiagCounter();
    if (typeof updateStatusDiags === "function") updateStatusDiags(messages);
}

function updateDiagCounter()
{
    const counter = document.getElementById("diagCounter");
    if (!counter) return;
    if (lastMessages.length === 0)
    {
        counter.textContent = "";
    }
    else
    {
        counter.textContent = `${diagIndex + 1}/${lastMessages.length}`;
    }

    // sync selected highlight in output panel
    document.querySelectorAll(".output-diag.selected").forEach(el => el.classList.remove("selected"));
    if (diagIndex >= 0 && diagIndex < lastMessages.length)
    {
        const targetLine = lastMessages[diagIndex].line;
        const el = document.querySelector(`.output-diag[data-line="${targetLine}"]`);
        if (el)
        {
            el.classList.add("selected");
            el.scrollIntoView({ block: "nearest" });
        }
    }
}

function diagNext()
{
    if (lastMessages.length === 0) return;
    diagIndex = (diagIndex + 1) % lastMessages.length;
    updateDiagCounter();
    jumpToLine(lastMessages[diagIndex].line);
}

function diagPrev()
{
    if (lastMessages.length === 0) return;
    diagIndex = (diagIndex - 1 + lastMessages.length) % lastMessages.length;
    updateDiagCounter();
    jumpToLine(lastMessages[diagIndex].line);
}

function diagFirst()
{
    if (lastMessages.length === 0) return;
    diagIndex = 0;
    updateDiagCounter();
    jumpToLine(lastMessages[diagIndex].line);
}

function diagLast()
{
    if (lastMessages.length === 0) return;
    diagIndex = lastMessages.length - 1;
    updateDiagCounter();
    jumpToLine(lastMessages[diagIndex].line);
}

async function loadFiles()
{
    try
    {
        const path = document.getElementById("pathInput").value;
        currentPath = path;

        const res = await fetch(`${API}/list?path=${encodeURIComponent(path)}`);
        if (!res.ok) throw new Error(`Server ${res.status}: ${res.statusText}`);
        const files = await res.json();

        const select = document.getElementById("fileList");
        const saveBtn = document.getElementById("saveBtn");
        const reloadBtn = document.getElementById("saveBtn");

        select.innerHTML = '<option value="">-- files --</option>';
        saveBtn.disabled = true;
        reloadBtn.disabled = true;

        files.forEach(f =>
        {
            const opt = document.createElement("option");
            opt.value = f;
            opt.textContent = f;
            select.appendChild(opt);
        });

        // Build IDE file tree if available
        if (typeof buildFileTree === "function") buildFileTree(files);

        if (files.length > 0)
        {
            select.selectedIndex = 1;
            await onFileSelect();
        }
    }
    catch (err)
    {
        setError("Load files: " + err.message);
    }
}

async function onFileSelect()
{
    try
    {
        const select = document.getElementById("fileList");
        const file = select.value;

        if (!file) return;

        currentFile = file;
        document.getElementById("saveBtn").disabled = false;
        document.getElementById("reloadBtn").disabled = false;

        // Sync IDE tab/tree/status if available
        if (typeof ensureTab === "function") ensureTab(file);
        if (typeof setActiveTree === "function") setActiveTree(file);
        if (typeof updateStatusFile === "function") updateStatusFile(file);

        await readFileFromServer(file);
    }
    catch (err)
    {
        setError("Open file: " + err.message);
    }
}

const DELIM = "=====CAKE-OUTPUT=====";

async function reload()
{
    try
    {
        const select = document.getElementById("fileList");
        const file = select.value;
        await readFileFromServer(file);
    }
    catch (err)
    {
        setError("Reload: " + err.message);
    }
}

async function readFileFromServer(file)
{
    try
    {
        // 1. Read plain source
        const readRes = await fetch(
            `${API}/read?path=${encodeURIComponent(currentPath)}&file=${encodeURIComponent(file)}`
        );
        if (!readRes.ok) throw new Error(`Read ${file}: server ${readRes.status}`);
        const text = await readRes.text();

        document.getElementById("c-editor").value = text;
        isDirty = false;
        highlight.innerHTML = highlightC(editor.value) + "\n";
        updateGutter();

        // 2. Compile the file as-is on disk
        const compRes = await fetch(`${API}/compile`, {
            method: "POST",
            headers: { "Content-Type": "text/plain" },
            body: file
        });
        if (!compRes.ok) throw new Error(`Compile ${file}: server ${compRes.status}`);
        const output = await compRes.text();

        var a = parseCompilerLines(output);
        setDiagMessages(a);

        var s = highlightC(editor.value) + "\n";
        highlight.innerHTML = appendMessagesToLines(s, a);

        document.getElementById("output").innerHTML = renderOutput(output);
    }
    catch (err)
    {
        setError(err.message);
    }
}

async function loadFile(path, file)
{
    try
    {
        const res = await fetch(
            `${API}/file?path=${encodeURIComponent(path)}&file=${encodeURIComponent(file)}`
        );
        if (!res.ok) throw new Error(`Server ${res.status}: ${res.statusText}`);
        const text = await res.text();
        document.getElementById("c-editor").value = text;
    }
    catch (err)
    {
        setError("Load file: " + err.message);
    }
}


async function saveFile()
{
    if (!currentFile) return;
    try
    {
        const content = document.getElementById("c-editor").value;

        const res = await fetch(`${API}/save`, {
            method: "POST",
            headers: { "Content-Type": "text/plain" },
            body: currentFile + "\n" + content
        });
        if (!res.ok) throw new Error(`Server ${res.status}: ${res.statusText}`);

        isDirty = false;
        updateHighlight();
        if (typeof setTabDirty === "function") setTabDirty(currentFile, false);
        setStatus("\u2713 Saved", 3000);
    }
    catch (err)
    {
        setError("Save: " + err.message);
    }
}



async function saveAndCompile()
{
    if (!currentFile) return;
    try
    {
        const content = document.getElementById("c-editor").value;

        const res = await fetch(`${API}/savecompile`, {
            method: "POST",
            headers: { "Content-Type": "text/plain" },
            body: currentFile + "\n" + content
        });
        if (!res.ok) throw new Error(`Server ${res.status}: ${res.statusText}`);

        const output = await res.text();

        var a = parseCompilerLines(output);
        setDiagMessages(a);

        var s = highlightC(editor.value) + "\n";
        isDirty = false;
        highlight.innerHTML = appendMessagesToLines(s, a);
        updateGutter();

        document.getElementById("output").innerHTML = renderOutput(output);
    }
    catch (err)
    {
        setError("Save & Compile: " + err.message);
    }
}


async function compile()
{
    if (!currentFile) return;
    try
    {
        const res = await fetch(`${API}/compile`, {
            method: "POST",
            headers: { "Content-Type": "text/plain" },
            body: currentFile
        });
        if (!res.ok) throw new Error(`Server ${res.status}: ${res.statusText}`);

        const output = await res.text();

        var a = parseCompilerLines(output);
        setDiagMessages(a);

        var s = highlightC(editor.value) + "\n";
        isDirty = false;
        highlight.innerHTML = appendMessagesToLines(s, a);
        updateGutter();

        document.getElementById("output").innerHTML = renderOutput(output);
    }
    catch (err)
    {
        setError("Compile: " + err.message);
    }
}

const ANSI_FG = ['#4e4e4e', '#e74c3c', '#2ecc71', '#f1c40f', '#3498db', '#9b59b6', '#1abc9c', '#ecf0f1'];
const ANSI_BG = ['#1e1e1e', '#c0392b', '#27ae60', '#d4ac0d', '#2980b9', '#8e44ad', '#17a589', '#bdc3c7'];
const ANSI_FG_BRIGHT = ['#888888', '#ff5555', '#55ff55', '#ffff55', '#5555ff', '#ff55ff', '#55ffff', '#ffffff'];

function escHtml(s)
{
    return s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
}

function renderOutput(text)
{
    // render one raw ANSI line into an HTML string
    function renderLine(raw)
    {
        let fg = null, bg = null, bold = false, dim = false, italic = false, underline = false;
        let html = '';
        const re = /\x1b\[([0-9;]*)m/g;
        let last = 0, m;

        function openSpan()
        {
            const styles = [];
            if (bold) styles.push('font-weight:bold');
            if (dim) styles.push('opacity:0.5');
            if (italic) styles.push('font-style:italic');
            if (underline) styles.push('text-decoration:underline');
            if (fg) styles.push('color:' + fg);
            if (bg) styles.push('background:' + bg);
            if (styles.length) html += '<span style="' + styles.join(';') + '">';
            return styles.length > 0;
        }

        function applyCode(c)
        {
            if (c === 0) { fg = bg = null; bold = dim = italic = underline = false; }
            else if (c === 1) bold = true;
            else if (c === 2) dim = true;
            else if (c === 3) italic = true;
            else if (c === 4) underline = true;
            else if (c === 22) { bold = false; dim = false; }
            else if (c === 23) italic = false;
            else if (c === 24) underline = false;
            else if (c >= 30 && c <= 37) fg = ANSI_FG[c - 30];
            else if (c === 39) fg = null;
            else if (c >= 40 && c <= 47) bg = ANSI_BG[c - 40];
            else if (c === 49) bg = null;
            else if (c >= 90 && c <= 97) fg = ANSI_FG_BRIGHT[c - 90];
            else if (c >= 100 && c <= 107) bg = ANSI_FG_BRIGHT[c - 100];
        }

        while ((m = re.exec(raw)) !== null)
        {
            const plain = raw.slice(last, m.index);
            if (plain) { const h = openSpan(); html += escHtml(plain); if (h) html += '</span>'; }
            last = m.index + m[0].length;
            (m[1] === '' ? [0] : m[1].split(';').map(Number)).forEach(applyCode);
        }

        const tail = raw.slice(last);
        if (tail) { const h = openSpan(); html += escHtml(tail); if (h) html += '</span>'; }

        return html;
    }

    // split into raw lines, wrap every line in a span so CSS can target blank ones
    const rawLines = text.split('\n');
    let html = '';

    for (const raw of rawLines)
    {
        const stripped = raw.replace(/\x1b\[[0-9;]*m/g, '').trim();

        if (!stripped) continue;

        const diagMatch = stripped.match(/\w+\.c:(\d+):\d+:\s*(warning|error|note)/);

        if (diagMatch)
        {
            const lineNum = parseInt(diagMatch[1], 10);
            html += `<span class="output-line output-diag" data-line="${lineNum}" onclick="outputDiagClick(this)">${renderLine(raw)}</span>`;
        }
        else
        {
            html += `<span class="output-line">${renderLine(raw)}</span>`;
        }
    }

    return html;
}
function appendToLine(text, targetLine, htmlToAppend)
{
    let line = 1;
    let i = 0;
    let start = 0;

    // find start of target line
    while (i < text.length)
    {
        if (line === targetLine)
        {
            start = i;
            break;
        }

        if (text[i] === '\n')
        {
            line++;
        }
        i++;
    }

    // if line not found, return original
    if (line !== targetLine) return text;

    // find end of the line
    let end = start;
    while (end < text.length && text[end] !== '\n')
    {
        end++;
    }

    // split and insert
    const before = text.slice(0, end);
    const after = text.slice(end);

    return before + htmlToAppend + after;
}
function appendMessagesToLines(input, messages)
{
    const sorted = [...messages].sort((a, b) => a.line - b.line);

    const lines = input.split('\n');
    const result = [];

    let msgIndex = 0;

    for (let i = 0; i < lines.length; i++)
    {
        let line = lines[i];
        const currentLineNumber = i + 1;

        while (msgIndex < sorted.length && sorted[msgIndex].line === currentLineNumber)
        {
            const msg = sorted[msgIndex];

            let cls, icon;
            if (msg.severity === 'error') { cls = 'diag-error'; icon = '\u2716 '; }
            else if (msg.severity === 'warning') { cls = 'diag-warning'; icon = '\u26a0 '; }
            else { cls = 'diag-note'; icon = '\u25cf '; }

            line += `<span class="${cls}">${icon}${escHtml(msg.text)}</span>`;
            msgIndex++;
        }

        result.push(line);
    }

    return result.join('\n');
}
function stripAnsi(str)
{
    return str.replace(/\x1b\[[0-9;]*m/g, '');
}

function parseCompilerLines(input)
{
    const result = [];
    const seen = new Set();
    const lines = input.split('\n');

    for (let i = 0; i < lines.length; i++)
    {
        const clean = stripAnsi(lines[i]);

        // match: file.c:LINE:COL: (warning|error|note) [ID]: message
        // ID can be a number ("warning 28:") or a bracketed gcc flag ("warning [-Wfoo]:")
        const m = clean.match(/\w+\.c:(\d+):\d+:\s*(warning|error|note)\s*(\d+|\[-[^\]]+\])?[^:]*:\s*(.*)/);
        if (!m) continue;

        const lineNumber = parseInt(m[1], 10);
        const severity = m[2].toLowerCase();   // "warning" | "error" | "note"
        const warnId = m[3] ? m[3] : '';      // e.g. "28" or "[-Wunused-variable]" or ""
        const message = m[4].trim();

        if (!lineNumber || !message) continue;

        // deduplicate: same line + severity + message
        const key = `${lineNumber}|${severity}|${message}`;
        if (seen.has(key)) continue;
        seen.add(key);

        // Prepend the warning ID to the message when present: "28: value is always non-zero"
        const displayText = warnId ? `${warnId}: ${message}` : message;

        result.push({
            line: lineNumber,
            severity,              // used by appendMessagesToLines and navigation
            text: displayText
        });
    }

    return result;
}

const KW_SPECIAL = new Set([
    "_Opt", "_Owner", "_View", "_Dtor", "_Ctor"
]);

const KW = new Set([
    // types
    "void", "char", "short", "int", "long", "float", "double", "signed", "unsigned",
    "bool", "_Bool", "_Complex", "_Imaginary", "_BitInt", "_Decimal32", "_Decimal64", "_Decimal128",
    // type qualifiers
    "const", "volatile", "restrict", "_Atomic",
    // storage class
    "auto", "extern", "register", "static", "typedef", "inline", "_Thread_local",
    // control flow
    "if", "else", "for", "while", "do", "switch", "case", "default", "break", "continue", "return", "goto",
    // structure
    "struct", "union", "enum",
    // operators/expressions
    "sizeof", "alignof", "_Alignof", "typeof", "typeof_unqual",
    // alignment
    "alignas", "_Alignas",
    // static assert
    "static_assert", "_Static_assert",
    // noreturn
    "noreturn", "_Noreturn",
    // generic
    "_Generic",
    // thread
    "_Thread_local",
    // nullptr
    "nullptr",
    // constexpr (C23)
    "constexpr",
    // common constants
    "NULL", "true", "false",

    // cake
    "throw", "try", "catch",
    "_Count", "assert_state", "override_state",
]);

function highlightC(code)
{
    const out = [];
    const n = code.length;
    let i = 0;

    // emit plain text with HTML escaping
    function flush(start, end)
    {
        for (let j = start; j < end; j++)
        {
            const c = code[j];
            if (c === '&') out.push('&amp;');
            else if (c === '<') out.push('&lt;');
            else if (c === '>') out.push('&gt;');
            else out.push(c);
        }
    }

    function span(cls, start, end)
    {
        out.push('<span class="', cls, '">');
        flush(start, end);
        out.push('</span>');
    }

    while (i < n)
    {
        const c = code[i];

        // line comment  //
        if (c === '/' && code[i + 1] === '/')
        {
            const start = i;
            while (i < n && code[i] !== '\n') i++;
            span('com', start, i);
            continue;
        }

        // block comment  /* */
        if (c === '/' && code[i + 1] === '*')
        {
            const start = i;
            i += 2;
            while (i < n && !(code[i - 1] === '*' && code[i] === '/')) i++;
            if (i < n) i++; // consume closing /
            span('com', start, i);
            continue;
        }

        // string literal  "..."
        if (c === '"')
        {
            const start = i++;
            while (i < n && code[i] !== '"')
            {
                if (code[i] === '\\') i++; // skip escape
                i++;
            }
            if (i < n) i++; // closing "
            span('str', start, i);
            continue;
        }

        // char literal  '.'
        if (c === '\'')
        {
            const start = i++;
            while (i < n && code[i] !== '\'')
            {
                if (code[i] === '\\') i++;
                i++;
            }
            if (i < n) i++;
            span('str', start, i);
            continue;
        }

        // preprocessor directive  #... whole line
        if (c === '#')
        {
            const start = i;
            while (i < n && code[i] !== '\n') i++;
            span('kw', start, i);
            continue;
        }

        // number  (starts with digit, or . followed by digit)
        if (c >= '0' && c <= '9')
        {
            const start = i++;
            while (i < n && (
                (code[i] >= '0' && code[i] <= '9') ||
                (code[i] >= 'a' && code[i] <= 'f') ||
                (code[i] >= 'A' && code[i] <= 'F') ||
                code[i] === 'x' || code[i] === 'X' ||
                code[i] === '.' || code[i] === 'u' ||
                code[i] === 'U' || code[i] === 'l' || code[i] === 'L'
            )) i++;
            span('num', start, i);
            continue;
        }

        // identifier or keyword
        if (c === '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        {
            const start = i++;
            while (i < n && (code[i] === '_' || (code[i] >= 'a' && code[i] <= 'z') || (code[i] >= 'A' && code[i] <= 'Z') || (code[i] >= '0' && code[i] <= '9'))) i++;
            const word = code.slice(start, i);
            if (KW.has(word))
                span('kw', start, i);
            else if (KW_SPECIAL.has(word))
                span('kwex', start, i);
            else
                flush(start, i);
            continue;
        }

        // anything else — emit as-is
        flush(i, i + 1);
        i++;
    }

    return out.join('');
}

const editor = document.getElementById("c-editor");
const highlight = document.getElementById("highlight");
const gutter = document.getElementById("gutter");
const gutterInner = document.getElementById("gutter-inner");

function updateGutter()
{
    const lines = editor.value.split("\n").length;
    const nums = [];
    for (let i = 1; i <= lines; i++) nums.push(i);
    gutterInner.textContent = nums.join("\n");
}

function updateHighlight()
{
    var s = highlightC(editor.value) + "\n";
    highlight.innerHTML = isDirty ? s : appendMessagesToLines(s, lastMessages);
    updateGutter();
}

// ── Debounced highlight ───────────────────────────────────────
// Syntax highlighting is expensive — skip it while the user is actively
// typing fast and only flush after 80 ms of silence.  The gutter only
// needs rebuilding when the number of lines changes, so we track that
// separately to avoid the heavy split("\n") on every keystroke.

let _highlightTimer = null;
let _highlightPending = false;
let _lastLineCount = editor.value.split("\n").length;

// Debounce delay scales with file size:
//   < 5 KB  →  20 ms  (feels instant for small files)
//   ~ 50 KB →  80 ms  (original value, comfortable for mid-size)
//   ≥ 200 KB → 300 ms (avoids janking on very large files)
function highlightDelay()
{
    const bytes = editor.value.length;
    if (bytes < 5000) return 20;
    if (bytes < 200000) return Math.round(20 + (bytes - 5000) / (200000 - 5000) * 280);
    return 300;
}

function scheduleHighlight()
{
    if (_highlightPending) return;          // one rAF already queued
    clearTimeout(_highlightTimer);
    _highlightTimer = setTimeout(() =>
    {
        _highlightPending = true;
        requestAnimationFrame(() =>
        {
            _highlightPending = false;
            var s = highlightC(editor.value) + "\n";
            highlight.innerHTML = isDirty ? s : appendMessagesToLines(s, lastMessages);

            // only rebuild gutter when line count actually changes
            const newCount = editor.value.split("\n").length;
            if (newCount !== _lastLineCount)
            {
                _lastLineCount = newCount;
                updateGutter();
            }
        });
    }, highlightDelay());
}

// update on typing
editor.addEventListener("input", () => { isDirty = true; scheduleHighlight(); });

// ── Tab key → insert spaces; Arrow keys → skip highlight ─────
const ARROW_KEYS = new Set(["ArrowUp", "ArrowDown", "ArrowLeft", "ArrowRight"]);

editor.addEventListener("keydown", (e) =>
{
    // Arrow keys never change content — cancel any queued highlight
    // so navigation feels instant with no deferred DOM churn.
    if (ARROW_KEYS.has(e.key))
    {
        clearTimeout(_highlightTimer);
        return;
    }

    if (e.key !== "Tab") return;
    e.preventDefault();
    const start = editor.selectionStart;
    const end = editor.selectionEnd;
    editor.value = editor.value.slice(0, start) + "    " + editor.value.slice(end);
    editor.selectionStart = editor.selectionEnd = start + 4;
    isDirty = true;
    scheduleHighlight();
});

// initialize on load
updateHighlight();

// textarea scroll drives highlight scroll (both axes) and gutter
editor.addEventListener("scroll", () =>
{
    highlight.scrollLeft = editor.scrollLeft;
    highlight.scrollTop = editor.scrollTop;
    gutterInner.style.transform = `translateY(-${editor.scrollTop}px)`;
});

// wheel on editor forwards scroll to highlight (vertical and horizontal)
editor.addEventListener("wheel", (e) =>
{
    highlight.scrollTop += e.deltaY;
    highlight.scrollLeft += e.deltaX;
    editor.scrollTop = highlight.scrollTop;
    editor.scrollLeft = highlight.scrollLeft;
    gutterInner.style.transform = `translateY(-${highlight.scrollTop}px)`;
}, { passive: true });

// measure actual scrollbar width and adjust textarea to expose it
(function adjustForScrollbar()
{
    const scrollbarWidth = highlight.offsetWidth - highlight.clientWidth;
    const w = scrollbarWidth > 0 ? scrollbarWidth : 17;
    editor.style.width = `calc(100% - ${w}px)`;

    const proxy = document.getElementById("scrollbar-proxy");
    proxy.style.width = w + "px";

    // make proxy a real scrollable with same scroll height as highlight
    // so its native scrollbar works, then sync both ways
    const inner = document.createElement("div");
    inner.style.height = highlight.scrollHeight + "px";
    inner.style.width = "1px";
    proxy.appendChild(inner);
    proxy.style.overflowY = "scroll";
    proxy.style.overflowX = "hidden";

    let fromProxy = false;
    let fromHighlight = false;

    proxy.addEventListener("scroll", () =>
    {
        if (fromHighlight) return;
        fromProxy = true;
        highlight.scrollTop = proxy.scrollTop;
        editor.scrollTop = proxy.scrollTop;
        gutterInner.style.transform = `translateY(-${proxy.scrollTop}px)`;
        fromProxy = false;
    });

    highlight.addEventListener("scroll", () =>
    {
        if (fromProxy) return;
        fromHighlight = true;
        proxy.scrollTop = highlight.scrollTop;
        fromHighlight = false;
    });

    // horizontal scrollbar proxy
    const proxyX = document.getElementById("scrollbar-proxy-x");
    const innerX = document.createElement("div");
    innerX.style.width = highlight.scrollWidth + "px";
    innerX.style.height = "1px";
    proxyX.appendChild(innerX);
    proxyX.style.overflowX = "scroll";
    proxyX.style.overflowY = "hidden";

    let fromProxyX = false;
    let fromHighlightX = false;

    proxyX.addEventListener("scroll", () =>
    {
        if (fromHighlightX) return;
        fromProxyX = true;
        highlight.scrollLeft = proxyX.scrollLeft;
        editor.scrollLeft = proxyX.scrollLeft;
        fromProxyX = false;
    });

    highlight.addEventListener("scroll", () =>
    {
        if (fromProxyX) return;
        fromHighlightX = true;
        proxyX.scrollLeft = highlight.scrollLeft;
        fromHighlightX = false;
    });

    // keep inner dimensions in sync when content changes
    const mo = new MutationObserver(() =>
    {
        inner.style.height = highlight.scrollHeight + "px";
        innerX.style.width = highlight.scrollWidth + "px";
    });
    mo.observe(highlight, { childList: true, subtree: true, characterData: true });
})();

// click a diagnostic line in the output panel
function outputDiagClick(el)
{
    // clear previous selection
    const prev = document.querySelector(".output-diag.selected");
    if (prev) prev.classList.remove("selected");
    el.classList.add("selected");

    const targetLine = parseInt(el.dataset.line, 10);
    if (!targetLine || targetLine < 1) return;

    // sync diagIndex to the matching entry in lastMessages
    const idx = lastMessages.findIndex(m => m.line === targetLine);
    if (idx !== -1)
    {
        diagIndex = idx;
        updateDiagCounter();
    }

    jumpToLine(targetLine);
}

function jumpToLine(lineNumber)
{
    const lines = editor.value.split("\n");
    if (lineNumber > lines.length) return;

    // calculate char offset to start of that line
    let offset = 0;
    for (let i = 0; i < lineNumber - 1; i++)
    {
        offset += lines[i].length + 1; // +1 for \n
    }

    // set cursor at start of target line
    editor.focus();
    editor.setSelectionRange(offset, offset + lines[lineNumber - 1].length);

    // scroll editor so the line is vertically centered
    const lineHeight = parseFloat(getComputedStyle(editor).lineHeight);
    const viewHeight = editor.clientHeight;
    const targetScrollTop = (lineNumber - 1) * lineHeight - viewHeight / 2 + lineHeight / 2;
    editor.scrollTop = Math.max(0, targetScrollTop);

    // sync highlight and gutter
    highlight.scrollTop = editor.scrollTop;
    gutterInner.style.transform = `translateY(-${editor.scrollTop}px)`;
}

// Auto-load on page start
window.addEventListener("load", loadFiles);

// Ctrl+S to save; F8 / Shift+F8 for diag navigation
// Ctrl+Down = next, Ctrl+Up = prev, Home = first, End = last
document.addEventListener("keydown", (e) =>
{
    if (e.ctrlKey && e.key === 's')
    {
        e.preventDefault();
        saveAndCompile();
    }
    else if (e.key === 'F8')
    {
        e.preventDefault();
        if (e.shiftKey) diagPrev(); else diagNext();
    }
    else if (e.ctrlKey && e.key === 'ArrowDown')
    {
        e.preventDefault();
        diagNext();
    }
    else if (e.ctrlKey && e.key === 'ArrowUp')
    {
        e.preventDefault();
        diagPrev();
    }
    else if (e.ctrlKey && e.key === 'Home')
    {
        e.preventDefault();
        diagFirst();
    }
    else if (e.ctrlKey && e.key === 'End')
    {
        e.preventDefault();
        diagLast();
    }
});
const resizeHandle = document.getElementById("resize-handle");
const outputEl = document.getElementById("output");

const panelEl = document.getElementById("panel") || outputEl;
if (resizeHandle)
{
    resizeHandle.addEventListener("mousedown", (e) =>
    {
        e.preventDefault();
        const startY = e.clientY;
        const startHeight = panelEl.offsetHeight;

        function onMouseMove(e)
        {
            const delta = startY - e.clientY;  // drag up = bigger panel
            const newHeight = Math.max(50, startHeight + delta);
            panelEl.style.flex = "none";
            panelEl.style.height = newHeight + "px";
        }

        function onMouseUp()
        {
            document.removeEventListener("mousemove", onMouseMove);
            document.removeEventListener("mouseup", onMouseUp);
        }

        document.addEventListener("mousemove", onMouseMove);
        document.addEventListener("mouseup", onMouseUp);
    });
}