var inputEditor = {};
var ouputEditor = {};
var model = {};

//https://microsoft.github.io/monaco-editor/playground.html?source=v0.36.1#example-extending-language-services-model-markers-example

var on_edited_timer = -1; // setTimeout(myGreeting, 3000);

// Pattern: filename:line:col:   e.g. main.c:17:11:
var OUTPUT_LOCATION_RE = /^([^\s:]+\.c(?:pp)?):(\d+):(\d+):/;

let useServer = false;

function toggleServerMode()
{
    useServer = document.getElementById('serverMode').checked;
    // Optionally recompile if you want an immediate switch
    // OnCompileButton();
}

function httpPost(url, data, callback)
{
    const xhr = new XMLHttpRequest();
    xhr.open('POST', url, true);
    xhr.setRequestHeader('Content-Type', 'text/plain');
    xhr.onreadystatechange = function ()
    {
        if (xhr.readyState === 4)
        {
            if (xhr.status === 200) callback(null, xhr.responseText);
            else callback(new Error('HTTP ' + xhr.status), null);
        }
    };
    xhr.send(data);
}

function httpGet(url, callback)
{
    const xhr = new XMLHttpRequest();
    xhr.open('GET', url, true);
    xhr.onreadystatechange = function ()
    {
        if (xhr.readyState === 4)
        {
            if (xhr.status === 200) callback(null, xhr.responseText);
            else callback(new Error('HTTP ' + xhr.status), null);
        }
    };
    xhr.send();
}

// Sets the output element's HTML and wraps each line in a <div>.
// Lines matching file:line:col: become clickable and jump the editor.
// htmlContent must already be the final HTML (ANSI colors converted, etc.)
function setOutputHtml(htmlContent)
{
    var element = document.getElementById('output');
    if (!element) return;

    var lines = htmlContent.split('\n');
    var result = '';
    var tmp = document.createElement('span');

    for (var i = 0; i < lines.length; i++)
    {
        var lineHtml = lines[i];

        // Extract plain text to test for file:line:col pattern
        tmp.innerHTML = lineHtml;
        var plainText = tmp.textContent || tmp.innerText || '';
        var match = OUTPUT_LOCATION_RE.exec(plainText);

        if (match)
        {
            var lineNum = parseInt(match[2], 10);
            var colNum = parseInt(match[3], 10);
            result += '<div class="output-line output-location"'
                + ' data-line="' + lineNum + '"'
                + ' data-col="' + colNum + '"'
                + ' title="Go to line ' + lineNum + ', col ' + colNum + '">'
                + lineHtml + '</div>';
        } else
        {
            result += '<div class="output-line">' + lineHtml + '</div>';
        }
    }

    element.innerHTML = result;
    element.scrollTop = element.scrollHeight;

    // Single delegated listener - safe to reassign each render
    element.onclick = function (e)
    {
        var div = e.target.closest('.output-location');
        if (!div) return;
        var ln = parseInt(div.getAttribute('data-line'), 10);
        var col = parseInt(div.getAttribute('data-col'), 10);
        if (inputEditor && inputEditor.revealLineInCenter)
        {
            inputEditor.revealLineInCenter(ln);
            inputEditor.setPosition({ lineNumber: ln, column: col });
            inputEditor.focus();
        }
    };
}

function validate(model)
{

    /*we parse the ouput to create markers*/
    let ouput = document.getElementById("output").innerText;
    var ouputlines = ouput.split(/\r?\n|\r|\n/g);

    const markers = [];

    for (var i = 0; i < ouputlines.length; i++)
    {
        /*
         we need to parse this
         c:/main.c:17:11: warning: returning an uninitialized 'x.name' object [-Wanalyzer-maybe-uninitialized]
         17 |   return xxx;
            |          ^~~     
        */
        if (ouputlines[i].indexOf(".c:") > 0)
        {
            let line_start = 0;
            let start_col = 0;
            let end_col = 0;
            let marks = ouputlines[i + 2];
            for (var k = 0; k < marks.length; k++)
            {
                if (marks.charAt(k) == "|")
                {
                    line_start = k;
                }
                if (marks.charAt(k) == "^" || marks.charAt(k) == "~")
                {
                    start_col = k;
                    end_col = k;
                    k++;
                    while (marks.charAt(k) == "~")
                    {
                        end_col++;
                        k++;
                    }
                }
            }

            end_col = end_col - line_start;
            start_col = start_col - line_start;

            var s = ouputlines[i];
            var c1 = s.indexOf(":");           // end of filename
            var c2 = s.indexOf(":", c1 + 1);   // end of line number
            var c3 = s.indexOf(":", c2 + 1);   // end of col number
            var c4 = s.indexOf(":", c3 + 1);   // end of severity word

            if (c1 >= 0 && c2 > c1 && c3 > c2 && c4 > c3)
            {
                const line = parseInt(s.substring(c1 + 1, c2), 10);
                const severity_str = s.substring(c3 + 1, c4).trim();
                const message = s.substring(c4 + 1).trim();

                var severity = monaco.MarkerSeverity.Info;
                if (severity_str.startsWith("warning"))
                    severity = monaco.MarkerSeverity.Warning;
                else if (severity_str.startsWith("error"))
                    severity = monaco.MarkerSeverity.Error;

                markers.push({
                    message: severity_str + ": " + message,
                    severity: severity,
                    startLineNumber: line,
                    startColumn: start_col,
                    endLineNumber: line,
                    endColumn: end_col + 1
                });
            }
        }
    }

    monaco.editor.setModelMarkers(model, "owner", markers);
}

function ReportIssue()
{
    var source = inputEditor.getValue();
    var generates = outputEditor.getValue();

    var to = -2;// document.getElementById("outtype").value;
    var options = document.getElementById("options").value;

    var title = "unexpected result";
    var textbeforecode = "I believe the following code is not producing the expected result or diagnostics.\n";

    var generatesText = "Is generating:\n";

    var link = "https://github.com/thradams/cake/issues/new?title=" + encodeURIComponent(title) +
        "&body=" + encodeURIComponent(textbeforecode + "\n```c\n" + source + "\n```\n");
    "&body=" + encodeURIComponent(textbeforecode + "\n```c\n" + source + "\n```\n") +
        encodeURIComponent(generatesText + "\n```c\n" + generates + "\n```\n");


    window.open(link, '_blank');
}
function Share()
{
    var sharelink = document.getElementById("sharelink");

    // Toggle off if already visible
    if (sharelink.style.display === "block")
    {
        sharelink.style.display = "none";
        return;
    }

    var source = inputEditor.getValue();
    var to = -2;// document.getElementById("outtype").value;
    var options = document.getElementById("options").value;

    var link = "http://cakecc.org/playground.html?code=" + encodeURIComponent(btoa(source)) +
        "&to=" + encodeURI(to) +
        "&options=" + encodeURI(options);

    sharelink.value = link;
    sharelink.style.display = "block";
    sharelink.select();

    // Hide when it loses focus
    sharelink.onblur = function ()
    {
        sharelink.style.display = "none";
    };
}

function OnSwap()
{
    var temp = inputEditor.getValue();
    inputEditor.setValue(outputEditor.getValue());
    outputEditor.setValue(temp);
}

function Run()
{
    var element = document.getElementById('output');
    if (element) element.innerHTML = "";

    var http = new XMLHttpRequest();
    http.open("POST", "https://coliru.stacked-crooked.com/compile", false);
    http.send(JSON.stringify({ "cmd": "gcc  -std=c89 -x c main.cpp && ./a.out", "src": outputEditor.getValue() }));
    var text = http.response;
    if (text.length == 0)
        text = "coliru compilation ok";
    text = "\x1b[37m" + text;
    var convert = new Filter();
    // toHtml produces colored HTML; pass it directly to setOutputHtml
    var coloredHtml = convert.toHtml(text);
    setOutputHtml(coloredHtml);
}
function OnCompileButton()
{
    var element = document.getElementById('output');
    if (element) element.innerHTML = "";
    outputEditor.setValue("");

    var options = document.getElementById("options").value;
    var target = document.getElementById("target").value;
    options += " -target=" + target;

    var source = inputEditor.getValue();

    if (useServer)
    {
        httpPost('/build', options + "\n" + source, function (err, res)
        {
            if (err)
            {
                setOutputHtml("Build error: " + err.message);
                return;
            }

            var data;
            try { data = JSON.parse(res); }
            catch (e) { setOutputHtml("Bad response: " + res); return; }

            // diagnostics → output panel (with ANSI colour)
            var convert = new Filter();
            var coloredHtml = convert.toHtml(data.output || "");
            setOutputHtml(coloredHtml);

            // transformed source → output editor (right panel)
            console.log("build code length:", (data.code || "").length, "output length:", (data.output || "").length);
            outputEditor.setValue(data.code || "");

            if (model) validate(model);
        });
    }
    else
    {
        // Original client‑side mode (WebAssembly)
        var outputLanguage = -2;
        if (outputLanguage == 0) options += " -E";
        var ot = CompileText(options, source);
        var rawHtml = element ? element.innerHTML : "";
        if (model) validate(model);
        setOutputHtml(rawHtml);
        outputEditor.setValue(ot);
    }
}


function OnChangeSelectionSample(index)
{
    var area = document.getElementById("samples").value;
    var source = sample[area][index];
    inputEditor.setValue(source, -1);
    outputEditor.setValue("");
}

function OnChangeSelection(index)
{
    var samples = sample[index];
    var se2 = document.getElementById("sample");
    se2.innerText = "";

    for (var i in samples)
    {
        var o = document.createElement('option');
        o.textContent = i;
        o.value = i;
        se2.appendChild(o);
    }
    OnChangeSelectionSample(Object.keys(sample[index])[0]);
}

function OnTimerAfterChanges()
{
    clearTimeout(on_edited_timer);
    on_edited_timer = -1;
    OnCompileButton();
}

function OnLoad()
{

    var se = document.getElementById("samples");

    for (var area in sample)
    {
        var o = document.createElement('option');
        o.textContent = area;
        o.value = area;
        se.appendChild(o);
    }

    const containerOne2 = document.getElementById('containerOne2');
    const div1 = document.getElementById('div11');
    const div2 = document.getElementById('div22');
    const splitterBar1 = new SplitterBar(containerOne2, div1, div2, true);

    const containerOne = document.getElementById('containerOne');

    const div11 = document.getElementById('containerOne2');
    const div22 = document.getElementById('div2');
    const splitterBar2 = new SplitterBar(containerOne, div11, div22, false);

    const uri = monaco.Uri.parse("inmemory://test");
    model = monaco.editor.createModel("", "c", uri);

    monaco.editor.defineTheme("myCTheme", {
        base: "vs",  // or "vs" for light
        inherit: true,    // inherit existing rules
        rules: [
            { token: "keyword.special", foreground: "008000" },   // your special keywords    
            { token: "comment.lint", foreground: "FFAA00", fontStyle: "bold" }
        ],
        colors: {
        }
    });

    monaco.editor.setTheme("myCTheme");

    inputEditor = monaco.editor.create(document.getElementById('in'), {
        model,
        language: 'c',
        automaticLayout: true,
        minimap: { enabled: false }
    });

    //validate(model);
    inputEditor.setValue("#include <stdio.h>\n" +
        "\n" +
        "int main()\n" +
        "{\n" +
        "    printf(\"Hello World\"); \n" +
        "    return 0;\n" +
        "}\n");


    clearTimeout(on_edited_timer);
    on_edited_timer = setTimeout(OnTimerAfterChanges, 500);

    model.onDidChangeContent(() =>
    {
        //https://microsoft.github.io/monaco-editor/typedoc/interfaces/editor.ITextModel.html#setValue.setValue-1
        //var source = inputEditor.getValue();
        //model.setValue(source);
        //validate(model);
        clearTimeout(on_edited_timer);
        on_edited_timer = setTimeout(OnTimerAfterChanges, 500);
    });


    outputEditor = monaco.editor.create(document.getElementById('out'), {
        language: 'c',
        automaticLayout: true,
        minimap: { enabled: false }
    });

    const urlParams = new URLSearchParams(window.location.search);

    var to = urlParams.get('to');
    if (to)
    {
        try
        {
            to = decodeURI(to);
            //document.getElementById("outtype").value = to;
        }
        catch { }
    }

    var options = urlParams.get('options');
    if (options)
    {
        try
        {
            options = decodeURI(options);
            document.getElementById("options").value = options;
        } catch { }
    }

    var code = urlParams.get('code');
    if (code)
    {
        code = atob(decodeURIComponent(code));
        inputEditor.setValue(code);
        //Not ready to call
        //OnCompileButton();
    }
}