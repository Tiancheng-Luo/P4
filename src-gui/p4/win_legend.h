/*  This file is part of P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
 *                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
 *
 *  P4 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WIN_LEGEND_H
#define WIN_LEGEND_H

#include <QBoxLayout>
#include <QPaintEvent>
#include <QWidget>

class QLegendWnd : public QWidget
{
    Q_OBJECT

  public:
    QLegendWnd();
    void paintEvent(QPaintEvent *);
    void calculateGeometry(void);

  private:
    QBoxLayout *mainLayout_;

    int orbitwidth;
    int sepwidth;

    int hmargin1;
    int hmargin2;
    int hmargin3;
    int hmargin4;
    int hmargin5;
    int hmargin6;
    int interline;
    int xheight;

    int vmargin1;
    int vmargin2;
    int vmargin25;
    int vmargin3;
    int vmargin4;
    int vmargin5;
    int vmargin6;
    int vmargin7;
    int vmargin8;
};

#endif /* WIN_LEGEND_H */
