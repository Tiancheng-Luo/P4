/*  This file is part of P4
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
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

#ifndef PLOT_TOOLS_H
#define PLOT_TOOLS_H

#include "win_sphere.h"

extern void (*plot_l)(QWinSphere *, double *, double *, int);
extern void (*plot_p)(QWinSphere *, double *, int);

void spherePlotLine(QWinSphere *sp, double *p1, double *p2, int color);
void spherePlotPoint(QWinSphere *sp, double *p, int color);
void spherePrintLine(QWinSphere *sp, double *p1, double *p2, int color);
void spherePrintPoint(QWinSphere *sp, double *p, int color);
bool lineRectangleIntersect(double &x1, double &y1, double &x2, double &y2,
                            double xmin, double xmax, double ymin, double ymax);

#endif // PLOT_TOOLS_H