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

#ifndef MATH_ORBITS_H
#define MATH_ORBITS_H

#include "file_tab.h"
#include "win_sphere.h"

void integrate_poincare_orbit(double p0, double p1, double p2, double *pcoord,
                              double *hhi, int *dashes, int *dir, double h_min,
                              double h_max);

void integrate_lyapunov_orbit(double p0, double p1, double p2, double *pcoord,
                              double *hhi, int *dashes, int *dir, double h_min,
                              double h_max);

void integrateOrbit(QWinSphere *, int);

orbits_points *integrate_orbit(QWinSphere *, double *, double, int, int, int,
                               orbits_points **);

void drawOrbit(QWinSphere *spherewnd, double *pcoord,
               struct orbits_points *points, int color);

bool startOrbit(QWinSphere *sphere, double x, double y, bool R);

void drawOrbits(QWinSphere *spherewnd);

void deleteLastOrbit(QWinSphere *spherewnd);

#endif // MATH_ORBITS_H
