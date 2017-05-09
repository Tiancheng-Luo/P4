#  This file is part of P4
# 
#  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier,
#                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
# 
#  P4 is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published
#  by the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
# 
#  You should have received a copy of the GNU Lesser General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#!/bin/bash

VERSION="$(awk '/VERSION.*".*"/{print $NF}' ~/git/P4/src-gui/version.h | awk -F'"' '$0=$2')"
DIR=p4_linux_$VERSION/p4
$(rm -r $DIR)

$(mkdir -p $DIR/bin $DIR/help $DIR/sum_tables)
$(chmod 777 $DIR/sum_tables)
#$(ln -s $DIR/sum_tables $DIR/sumtables)

$(cp ~/git/P4/src-mpl/p4*m $DIR/bin/)
$(cp ~/git/P4/src-gui/build-p4-Desktop_Qt_5_5_1_GCC_64bit-Release/p4 $DIR/bin/)
$(cp ~/git/P4/src-gui/build-lyapunov-Desktop_Qt_5_5_1_GCC_64bit-Release/lyapunov $DIR/bin/)
$(cp ~/git/P4/src-gui/build-separatrice-Desktop_Qt_5_5_1_GCC_64bit-Release/separatrice $DIR/bin/)
$(cp ~/git/P4/help/*.* $DIR/help/)
$(cp -r ~/git/P4/help/screenshots/ $DIR/help/)
$(cp ~/git/P4/help/p4_flag.png $DIR/bin/portrait.png)
#$(cp ~/git/P4/help/p4smallicon.png $DIR/bin/)
$(cp ~/git/P4/help/newp4icon.png $DIR/bin/p4smallicon.png)


tar -cvzf p4_linux_v$VERSION.tar.gz $DIR
