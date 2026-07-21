### Platform Support

Cake runs on Windows (targeting MSVC), Linux (targeting GCC) and macOs
(targeting Clang). Cake can also perform cross-compilation, as long as you
provide the headers from the target platform.

### Include Directories

Include directories are specified in a `cakeconf.h` configuration header. Cake
first searches for this file relative to the source file being compiled. If no
local `cakeconf.h` is found, the root `cakeconf.h` from the installation
directory is used.


**Discovering system include paths manually:**

On Windows (from a Visual Studio Developer Command Prompt):

```
echo %INCLUDE%
```

On Linux:

```
echo | gcc -E -Wp,-v -
```

**Sample `cakeconf.h` for Linux:**

```c
#ifdef __linux__
#pragma dir "/usr/lib/gcc/x86_64-linux-gnu/11/include"
#pragma dir "/usr/local/include"
#pragma dir "/usr/include/x86_64-linux-gnu"
#pragma dir "/usr/include"
#endif
```

**Sample `cakeconf.h` for Windows:**

```c
#ifdef _WIN32
#pragma dir "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC/14.38.33130/include"
#pragma dir "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC/14.38.33130/ATLMFC/include"
#pragma dir "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Auxiliary/VS/include"
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/ucrt"
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/um"
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/shared"
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/winrt"
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/cppwinrt"
#pragma dir "C:/Program Files (x86)/Windows Kits/NETFXSDK/4.8/include/um"
#endif
```

**Per-project configuration:**

Projects can have their own local `cakeconf.h` that includes a shared
system-level configuration and adds project-specific paths:

`yourproject\cakeconf.h`:

```c
// System includes
#include "C:\Program Files (x86)\cake\cakeconf.h"

// Project-specific includes
#pragma dir ".\openssl\include"
```
### System include paths auto-configuration

The `-auto-config` option generates a root `cakeconf.h` automatically by querying
the active compiler environment:

```
cake -auto-config
```


###  Using cake inside Visual Studio
Use cake as Custom Build Tool for a specific file.c

- Command line: `cake -sarif -sarif-path "$(SolutionDir).sarif" -line-directives -msvc-output file.c`
- Outputs: `x86/file.c`
- Add Output to item type: `C/C++ Compiler`

Then you can run/debug normally.

It also can be used as external tool for static analysis:

- Command: `cake.exe`
- Arguments: `-msvc-output -no-output -sarif -sarif-path "$(SolutionDir).sarif"  $(ItemPath)`
- Initial directory: `$(ItemDir)`
- [x] Use output window



