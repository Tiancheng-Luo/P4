<!--
    This file is part of P4 

    Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck, 
                             F. Dumortier, J. Llibre, O. Saleta 

    P4 is free software: you can redistribute it and/or modify 
    it under the terms of the GNU Lesser General Public License as published 
    by the Free Software Foundation, either version 3 of the License, or 
    (at your option) any later version. 

    This program is distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
    GNU Lesser General Public License for more details. 

    You should have received a copy of the GNU Lesser General Public License 
    along with this program.  If not, see <http://www.gnu.org/licenses/>. 
-->

# P4 (Polynomial Planar Phase Portraits)

## Description

P4 is a software that is able to draw the planar phase portrait for any polynomial differential system on the Poincaré or Poincaré-Lyapunov compactified planes.

![Main window](help/screenshots/p4_main_window.png)|![Plot window](help/screenshots/p4_plot_poincare.png)
:-------------------------------------------------:|:---------------------------------------------------:

## Installation

Check <a href="https://github.com/oscarsaleta/P4/releases/latest">releases page</a> to see more detailed instructions and download links.

### Linux

0. Check <a href="#dependencies">dependencies</a>,
1. `git clone https://github.com/oscarsaleta/P4.git && cd P4`,
2. `./compile_install_linux` and follow the instructions,
3. Done!

### Windows

1. Download *P4setup.exe* from <a href="https://github.com/oscarsaleta/P4/releases/latest">here</a>,
2. Execute installer and follow the instructions,
3. Done!

## Files

* **binaries**: some development scripts that help creating binaries (this directory doesn't actually contain any binaries, you can find those <a href="https://github.com/oscarsaleta/P4/releases/latest">here</a>).
* **help**: Help files in *.html* format and supporting images.
* **mpir**: copy of <a href="https://github.com/wbhart/mpir">MPIR repository</a>, a fork of GMP (GNU Multiprecision library), for multiprecision integer computations.
* **mpfr**: copy of <a href="https://github.com/BrianGladman/mpfr">MPFR repository</a>, the GNU MPFR library, for multiprecision floating point computations.
* **QtCreator**: files and projects for building P4 using Qt Creator.
* **src-gui**:
    * **lyapunov**: C++ code for numerical computation of Lyapunov constants
    * **lyapunov_mpf**: C++ code for numerical computation of Lyapunov constants (multiprecision version)
    * **p4**: C++/Qt source code for the GUI of P4,
    * **separatrice**: C++ code for numerical computation of separatrices
* **src-mpl**: Maple scripts that drive the computations of P4 (finding singular points, algebraic computation of Lyapunov constants, blow-ups, etc.).

## Dependencies

**Maple must be installed in the computer in order to use P4.**

### Linux

* **Installation with `compile_install_linux` or compilation from source using `qmake`:**
    - Git: must have to work with *git projects*:
        + Debian-based (Debian/Ubuntu/Mint): `sudo apt-get install git`
        + Fedora-based (Fedora/Kokora/Arquetype): `sudo dnf install git`
        + Arch-based (Archlinux/Antergos): `sudo pacman -S git`
        + (See the <a href="https://wiki.archlinux.org/index.php/git">git ArchWiki page</a> for more information and configuration instructions).
	- Qt modules *QtCore*, *QtGui*, *QtWidgets*, *QtPrintSupport* (minimum required version 5.2.1, could work for 4.6.0 but this has not been tested):
        + Debian-based (Debian/Ubuntu/Mint): `sudo apt-get install qt5-default`
        + Fedora-based (Fedora/Kokora/Arquetype): `sudo dnf install qt5*-devel --allowerasing`
        + Arch-based (Archlinux/Antergos): `sudo pacman -S qt5-base`
	- `qmake` (version 3.0 recommended):
        + Debian-based (Debian/Ubuntu/Mint): `sudo apt-get install qt5-qmake`
        + Other distros: already installed if you followed the install instructions for Qt 5.
	- C++ compiler: `g++` (version >= 4.9 recommended, must support the **C++11** standard):
        + Debian-based (Debian/Ubuntu/Mint): `sudo apt-get install gcc g++`
        + Fedora-based (Fedora/Kokora/Arquetype): `sudo dnf group install 'Development Tools'`
        + Arch-based (Archlinux/Antergos): `sudo pacman -S gcc`
    - `pkg-config` (0.28 or greater):
        + Debian-based (Debian/Ubuntu/Mint): `sudo apt-get install pkg-config`
        + Fedora-based (Fedora/Kokora/Arquetype): `sudo dnf install pkgconfig`
        + Arch-based (Archlinux/Antergos): `sudo pacman -S pkg-config`
    - `mpfr` library (version >= 3.1.2):
        + Debian-based (Debian/Ubuntu/Mint): `sudo apt-get install libmpfr-dev`
        + Fedora-based (Fedora/Kokora/Arquetype): `sudo dnf install mpfr-devel`
        + Arch-based (Archlinux/Angergos): `sudo pacman -S mpfr`
* **Compilation from source using Qt Creator:**
    - Dependencies itemized above,
	- Qt Creator IDE:
        + Debian-based (Debian/Ubuntu/Mint): `sudo apt-get install qtcreator`
        + Fedora-based (Fedora/Kokora/Arquetype): `sudo dnf install qt-creator`
        + Arch-based (Archlinux/Antergos): `sudo pacman -S qtcreator`

### Windows

* **Installation using `P4setup.exe`**:
	- Dependencies are taken care of.
* **Compilation from source using Qt Creator:**
	- Qt modules *QtCore*, *QtGui*, *QtWidgets*, *QtPrintSupport* (minimum required version 5.2.1, could work for 4.6.0 but this has not been tested).
        + Download from <a href="https://www.qt.io/">Qt website</a>,
	- C++ compiler: Microsoft Visual C++ (tested with version 14.0).
        + Download from <a href="https://www.visualstudio.com/vs/cplusplus/#downloadvs">Visual Studio Community</a>.

## Manually build P4 from source

### Build using `qmake`

Qt's `qmake` makes it easy to compile the whole project with two commands.

* From the root of the project, run `qmake -r P4.pro` in order to generate the Makefiles.
* From this same directory, run `make` to compile **p4**, **lyapunov** and **separatrice**.
    - The output of the compilation will be found in a new directory named *build*.
    - The `make` command also compiles **mplstrip**, which is a C++ program used to generate Maple scripts inside *src-mpl*.
* **If your Maple version is NOT 2015**: go into *src-mpl* and execute:
    - `make -f MakeTexMaple clean`
    - `make p4.m`. 
    This will generate **p4gcf.m** and **p4.m**, two Maple scripts needed by P4.
* From the root directory of the project, run `make install` to generate a *p4* folder with the layout explained <a href="#binary-tree">below</a>. This directory can be placed anywhere in the system and P4 can be executed using `/path/to/p4/bin/p4`.
* It is advisable that you set the environment variable `P4_DIR=/path/to/p4` to the correct path of the P4 directory.

### Build using Qt Creator

Check <a href="QtCreator/README.md">here</a> for instructions on how to build this project using Qt Creator.

## Binary tree

Check <a href="QtCreator/README.md">here</a> for instructions of how to create a correct file tree for P4 to work after each of its parts has been compiled.

## Contributors

- J.C. Artés (Universitat Autònoma de Barcelona)
- C. Herssens (Hasselt University)
- P. De Maesschalck (Hasselt University)
- F. Dumortier (Hasselt University)
- J. Llibre (Universitat Autònoma de Barcelona)
- O. Saleta (Universitat Autònoma de Barcelona)
