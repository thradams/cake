
var inputEditor = {};
var ouputEditor = {};
var model = {};

//https://microsoft.github.io/monaco-editor/playground.html?source=v0.36.1#example-extending-language-services-model-markers-example

var on_edited_timer = -1; // setTimeout(myGreeting, 3000);

function validate(model) {

    /*we parse the ouput to create markers*/
    let ouput = document.getElementById("output").innerText;
    var ouputlines = ouput.split(/\r?\n|\r|\n/g);

    const markers = [];

    for (var i = 0; i < ouputlines.length; i++) {
        /*
         we need to parse this
         c:/main.c:17:11: warning: returning an uninitialized 'x.name' object [-Wanalyzer-maybe-uninitialized]
         17 |   return xxx;
            |          ^~~     
        */
        if (ouputlines[i].search("c:/main.c") == 0) {
            let line_start = 0;
            let start_col = 0;
            let end_col = 0;
            let marks = ouputlines[i + 2];
            for (var k = 0; k < marks.length; k++) {
                if (marks.charAt(k) == "|") {
                    line_start = k;
                }
                if (marks.charAt(k) == "^" || marks.charAt(k) == "~") {
                    start_col = k;
                    end_col = k;
                    k++;
                    while (marks.charAt(k) == "~") {
                        end_col++;
                        k++;
                    }
                }
            }

            end_col = end_col - line_start;
            start_col = start_col - line_start;

            var nn = ouputlines[i].split(":");
            let severity_str = nn[4].trim();
            var severity;
            if (severity_str == "warning")
                severity = monaco.MarkerSeverity.Warning;
            else if (severity_str == "error")
                severity = monaco.MarkerSeverity.Error;
            else
                severity = monaco.MarkerSeverity.Info;

            const line = parseInt(nn[2]);
            markers.push({
                message: severity_str + ":" + nn[5],
                severity: severity,
                startLineNumber: line,//range.startLineNumber,
                startColumn: start_col,//range.startColumn,
                endLineNumber: line,//range.endLineNumber,
                endColumn: end_col + 1 //model.getLineLength(line) + 1//range.endColumn,
            });
        }
    }

    monaco.editor.setModelMarkers(model, "owner", markers);
}

function ReportIssue() {
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
function Share() {
    var source = inputEditor.getValue();
    var to = -2;// document.getElementById("outtype").value;
    var options = document.getElementById("options").value;

    var link = "http://cakecc.org/playground.html?code=" + encodeURIComponent(btoa(source)) +
        "&to=" + encodeURI(to) +
        "&options=" + encodeURI(options);

    document.getElementById("sharelink").value = link;
    document.getElementById("sharelink").style.display = "block";
    document.getElementById("sharelink").select();
}

function OnSwap() {
    var temp = inputEditor.getValue();
    inputEditor.setValue(outputEditor.getValue());
    outputEditor.setValue(temp);
}

function Run() {
    document.getElementById("output").value = "";
    var http = new XMLHttpRequest();

    http.open("POST", "https://coliru.stacked-crooked.com/compile", false);
    http.send(JSON.stringify({ "cmd": "gcc  -std=c89 -x c main.cpp && ./a.out", "src": outputEditor.getValue() }));
    var text = http.response;
    if (text.length == 0)
        text = "coliru compilation ok";
    text = "\x1b[37m" + text;
    var element = document.getElementById('output');
    var convert = new Filter();
    text = convert.toHtml(text);
    if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
    if (element) {
        element.innerHTML = text + "\n";
        element.scrollTop = element.scrollHeight; // focus on bottom
    }
}


function OnCompileButton() {
    document.getElementById("output").value = "";
    outputEditor.setValue("");
    var outputLanguage = -2;// document.getElementById("outtype").value;

    var options = document.getElementById("options").value;

    var target = document.getElementById("target").value;
    options += " -target=" + target;

    if (outputLanguage == 0)
        options += " -E";


    var source = inputEditor.getValue();
    var ot = CompileText(options, source);

    if (model) {
        validate(model);
    }
    outputEditor.setValue(ot);
}

function OnChangeSelectionSample(index) {
    var area = document.getElementById("samples").value;
    var source = sample[area][index];
    inputEditor.setValue(source, -1);
    outputEditor.setValue("");
}

function OnChangeSelection(index) {
    var samples = sample[index];
    var se2 = document.getElementById("sample");
    se2.innerText = "";

    for (var i in samples) {
        var o = document.createElement('option');
        o.textContent = i;
        o.value = i;
        se2.appendChild(o);
    }
    OnChangeSelectionSample(Object.keys(sample[index])[0]);
}

function OnTimerAfterChanges() {
    clearTimeout(on_edited_timer);
    on_edited_timer = -1;
    OnCompileButton();
}

function OnLoad() {

    var se = document.getElementById("samples");

    for (var area in sample) {
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

    model.onDidChangeContent(() => {
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
    if (to) {
        try {
            to = decodeURI(to);
            //document.getElementById("outtype").value = to;
        }
        catch { }
    }

    var options = urlParams.get('options');
    if (options) {
        try {
            options = decodeURI(options);
            document.getElementById("options").value = options;
        } catch { }
    }

    var code = urlParams.get('code');
    if (code) {
        code = atob(decodeURIComponent(code));
        inputEditor.setValue(code);
        //Not ready to call
        //OnCompileButton();
    }
}