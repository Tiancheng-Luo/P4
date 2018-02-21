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

#ifndef MATH_ISOCLINES_H
#define MATH_ISOCLINES_H

#define EVAL_ISOCLINES_NONE 0
#define EVAL_ISOCLINES_R2 1
#define EVAL_ISOCLINES_U1 2
#define EVAL_ISOCLINES_U2 3
#define EVAL_ISOCLINES_V1 4
#define EVAL_ISOCLINES_V2 5
#define EVAL_ISOCLINES_FINISHPOINCARE 6
#define EVAL_ISOCLINES_LYP_R2 7
#define EVAL_ISOCLINES_CYL1 8
#define EVAL_ISOCLINES_CYL2 9
#define EVAL_ISOCLINES_CYL3 10
#define EVAL_ISOCLINES_CYL4 11
#define EVAL_ISOCLINES_FINISHLYAPUNOV 12

class P4WinSphere;
struct orbits_points;

extern orbits_points *gLastIsoclinesPoint;

bool evalIsoclinesStart(P4WinSphere *sp, int dashes, int precision, int points);
bool evalIsoclinesContinue(int precision, int points);
bool evalIsoclinesFinish();
bool runTaskIsoclines(int task, int precision, int points);
void draw_isoclines(P4WinSphere *spherewnd, orbits_points *isoc, int color, int dashes);
void deleteLastIsocline(P4WinSphere *sp);

#endif // MATH_ISOCLINES_H
