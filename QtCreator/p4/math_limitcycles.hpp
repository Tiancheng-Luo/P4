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

#pragma once

class P4Sphere;

void drawLimitCycle(P4Sphere *spherewnd, double x, double y, double a,
                    double b, double c);
void searchLimitCycle(P4Sphere *spherewnd, double x0, double y0, double x1,
                      double y1, double grid);
void storeLimitCycle(P4Sphere *spherewnd, double x, double y, double a,
                     double b, double c);
void drawLimitCycles(P4Sphere *spherewnd);
void deleteLastLimitCycle(P4Sphere *spherewnd);
