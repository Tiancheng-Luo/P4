# P4 (Polynomial Planar Phase Portraits)

> Check <a href="https://github.com/oscarsaleta/P4/releases">the release page</a> to download the latest version of the compiled binaries for your system.

## Description

P4 is a software that is able to draw the planar phase portrait for any polynomial differential system on the Poincaré or Poincaré-Lyapunov compactified planes.

<img src="help/screenshots/p4_main_window.png" alt="file not found" height="430px"/>
<img src="help/screenshots/p4_plot_poincare.png" alt="file not found" height="430px" align="right" />

## Dependencies

**Maple must be installed in the computer in order to use P4.**

### Linux

* **Installation using `installP4.sh` and `installP4_root.sh` scripts:**
	- Qt (modules: *QtCore*, *QtGui*, *QtWidgets*, *QtPrintSupport*), minimum required version 5.5.1.
* **Compilation from source using `configure` and `make`:**
	- Qt (modules: *QtCore*, *QtGui*, *QtWidgets*, *QtPrintSupport*), minimum required version 5.2.1 (could work for 4.6.0 but this has not been tested),
	- C++ compiler: `g++`,
	- Automake tools (optional): `autoconf`, `automake`,
* **Compilation from source using Qt Creator:**
	- Qt Creator IDE,
	- Qt (modules: *QtCore*, *QtGui*, *QtWidgets*, *QtPrintSupport*), minimum required version 5.2.1 (could work for 4.6.0 but this has not been tested),
	- C++ compiler: `g++`.
	
### Windows

* **Installation using setup**:
	- Dependencies are taken care of.
* **Compilation from source using Qt Creator:**
	- Qt (modules: *QtCore*, *QtGui*, *QtWidgets*, *QtPrintSupport*), minimum required version 5.2.1 (could work for 4.6.0 but this has not been tested),
	- C++ compiler: Microsoft Visual C++ (tested with version 14.0).

## Files

* **src-gui**:
    * **p4**: C++/Qt source code for the GUI of P4,
    * **lyapunov**: C++ code for numerical computation of Lyapunov constants
    * **separatrice**: C++ code for numerical computation of separatrices
* **src-mpl**: Maple scripts that drive the computations of P4 (finding singular points, algebraic computation of Lyapunov constants, blow-ups, etc.).
* **help**: Help files in *.html* format and supporting images.
* **binaries**: latest compiled version of P4 for both Linux and Windows operative systems (64-bit only).

## Build from source

Generally, only source from *src-gui* needs to be compiled. 

* **src-gui**: See <a href="#dependencies">dependencies</a>.
	- **Linux**: 
		+ **Using Autotools:** follow the usual `./configure`, `make`, `make install` procedure.
	- **Linux** and **Windows**:
		+ **Using Qt Creator:** build the three separate programs (*p4*, *separatrice* and *lyapunov*) using their respective *.pro* Qt configuration files (use a *Release* profile to get optimized executables).
* **src-mpl**: The Maple scripts in this repository are compatible with Maple 2015. Older versions of Maple might still work but have not been tested. If you have Maple 2015, nothing needs to be compiled (since the files are already compiled in the source tree).
	- **Linux** and **Windows**:
		+ **Using Qt Creator**: compile the *mplscript* program using the *mplscript.pro* file and then use the *Makefile* (`make`) to extract the Maple code from the *.tex* files in order to generate *p4.m* and *p4gcf.m*.

## Binary tree

The executable files have to be put in the following tree somewhere in the file system following this structure (in Linux, `make install` and the scripts `installP4.sh` and `installP4_root.sh` do this automatically; in Windows, the installer also does this automatically):

* **Linux**: in a folder named p4:
    * p4/bin: *p4*, *lyapunov*, *separatrice*, *p4.m*, *p4gcf.m*, *p4smallicon.png*, *portrait.png*,
    * p4/help: contents of **P4/help** folder,
    * p4/sum_tables: empty folder used by P4 to store some files.
* **Windows**: in a folder named p4:
    * p4\bin: *p4.exe*, *lyapunov.exe*, *separatrice.exe*, *p4.m*, *p4gcf.m*, *p4smallicon.png*, *portrait.png*,
    * p4\help: contents of **P4\help** folder,
    * p4\sum_tables: empty folder used by P4 to store some files.

## Contributors

- J.C. Artés (Universitat Autònoma de Barcelona)
- C. Herssens (Hasselt University)
- P. De Maesschalck (Hasselt University)
- F. Dumortier (Hasselt University)
- J. Llibre (Universitat Autònoma de Barcelona)
- O. Saleta (Universitat Autònoma de Barcelona)