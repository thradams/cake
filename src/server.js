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
    document.getElementById("status-bar")?.classList.remove("error");
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
    el.style.cssText = "opacity:1 !important; color:#ffffff; font-weight:600; transition:none;";
    document.getElementById("status-bar")?.classList.add("error");
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
        setStatus("Loading files...");
        const path = document.getElementById("pathInput").value;
        currentPath = path;

        const res = await fetch(`${API}/list?path=${encodeURIComponent(path)}`);
        if (!res.ok) throw new Error(`Server ${res.status}: ${res.statusText}`);
        const files = await res.json();

        const select = document.getElementById("fileList");
        const saveBtn = document.getElementById("saveBtn");
        const reloadBtn = document.getElementById("reloadBtn");

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

        setStatus("Opening " + file + "...");
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

async function reload()
{
    try
    {
        setStatus("Reloading...");
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
        setStatus("Loading " + file + "...");
        const readRes = await fetch(
            `${API}/read?path=${encodeURIComponent(currentPath)}&file=${encodeURIComponent(file)}`
        );
        if (!readRes.ok) throw new Error(`Read ${file}: server ${readRes.status}`);
        const text = await readRes.text();

        document.getElementById("c-editor").value = text;
        isDirty = false;

        // 2. Compile the file as-is on disk
        setStatus("Compiling " + file + "...");
        const compRes = await fetch(`${API}/compile`, {
            method: "POST",
            headers: { "Content-Type": "text/plain" },
            body: file
        });
        if (!compRes.ok) throw new Error(`Compile ${file}: server ${compRes.status}`);
        const output = await compRes.text();

        setDiagMessages(parseCompilerLines(output));
        document.getElementById("output").innerHTML = renderOutput(output);
        setStatus("\u2713 Ready", 3000);
        if (typeof autoCollapsePanel === "function") autoCollapsePanel();
    }
    catch (err)
    {
        setError(err.message);
    }
}

async function saveFile()
{
    if (!currentFile) return;
    try
    {
        setStatus("Saving...");
        const content = document.getElementById("c-editor").value;

        const res = await fetch(`${API}/save`, {
            method: "POST",
            headers: { "Content-Type": "text/plain" },
            body: currentFile + "\n" + content
        });
        if (!res.ok) throw new Error(`Server ${res.status}: ${res.statusText}`);

        isDirty = false;
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
        setStatus("Saving & compiling...");
        const content = document.getElementById("c-editor").value;

        const res = await fetch(`${API}/savecompile`, {
            method: "POST",
            headers: { "Content-Type": "text/plain" },
            body: currentFile + "\n" + content
        });
        if (!res.ok) throw new Error(`Server ${res.status}: ${res.statusText}`);

        const output = await res.text();
        isDirty = false;
        if (typeof setTabDirty === "function") setTabDirty(currentFile, false);
        setDiagMessages(parseCompilerLines(output));
        document.getElementById("output").innerHTML = renderOutput(output);
        if (typeof autoCollapsePanel === "function") autoCollapsePanel();
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
        setStatus("Compiling...");
        const res = await fetch(`${API}/compile`, {
            method: "POST",
            headers: { "Content-Type": "text/plain" },
            body: currentFile
        });
        if (!res.ok) throw new Error(`Server ${res.status}: ${res.statusText}`);

        const output = await res.text();
        isDirty = false;
        setDiagMessages(parseCompilerLines(output));
        document.getElementById("output").innerHTML = renderOutput(output);
        if (typeof autoCollapsePanel === "function") autoCollapsePanel();
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
        const severity = m[2].toLowerCase();
        const warnId = m[3] ? m[3] : '';
        const message = m[4].trim();

        if (!lineNumber || !message) continue;

        const key = `${lineNumber}|${severity}|${message}`;
        if (seen.has(key)) continue;
        seen.add(key);

        const displayText = warnId ? `${warnId}: ${message}` : message;
        result.push({ line: lineNumber, severity, text: displayText });
    }

    return result;
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

// click a diagnostic line in the output panel
function outputDiagClick(el)
{
    const prev = document.querySelector(".output-diag.selected");
    if (prev) prev.classList.remove("selected");
    el.classList.add("selected");

    const targetLine = parseInt(el.dataset.line, 10);
    if (!targetLine || targetLine < 1) return;

    const idx = lastMessages.findIndex(m => m.line === targetLine);
    if (idx !== -1)
    {
        diagIndex = idx;
        updateDiagCounter();
    }

    jumpToLine(targetLine);
}

// Auto-load on page start
window.addEventListener("load", loadFiles);

// Ctrl+S → save & compile; F8 / Shift+F8 / Ctrl+↑↓ → diag navigation
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
});