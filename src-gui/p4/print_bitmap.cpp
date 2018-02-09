/*  This file is part of P4
 *
 *  Copyright (C) 1996-2018  J.C. Artés, P. De Maesschalck, F. Dumortier
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

#include "print_bitmap.h"

#include "custom.h"
#include "main.h"
#include "plot_tools.h"
#include "print_points.h"

#include <QBrush>
#include <QPainter>

static bool s_P4PrintBlackWhite = true;

static std::unique_ptr<QPainter> s_P4PrintPainter{};

static int s_P4PrintLineWidth = 0;
static int s_P4PrintSymbolWidth = 0;
static int s_LastP4PrintX0 = 0;
static int s_LastP4PrintY0 = 0;
static int s_LastP4Printcolor = 0;

// ---------------------------------------------------------------------------------------
int printColorTable(int color)
{
    int colorTable[NUMXFIGCOLORS] = {
        WHITE,  // black --> white when printing
        BLUE,   GREEN,  CYAN,   RED,   MAGENTA,
        BLACK,  // yellow --> black when printing
        BLACK,  // white --> black when printing
        BLUE1,  BLUE2,  BLUE3,  BLUE4, GREEN1,  GREEN2,   GREEN3,   CYAN1,
        CYAN2,  CYAN3,  RED1,   RED2,  RED3,    MAGENTA1, MAGENTA2, MAGENTA3,
        BROWN1, BROWN2, BROWN3, PINK1, PINK2,   PINK3,    PINK4,    GOLD};
    int colorTableReverse[NUMXFIGCOLORS] = {
        BLACK,  BLUE,   GREEN,  CYAN,  RED,    MAGENTA,  YELLOW,   WHITE,
        BLUE1,  BLUE2,  BLUE3,  BLUE4, GREEN1, GREEN2,   GREEN3,   CYAN1,
        CYAN2,  CYAN3,  RED1,   RED2,  RED3,   MAGENTA1, MAGENTA2, MAGENTA3,
        BROWN1, BROWN2, BROWN3, PINK1, PINK2,  PINK3,    PINK4,    GOLD};

    if (bgColours::PRINT_WHITE_BG)
        return colorTable[color];
    else
        return colorTableReverse[color];
}

static void P4Print_comment(QString s)
{
    // do nothing
}

static void P4Print_print_saddle(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSADDLE);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print box:
    s_P4PrintPainter->drawRect(x - s_P4PrintSymbolWidth / 2,
                               y - s_P4PrintSymbolWidth / 2,
                               s_P4PrintSymbolWidth, s_P4PrintSymbolWidth);
}

static void P4Print_virtualsaddle(double _x, double _y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSADDLE);

    QPen p{QXFIGCOLOR(color), (int)(s_P4PrintLineWidth * 26 / 20)};
    s_P4PrintPainter->setPen(p);
    s_P4PrintPainter->setBrush(Qt::NoBrush);

    // print box:
    s_P4PrintPainter->drawRect(x - s_P4PrintSymbolWidth / 2,
                               y - s_P4PrintSymbolWidth / 2,
                               s_P4PrintSymbolWidth, s_P4PrintSymbolWidth);
}

static void P4Print_print_stablenode(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_S);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print box:
    s_P4PrintPainter->drawRect(x - s_P4PrintSymbolWidth / 2,
                               y - s_P4PrintSymbolWidth / 2,
                               s_P4PrintSymbolWidth, s_P4PrintSymbolWidth);
}

static void P4Print_print_virtualstablenode(double _x, double _y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_S);

    QPen p{QXFIGCOLOR(color), (int)(s_P4PrintLineWidth * 26 / 20)};
    s_P4PrintPainter->setPen(p);
    s_P4PrintPainter->setBrush(Qt::NoBrush);

    // print box:
    s_P4PrintPainter->drawRect(x - s_P4PrintSymbolWidth / 2,
                               y - s_P4PrintSymbolWidth / 2,
                               s_P4PrintSymbolWidth, s_P4PrintSymbolWidth);
}

static void P4Print_print_unstablenode(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_U);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print box:
    s_P4PrintPainter->drawRect(x - s_P4PrintSymbolWidth / 2,
                               y - s_P4PrintSymbolWidth / 2,
                               s_P4PrintSymbolWidth, s_P4PrintSymbolWidth);
}

static void P4Print_print_virtualunstablenode(double _x, double _y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_U);

    QPen p{QXFIGCOLOR(color), (int)(s_P4PrintLineWidth * 26 / 20)};
    s_P4PrintPainter->setPen(p);
    s_P4PrintPainter->setBrush(Qt::NoBrush);

    // print box:
    s_P4PrintPainter->drawRect(x - s_P4PrintSymbolWidth / 2,
                               y - s_P4PrintSymbolWidth / 2,
                               s_P4PrintSymbolWidth, s_P4PrintSymbolWidth);
}

static void P4Print_print_stableweakfocus(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CWEAK_FOCUS_S);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);

    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_virtualstableweakfocus(double _x, double _y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CWEAK_FOCUS_S);

    QPen p{QXFIGCOLOR(color), (int)(s_P4PrintLineWidth * 26 / 20)};
    s_P4PrintPainter->setPen(p);
    s_P4PrintPainter->setBrush(Qt::NoBrush);

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_unstableweakfocus(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CWEAK_FOCUS_U);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_virtualunstableweakfocus(double _x, double _y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CWEAK_FOCUS_U);

    QPen p{QXFIGCOLOR(color), (int)(s_P4PrintLineWidth * 26 / 20)};
    s_P4PrintPainter->setPen(p);
    s_P4PrintPainter->setBrush(Qt::NoBrush);

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_weakfocus(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CWEAK_FOCUS);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_virtualweakfocus(double _x, double _y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CWEAK_FOCUS);

    QPen p{QXFIGCOLOR(color), (int)(s_P4PrintLineWidth * 26 / 20)};
    s_P4PrintPainter->setPen(p);
    s_P4PrintPainter->setBrush(Qt::NoBrush);

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_center(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CCENTER);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_virtualcenter(double _x, double _y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CCENTER);

    QPen p{QXFIGCOLOR(color), (int)(s_P4PrintLineWidth * 26 / 20)};
    s_P4PrintPainter->setPen(p);
    s_P4PrintPainter->setBrush(Qt::NoBrush);

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_stablestrongfocus(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSTRONG_FOCUS_S);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_virtualstablestrongfocus(double _x, double _y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSTRONG_FOCUS_S);

    QPen p{QXFIGCOLOR(color), (int)(s_P4PrintLineWidth * 26 / 20)};
    s_P4PrintPainter->setPen(p);
    s_P4PrintPainter->setBrush(Qt::NoBrush);

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_unstablestrongfocus(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSTRONG_FOCUS_U);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_virtualunstablestrongfocus(double _x, double _y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSTRONG_FOCUS_U);

    QPen p{QXFIGCOLOR(color), (int)(s_P4PrintLineWidth * 26 / 20)};
    s_P4PrintPainter->setPen(p);
    s_P4PrintPainter->setBrush(Qt::NoBrush);

    // print diamond:
    QPolygon qpa(4);
    qpa.setPoints(4, x, y - s_P4PrintSymbolWidth * 13 / 20,
                  x + s_P4PrintSymbolWidth * 13 / 20, y, x,
                  y + s_P4PrintSymbolWidth * 13 / 20,
                  x - s_P4PrintSymbolWidth * 13 / 20, y);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_sesaddle(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSADDLE);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print triangle:
    QPolygon qpa(3);
    qpa.setPoints(3, x - s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20,
                  x + s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20, x,
                  y - s_P4PrintSymbolWidth * 12 / 20);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_virtualsesaddle(double _x, double _y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSADDLE);

    QPen p{QXFIGCOLOR(color), (int)(s_P4PrintLineWidth * 26 / 20)};
    s_P4PrintPainter->setPen(p);
    s_P4PrintPainter->setBrush(Qt::NoBrush);

    // print triangle:
    QPolygon qpa(3);
    qpa.setPoints(3, x - s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20,
                  x + s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20, x,
                  y - s_P4PrintSymbolWidth * 12 / 20);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_sesaddlenode(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSADDLE_NODE);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print triangle:
    QPolygon qpa(3);
    qpa.setPoints(3, x - s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20,
                  x + s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20, x,
                  y - s_P4PrintSymbolWidth * 12 / 20);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_virtualsesaddlenode(double _x, double _y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CSADDLE_NODE);

    QPen p{QXFIGCOLOR(color), (int)(s_P4PrintLineWidth * 26 / 20)};
    s_P4PrintPainter->setPen(p);
    s_P4PrintPainter->setBrush(Qt::NoBrush);

    // print triangle:
    QPolygon qpa(3);
    qpa.setPoints(3, x - s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20,
                  x + s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20, x,
                  y - s_P4PrintSymbolWidth * 12 / 20);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_sestablenode(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_S);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print triangle:
    QPolygon qpa(3);
    qpa.setPoints(3, x - s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20,
                  x + s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20, x,
                  y - s_P4PrintSymbolWidth * 12 / 20);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_virtualsestablenode(double _x, double _y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_S);

    QPen p{QXFIGCOLOR(color), (int)(s_P4PrintLineWidth * 26 / 20)};
    s_P4PrintPainter->setPen(p);
    s_P4PrintPainter->setBrush(Qt::NoBrush);

    // print triangle:
    QPolygon qpa(3);
    qpa.setPoints(3, x - s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20,
                  x + s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20, x,
                  y - s_P4PrintSymbolWidth * 12 / 20);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_seunstablenode(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_U);

    s_P4PrintPainter->setPen(QXFIGCOLOR(color));
    s_P4PrintPainter->setBrush(QXFIGCOLOR(color));

    // print triangle:
    QPolygon qpa(3);
    qpa.setPoints(3, x - s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20,
                  x + s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20, x,
                  y - s_P4PrintSymbolWidth * 12 / 20);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_virtualseunstablenode(double _x, double _y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CNODE_U);

    QPen p{QXFIGCOLOR(color), (int)(s_P4PrintLineWidth * 26 / 20)};
    s_P4PrintPainter->setPen(p);
    s_P4PrintPainter->setBrush(Qt::NoBrush);

    // print triangle:
    QPolygon qpa(3);
    qpa.setPoints(3, x - s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20,
                  x + s_P4PrintSymbolWidth * 12 / 20,
                  y + s_P4PrintSymbolWidth * 12 / 20, x,
                  y - s_P4PrintSymbolWidth * 12 / 20);
    s_P4PrintPainter->drawPolygon(qpa, Qt::OddEvenFill);
}

static void P4Print_print_degen(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CDEGEN);

    QPen p = QPen(QXFIGCOLOR(color), (int)(s_P4PrintLineWidth * 26 / 20));
    s_P4PrintPainter->setPen(p);

    // print cross:
    s_P4PrintPainter->drawLine(
        x - s_P4PrintSymbolWidth / 2, y - s_P4PrintSymbolWidth / 2,
        x + s_P4PrintSymbolWidth / 2, y + s_P4PrintSymbolWidth / 2);
    s_P4PrintPainter->drawLine(
        x + s_P4PrintSymbolWidth / 2, y - s_P4PrintSymbolWidth / 2,
        x - s_P4PrintSymbolWidth / 2, y + s_P4PrintSymbolWidth / 2);
}

static void P4Print_print_virtualdegen(double _x, double _y)
{
    if (!g_VFResults.plotVirtualSingularities_)
        return;

    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CDEGEN);

    QPen p{QXFIGCOLOR(color), (int)(s_P4PrintLineWidth * 26 / 20)};
    s_P4PrintPainter->setPen(p);

    // print cross:
    s_P4PrintPainter->drawLine(
        x - s_P4PrintSymbolWidth / 2, y - s_P4PrintSymbolWidth / 2,
        x + s_P4PrintSymbolWidth / 2, y + s_P4PrintSymbolWidth / 2);
    s_P4PrintPainter->drawLine(
        x + s_P4PrintSymbolWidth / 2, y - s_P4PrintSymbolWidth / 2,
        x - s_P4PrintSymbolWidth / 2, y + s_P4PrintSymbolWidth / 2);
}

static void P4Print_print_coinciding(double _x, double _y)
{
    int x{(int)_x};
    int y{(int)_y};
    int color;

    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);
    else
        color = printColorTable(CDEGEN);

    QPen p = QPen(QXFIGCOLOR(color), (int)P4PrintLineWidth * 26 / 20);
    s_P4PrintPainter->setPen(p);

    // print double cross:
    s_P4PrintPainter->drawLine(
        x - s_P4PrintSymbolWidth / 2, y - s_P4PrintSymbolWidth / 2,
        x + s_P4PrintSymbolWidth / 2, y + s_P4PrintSymbolWidth / 2);
    s_P4PrintPainter->drawLine(
        x + s_P4PrintSymbolWidth / 2, y - s_P4PrintSymbolWidth / 2,
        x - s_P4PrintSymbolWidth / 2, y + s_P4PrintSymbolWidth / 2);
    s_P4PrintPainter->drawLine(x, y - (s_P4PrintSymbolWidth * 3) / 4, x,
                               y + (s_P4PrintSymbolWidth * 3) / 4);
    s_P4PrintPainter->drawLine(x + (s_P4PrintSymbolWidth * 3) / 4, y,
                               x - (s_P4PrintSymbolWidth * 3) / 4, y);
}

static void P4Print_print_elips(double x0, double y0, double a, double b,
                                int color, bool dotted,
                                std::vector<P4POLYLINES> ellipse)
{
    color = printColorTable(color);
    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);

    QPen p = QPen(QXFIGCOLOR(color), s_P4PrintLineWidth);
    p.setCapStyle(Qt::RoundCap);
    s_P4PrintPainter->setPen(p);

    // we do not use the (x0,y0,a,b,dotted) parameters. Instead, we use the
    // "precompiled" ellipse parameter.  Here, a list of lines is computed that
    // approximates the ellipse.

    for (auto it : ellipse) {
        s_P4PrintPainter->drawLine((int)(it.x1), (int)(it.y1), (int)(it.x2),
                                   (int)(it.y2));
    }
}

static void P4Print_print_line(double _x0, double _y0, double _x1, double _y1,
                               int color)
{
    color = printColorTable(color);
    if (s_P4PrintBlackWhite)
        color = printColorTable(bgColours::CFOREGROUND);

    int x0{(int)_x0};
    int x1{(int)_x1};
    int y0{(int)_y0};
    int y1{(int)_y1};

    if (x0 == x1 && y0 == y1)
        return;

    QPen p = QPen(QXFIGCOLOR(color), s_P4PrintLineWidth);
    p.setCapStyle(Qt::RoundCap);

    s_P4PrintPainter->setPen(p);
    s_P4PrintPainter->drawLine(x0, y0, x1, y1);
}

static void P4Print_print_point(double _x0, double _y0, int color)
{
    if (s_P4PrintBlackWhite)
        color = bgColours::CFOREGROUND;
    color = printColorTable(color);

    int x0{(int)_x0};
    int y0{(int)_y0};

    if (x0 == s_LastP4PrintX0 && y0 == s_LastP4PrintY0 &&
        color == s_LastP4Printcolor)
        return;

    s_LastP4PrintX0 = x0;
    s_LastP4PrintY0 = y0;
    s_LastP4Printcolor = color;

    QPen p = QPen(QXFIGCOLOR(color), s_P4PrintLineWidth);
    p.setCapStyle(Qt::RoundCap);

    s_P4PrintPainter->setPen(p);
    if (s_P4PrintLineWidth > 1) {
        s_P4PrintPainter->drawLine(x0, y0, x0, y0);
    } else {
        s_P4PrintPainter->drawPoint(x0, y0);
    }
}

// ---------------------------------------------------------------------------------------

void prepareP4Printing(int w, int h, bool isblackwhite,
                       std::unique_ptr<QPainter> p4paint, int linewidth,
                       int symbolwidth)
{
    QString s;

    s_P4PrintBlackWhite = isblackwhite;
    s_P4PrintPainter = p4paint;
    s_P4PrintLineWidth = linewidth;
    s_P4PrintSymbolWidth = symbolwidth;

    plot_l = spherePrintLine;
    plot_p = spherePrintPoint;

    print_saddle = P4Print_print_saddle;
    print_virtualsaddle = P4Print_print_virtualsaddle;

    print_stablenode = P4Print_print_stablenode;
    print_virtualstablenode = P4Print_print_virtualstablenode;

    print_unstablenode = P4Print_print_unstablenode;
    print_virtualunstablenode = P4Print_print_virtualunstablenode;

    print_stableweakfocus = P4Print_print_stableweakfocus;
    print_virtualstableweakfocus = P4Print_print_virtualstableweakfocus;

    print_unstableweakfocus = P4Print_print_unstableweakfocus;
    print_virtualunstableweakfocus = P4Print_print_virtualunstableweakfocus;

    print_weakfocus = P4Print_print_weakfocus;
    print_virtualweakfocus = P4Print_print_virtualweakfocus;

    print_stablestrongfocus = P4Print_print_stablestrongfocus;
    print_virtualstablestrongfocus = P4Print_print_virtualstablestrongfocus;

    print_unstablestrongfocus = P4Print_print_unstablestrongfocus;
    print_virtualunstablestrongfocus = P4Print_print_virtualunstablestrongfocus;

    print_sesaddle = P4Print_print_sesaddle;
    print_virtualsesaddle = P4Print_print_virtualsesaddle;

    print_sesaddlenode = P4Print_print_sesaddlenode;
    print_virtualsesaddlenode = P4Print_print_virtualsesaddlenode;

    print_sestablenode = P4Print_print_sestablenode;
    print_virtualsestablenode = P4Print_print_virtualsestablenode;

    print_seunstablenode = P4Print_print_seunstablenode;
    print_virtualseunstablenode = P4Print_print_virtualseunstablenode;

    print_degen = P4Print_print_degen;
    print_virtualdegen = P4Print_print_virtualdegen;

    print_center = P4Print_print_center;
    print_virtualcenter = P4Print_print_virtualcenter;

    print_coinciding = P4Print_print_coinciding;
    print_elips = P4Print_print_elips;
    print_point = P4Print_print_point;
    print_line = P4Print_print_line;
    print_comment = P4Print_comment;
    
    s_LastP4Printcolor = -1;

    p4paint->fillRect(
        0, 0, w, h,
        QBrush(QXFIGCOLOR(printColorTable(bgColours::CBACKGROUND))));
}

void finishP4Printing(void)
{
    plot_l = spherePlotLine;
    plot_p = spherePlotPoint;
    
    s_P4PrintPainter.reset();
}
