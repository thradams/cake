# Cake IDE — User Manual

## 1. Introduction

Cake IDE (`cake_ide`) is a lightweight, self-contained editor and debugger built around the Cake compiler. It runs on Windows, Linux, and macOS, and provides file/folder/project editing, one-key compiling, and integrated source-level debugging without requiring a separate IDE or debugger install.

---

## 2. Getting Started

### 2.1 Playground

The **Playground** (View > Playground) is a standing scratch file — always the same file (`playground.c`, stored in the IDE's own config directory, independent of any open project), created with a small Hello World the first time the IDE runs. It's the fastest way to try something small: open it, edit it, Build (F7), Debug (F5) — no project setup required. Closing the Playground window doesn't delete the file; reopening it always reads the latest saved contents from disk.

### 2.2 Opening files, folders, and projects

- **File > Open...** opens a single file.
- **File > Open Folder...** opens a folder as a lightweight file tree, without requiring a project file.
- **File > New Project... / Open Project...** creates or opens a `.cakeproj` project — a file list plus include directories, persisted to disk. A project's **Build** (Project menu) gathers all its `.c` files and hands them to the compiler in one invocation; multi-file linking is the underlying compiler's job, not reimplemented by the IDE.

### 2.3 Building

**Build > Build** (F7) compiles the active file (or, with a project open, the whole project) with Cake. **Build > Show Generated Code** shows the C89-compatible output Cake produced. **Build > Config File** and **Build > Options...** control compiler flags and the active `cakeconf.h`.

---

## 3. Editor

- Standard editing: Undo/Redo, Cut/Copy/Paste, Find/Replace/Find in Files, Go to Line, Go to Definition (F12).
- **Edit > Format** (Ctrl+Shift+F) reformats the current file.
- **Edit > Word Wrap...** toggles soft wrapping.
- **View > Line Numbers** toggles the gutter's line-number display.
- The gutter also shows breakpoint markers (see Debugging below) and, during a stopped debug session, the current execution line.

---

## 4. Debugging

### 4.1 Backend

The IDE drives a real console debugger behind the scenes and scrapes its plain-text output — there is no separate install step beyond having the right tool on your machine:

- **Windows:** [`cdb.exe`](https://learn.microsoft.com/windows-hardware/drivers/debugger/) ("Debugging Tools for Windows"). If it's missing, install it via the Windows SDK installer (select only the "Debugging Tools for Windows" component) or `winget install Microsoft.WindowsSDK.10.0.22621` (or a similar SDK package). The IDE looks for it under the standard install path (`Windows Kits\10\Debuggers\<x86|x64>\cdb.exe`, bitness-matched to the target executable), falling back to a bare `cdb` resolved off PATH.
- **Linux / macOS:** `lldb`, invoked as a plain console subprocess (not through Python bindings).

### 4.2 Breakpoints

Click a line's gutter (or place the caret on it and press **F9** / Debug > Toggle Breakpoint) to set or clear a breakpoint. Breakpoints are stored per file and persist across debug sessions until explicitly toggled off — they are not cleared automatically when a session ends.

### 4.3 Running a session

- **F5** is dual-purpose, matching Visual Studio's convention: **Start Debugging** when idle, **Continue** once a session is already stopped at a breakpoint (the two menu items are never enabled at the same time, so F5 always does the right thing).
- **Shift+F5** — Stop Debugging, ends the session immediately.
- **F10** — Step Over.
- **F11** — Step Into.

The Debug menu's Continue/Step Over/Step Into items (and their shortcuts) are only enabled while a session is actually stopped at a line; they're disabled while running and while idle.

### 4.4 Locals and Call Stack

**View > Debug Info** opens the Locals/Call Stack panel, refreshed automatically every time execution stops at a new line.

### 4.5 Known limitations (Windows/cdb backend)

- A breakpoint on a function's very first statement can occasionally fail to resolve silently, depending on how the compiler's line table merges the prologue and first statement — a breakpoint one line earlier (on the function's opening brace) always works reliably. If a breakpoint doesn't seem to fire, try moving it one line up.
- Stepping into a call whose target has no debug info (e.g. deep CRT internals reached through a local `printf`) may take a moment to resolve back to a source line as the debugger walks out of unsymbolized code automatically.

---

## 5. Menu Reference

| Menu | Notable items |
|---|---|
| **File** | New, Open..., Open Folder..., Save, Save As..., Save all, New/Open Project..., Exit |
| **Edit** | Undo/Redo, Cut/Copy/Paste, Stringify, To Upper/Lower, Word Wrap..., Format, Read-only toggle |
| **View** | Output, Folder, Project, Playground, Debug Info, Line Numbers |
| **Search** | Find..., Replace..., Search Next, Go to line..., Go to Definition, Find in Files... |
| **Project** | Add Existing File..., Include Directories..., Options..., Build, Close Project |
| **Build** | Build, Show Generated Code, Config File, Options... |
| **Debug** | Start Debugging, Stop Debugging, Continue, Step Over, Step Into, Toggle Breakpoint |
| **Tools** | Terminal, plus any configured External Tools |
| **Window** | Tile, Cascade, Close all, Environment..., Font size |
| **Help** | Index, Check, About... |

---

## 6. Keyboard Shortcuts

| Shortcut | Action |
|---|---|
| Ctrl+O | Open... |
| Ctrl+S | Save |
| Ctrl+Shift+S | Save all |
| Ctrl+Z / Ctrl+Y | Undo / Redo |
| Ctrl+X / Ctrl+C / Ctrl+V | Cut / Copy / Paste |
| Ctrl+U / Ctrl+L | To Upper / To Lower |
| Ctrl+W | Word Wrap... |
| Ctrl+Shift+F | Format |
| Ctrl+R | Replace... |
| F3 | Search Next |
| Ctrl+G | Go to line... |
| F12 | Go to Definition |
| Ctrl+F | Find in Files... |
| F7 | Build |
| F5 | Start Debugging / Continue |
| Shift+F5 | Stop Debugging |
| F10 | Step Over |
| F11 | Step Into |
| F9 | Toggle Breakpoint |
| Ctrl++ / Ctrl+- | Increase / decrease font size |
| F1 | Help |
| Shift+F2 / Shift+F3 | Navigate Back / Forward |

---

## 7. Troubleshooting

**"Could not start debugging: ... cdb itself failing to launch"** — `cdb.exe` isn't installed or isn't on PATH. Install the Windows SDK's "Debugging Tools for Windows" component (see §4.1).

**Breakpoint doesn't stop the program** — check the gutter for stray breakpoints left over from an earlier session on unrelated lines; also see the known limitation on first-statement breakpoints in §4.5.

**Session seems stuck after the program finishes** — should end automatically the instant the debuggee exits; if it doesn't, Debug > Stop Debugging (Shift+F5) ends it manually.
