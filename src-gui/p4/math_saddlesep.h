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

class P4WinSphere;

void start_plot_saddle_sep(std::shared_ptr<P4WinSphere> spherewnd, int vfindex);
void cont_plot_saddle_sep(std::shared_ptr<P4WinSphere> spherewnd);
void plot_next_saddle_sep(std::shared_ptr<P4WinSphere> spherewnd, int vfindex);
void select_next_saddle_sep(std::shared_ptr<P4WinSphere> spherewnd);
void plot_all_saddle_sep(std::shared_ptr<P4WinSphere> spherewnd, int vfindex,
                         const std::vector<p4singularity::saddle> &point);
