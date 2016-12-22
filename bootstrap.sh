#!/bin/bash
set -e

echo "======== COMPILATION AND INSTALLATION OF P4 ========"

echo "=== Checking for older P4 versions..."
if [ -d $HOME/p4 ]; then
    if whiptail --title "Uninstalling older versions" --yesno "Older P4 install found in $HOME/p4. Do you want to remove it?" 20 60
    then
        rm -rf $HOME/p4
        if test -f $HOME/.local/share/applications/p4.desktop
        then
            rm -f $HOME/.local/share/applications/p4.desktop
        fi
    fi
fi
if [ -d /usr/local/p4 ]; then
    if whiptail --title "Uninstalling older versions" --yesno "Older P4 install found in /usr/local/p4. Do you want to remove it?" 20 60
    then
        sudo rm -rf /usr/local/p4
        if test -f /usr/share/applications/p4.desktop
        then
            sudo rm -f /usr/share/applications/p4.desktop
        fi
        if test -f /usr/local/bin/p4
        then
            sudo rm -f /usr/local/bin/p4
        fi
    fi
else
    echo "No older P4 version found."
fi
if [ -d $HOME/.config/P4 ]; then
    if whiptail --title "Uninstalling older versions" --yesno --defaultno "Do you want to remove P4 configuration?" 20 60; then
        if test -d $HOME/.config/P4; then
            rm -rf $HOME/.config/P4
        fi
    fi
fi
echo "Done."

echo "=== Checking for dependencies..."
if (pkg-config --exists Qt5Core && pkg-config --exists Qt5Gui && pkg-config --exists Qt5Widgets && pkg-config --exists Qt5PrintSupport); then
    echo "Qt5Core, Qt5Gui, Qt5Widgets and Qt5PrintSupport found in the system."
else
    whiptail --title "Missing Qt5 modules" --msgbox "Some Qt5 modules were not found in the system. Install them using the proper method for your distribution, e.g.:\n\n - Debian-based (Debian/Ubuntu/Mint): sudo apt install qt5-default qt5-qmake\n - Fedora-based (Fedora/Kokora/Arquetype): sudo dnf install qt5*-devel --allowerasing\n - Arch-based (Archlinux/Antergos): sudo pacman -S qt5-base" 20 80
    echo "Missing Qt modules which are needed for compiling the source code."
    exit 1
fi
if (ldconfig -p | grep libstdc++ >/dev/null && ldconfig -p | grep libgcc_s >/dev/null && ldconfig -p | grep libc >/dev/null); then
    echo "GCC libraries found in the system."
else
    whiptail --title "Missing GCC libraries" --msgbox "Some GCC libraries were not found in the system. Install them using the proper method for your distribution, e.g.:\n\n - Debian-based (Debian/Ubuntu/Mint): sudo apt install gcc g++\n - Fedora-based (Fedora/Kokora/Arquetype): sudo dnf group install 'Development Tools'\n - Arch-based (Archlinux/Antergos): sudo pacman -S gcc" 20 80
    echo "Missing gcc libraries which are needed for compiling the source code."
    exit 1
fi
echo "Done."

echo "=== Compiling P4..."
whiptail --infobox --title "Compiling P4..." "Compiling P4, be patient!" 20 60
echo "Cleaning old builds..."
if [ -z ${VERBOSE+x} ]; then
    OUT=/dev/null
    ERR=/dev/null
else
    OUT=/dev/stdout
    ERR=/dev/stderr
fi
rm -rf build p4 $DEST
if ! make distclean >$OUT 2>$ERR; then
    echo "Nothing to clean."
fi
echo "Running qmake..."
if [ -z ${QMAKE+x} ]; then
    QMAKE=qmake
fi
if ! $QMAKE -r P4.pro >$OUT 2>$ERR; then
    whiptail --title "qmake error" --msgbox "Error, $QMAKE is not a valid command for building a Qt application. Possible causes are that Qt5 is not properly installed:\n\n - Debian-based (Debian/Ubuntu/Mint): sudo apt install qt5-default qt5-qmake\n - Fedora-based (Fedora/Kokora/Arquetype): sudo dnf install qt5*-devel --allowerasing\n - Arch-based (Archlinux/Antergos): sudo pacman -S qt5-base\n\nor that your Linux distribution uses a different name for the qmake executable (e.g. in Fedora-based distributions, it is called qmake-qt5. In this case, run\n\nenv QMAKE=/path/to/qmake ./bootstrap.sh\n\n(where /path/to/qmake should be replaced by the actual path or command of qmake in your system." 20 80
    echo "Error, check that $QMAKE is a valid command."
    exit 1
fi

CPUCNT=$(grep -c ^processor /proc/cpuinfo)
echo "Compiling... Will use $CPUCNT jobs for make"
make -j$CPUCNT >$OUT 2>$ERR
make install >$OUT 2>$ERR
echo "Done."

echo "=== Installing P4..."
if whiptail --title "Install P4" --yesno --yes-button "User" --no-button "Root" "Where do you want to install P4?\n\nUser: $HOME/p4\nRoot: /usr/local/p4" 20 60; then
    mv p4 $HOME/
    INSTALLDIR=$HOME/p4
    ln -s $INSTALLDIR/sumtables $INSTALLDIR/sum_tables
    if whiptail --title "Create shortcut?" --yesno "Do you want to create an alias for executing P4 from the terminal?\nIf not, you will be able to execute it by typing\n\n ~/p4/bin/p4\n\nin the command line." 20 60; then
        if ! grep $HOME/.profile -e "P4_DIR" >/dev/null; then
            echo "P4_DIR=$HOME/p4/" >> $HOME/.profile
            echo "export P4_DIR" >> $HOME/.profile
            echo "export PATH=$HOME/p4/bin:$PATH" >> $HOME/.profile
            source $HOME/.profile
        fi
    fi
else
    sudo mv p4 /usr/local
    INSTALLDIR=/usr/local/p4
    sudo ln -s $INSTALLDIR/sumtables $INSTALLDIR/sum_tables
    sudo ln -s $INSTALLDIR/bin/p4 /usr/local/bin/p4
fi
echo "Done."

echo "=== Finished."
echo "Relogging might be necessary for the PATH variable to refresh."

#if ($INSTALLED)
#then
#    echo "=== Creating application entry..."
#    TMPDIR=/tmp/p4install
#    mkdir -p $TMPDIR
#    APPNAME=$HOME/.local/share/applications/p4.desktop
#    TMPAPPNAME=$TMPDIR/p4.desktop
#    VERSION=$(cat src-gui/version.h | grep -we VERSION | awk '{gsub("#define VERSION \"","\"");gsub("-.*,","");print}' | awk -F'"' '$0=$2')
#    echo "[Desktop Entry]" > $TMPAPPNAME
#    echo "Name=P4" >> $TMPAPPNAME
#    echo "Version="$VERSION >> $TMPAPPNAME
#    echo "Comment=Polynomial Planar Phase Portraits" >> $TMPAPPNAME
#    echo "Icon=$INSTALLDIR/bin/p4smallicon.png" >> $TMPAPPNAME
#    echo "Keywords=Math" >> $TMPAPPNAME
#    echo "Exec=env P4_DIR=$INSTALLDIR $INSTALLDIR/bin/p4" >> $TMPAPPNAME
#    echo "Path=$HOME" >> $TMPAPPNAME
#    echo "Terminal=true" >> $TMPAPPNAME
#    echo "Type=Application" >> $TMPAPPNAME
#    echo "Categories=Education;Science;Math" >> $TMPAPPNAME
#    echo "StartupNotify=true" >> $TMPAPPNAME
#    mv $TMPAPPNAME $APPNAME
#    chmod 664 $APPNAME
    # remove temporary files
#    rm -r $TMPDIR
#    echo "=== Creating application entry... Done."
#fi
