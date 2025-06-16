## AntTweakBar Library v1.16b

[AntTweakBar](https://anttweakbar.sourceforge.io/doc) (**ATB**) is a small and easy-to-use C/C++ library developed by [Philippe Decaudin](https://phildec.users.sourceforge.net/).  
**ATB** allows programmers to quickly add a lightweight and intuitive GUI to OpenGL- and DirectX-based graphics programs, enabling real-time parameter tweaking.  
Representative screenshots can be found, for example, on the [magnoom](https://github.com/n-s-kiselev/magnoom) page.

This package contains the **development version** of AntTweakBar for **GNU/Linux** and **macOS**.

> 🔧 **Update:**  
> I only fixed a few typos in the [development version](https://anttweakbar.sourceforge.io/doc/tools_anttweakbar_download.html) and added `install` and `uninstall` targets to the Makefiles for my personal convenience.

---

### Building and installing system-wide:

On Linux:
```bash
cd src
make clean
make
sudo make install
````

On MacOS:
```bash
cd src
make -f Makefile.os clean
make -f Makefile.os
sudo make -f Makefile.os install
```

###  Uninstalling

On Linux:
```bash
sudo make uninstall
```

On MacOS:
```bash
sudo make -f Makefile.os uninstall
```

Have fun!

---
> ⚠️ **Note:**
> This version of AntTweakBar only supports **legacy versions** of GLFW and SDL (not compatible with GLFW 3 or SDL 2).
> I’m working on an updated version with modern backend support: [AntTweakBarGLFW3 (WIP)](https://github.com/n-s-kiselev/AntTweakBarGLFW3)
---