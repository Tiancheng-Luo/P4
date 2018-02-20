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

#include "math_gcf.h"

#include "P4WinSphere.h"
#include "custom.h"
#include "file_tab.h"
#include "file_vf.h"
#include "math_charts.h"
#include "math_p4.h"
#include "plot_tools.h"

#include <QFile>

#include <cmath>

// static global variables
static int s_GcfTask = EVAL_GCF_NONE;
static std::shared_ptr<P4WinSphere> s_GcfSphere;
static int s_GcfDashes = 0;
static bool s_GcfError = false;
static int s_GcfVfIndex = 0;

// static functions
static void insert_gcf_point(double x0, double y0, double z0, int dashes);
static bool readTaskResults(int task, int index);
static bool read_gcf(void (*chart)(double, double, double *));

// function definitions
bool evalGcfStart(std::smart_ptr<P4WinSphere> sp, int dashes, int points,
                  int precis)
{
    int r;
    sp->prepareDrawing();
    for (r = 0; r < g_ThisVF->numVF_; r++) {
        if (!g_VFResults.vf_[r].gcf_points_.empty()) {
            draw_gcf(sp, g_VFResults.vf_[r].gcf_points_, bgColours::CBACKGROUND,
                     s_GcfDashes);
            g_VFResults.vf_[r].gcf_points_.clear();
        }
    }
    sp->finishDrawing();

    if (g_VFResults.plweights_)
        s_GcfTask = EVAL_GCF_LYP_R2;
    else
        s_GcfTask = EVAL_GCF_R2;

    s_GcfError = false;
    s_GcfSphere = sp;
    s_GcfDashes = dashes;
    return runTask(s_GcfTask, points, precis, memory, 0);
}

// returns true when finished.  Then run EvalGCfFinish to see if error occurred
// or not
bool evalGcfContinue(int points, int prec, int memory)
{
    if (s_GcfTask == EVAL_GCF_NONE)
        return true;

    if (!readTaskResults(s_GcfTask, s_GcfVfIndex)) {
        s_GcfError = true;
        return true;
    }
    s_GcfTask++;
    if (s_GcfTask == EVAL_GCF_FINISHPOINCARE ||
        s_GcfTask == EVAL_GCF_FINISHLYAPUNOV) {
        while (1) {
            if (++s_GcfVfIndex >= g_ThisVF->numVF_)
                break;
            if (!g_VFResults.vf_[s_GcfVfIndex].gcf_.empty())
                break;
        }
        if (s_GcfVfIndex >= g_ThisVF->numVF_)
            // all gcfs are evaluated
            return true;
        // retrigger new set of tasks
        if (g_VFResults.plweights_)
            s_GcfTask = EVAL_GCF_LYP_R2;
        else
            s_GcfTask = EVAL_GCF_R2;
    }

    if (!runTask(s_GcfTask, points, prec)) {
        s_GcfError = true;
        return true;
    }

    return false;  // still busy
}

bool evalGcfFinish(void)  // return false in case an error occured
{
    int index;
    if (s_GcfTask != EVAL_GCF_NONE) {
        for (index = 0; index < g_ThisVF->numVF_; index++)
            g_ThisVF->resampleGcf(index);
        s_GcfSphere->prepareDrawing();
        for (index = 0; index < g_ThisVF->numVF_; index++) {
            if (!g_VFResults.vf_[index].gcf_points_.empty())
                draw_gcf(s_GcfSphere, g_VFResults.vf_[index].gcf_points_, CSING,
                         1);
        }
        s_GcfSphere->finishDrawing();

        s_GcfTask = EVAL_GCF_NONE;

        if (s_GcfError) {
            s_GcfError = false;
            return false;
        }
    }
    return true;
}

bool runTask(int task, int points, int prec, int mem, int index)
{
    bool value;

    while (!g_VFResults.vf_[index].gcf_.empty()) {
        if (++index == g_ThisVF->numVF_)
            return false;
    }
    s_GcfVfIndex = index;

    switch (task) {
    case EVAL_GCF_R2:
        value = g_ThisVF->prepareGcf(g_VFResults.vf_[index].gcf_, -1, 1, prec,
                                     points);
        break;
    case EVAL_GCF_U1:
        value = g_ThisVF->prepareGcf(g_VFResults.vf_[index].gcf_U1_, 0, 1, prec,
                                     points);
        break;
    case EVAL_GCF_V1:
        value = g_ThisVF->prepareGcf(g_VFResults.vf_[index].gcf_U1_, -1, 0,
                                     prec, points);
        break;
    case EVAL_GCF_U2:
        value = g_ThisVF->prepareGcf(g_VFResults.vf_[index].gcf_U2_, 0, 1, prec,
                                     points);
        break;
    case EVAL_GCF_V2:
        value = g_ThisVF->prepareGcf(g_VFResults.vf_[index].gcf_U2_, -1, 0,
                                     prec, points);
        break;
    case EVAL_GCF_LYP_R2:
        value = g_ThisVF->prepareGcf_LyapunovR2(prec, points, index);
        break;
    case EVAL_GCF_CYL1:
        value = g_ThisVF->prepareGcf_LyapunovCyl(-PI_DIV4, PI_DIV4, prec,
                                                 points, index);
        break;
    case EVAL_GCF_CYL2:
        value = g_ThisVF->prepareGcf_LyapunovCyl(PI_DIV4, PI - PI_DIV4, prec,
                                                 points, index);
        break;
    case EVAL_GCF_CYL3:
        value = g_ThisVF->prepareGcf_LyapunovCyl(PI - PI_DIV4, PI + PI_DIV4,
                                                 prec, points, index);
        break;
    case EVAL_GCF_CYL4:
        value = g_ThisVF->prepareGcf_LyapunovCyl(-PI + PI_DIV4, -PI_DIV4, prec,
                                                 points, index);
        break;
    default:
        value = false;
        break;
    }

    return g_ThisVF->evaluateGcf();
}

static bool readTaskResults(int task, int index)
{
    bool value;

    switch (task) {
    case EVAL_GCF_R2:
        value = read_gcf(R2_to_psphere, index);
        break;
    case EVAL_GCF_U1:
        value = read_gcf(U1_to_psphere, index);
        break;
    case EVAL_GCF_V1:
        value = read_gcf(VV1_to_psphere, index);
        break;
    case EVAL_GCF_U2:
        value = read_gcf(U2_to_psphere, index);
        break;
    case EVAL_GCF_V2:
        value = read_gcf(VV2_to_psphere, index);
        break;
    case EVAL_GCF_LYP_R2:
        value = read_gcf(rplane_plsphere0, index);
        break;
    // here: the old herc files contained (the equivalent of)
    // value = read_gcf( rplane_psphere ,index);
    case EVAL_GCF_CYL1:
        value = read_gcf(cylinder_to_plsphere, index);
        break;
    case EVAL_GCF_CYL2:
        value = read_gcf(cylinder_to_plsphere, index);
        break;
    case EVAL_GCF_CYL3:
        value = read_gcf(cylinder_to_plsphere, index);
        break;
    case EVAL_GCF_CYL4:
        value = read_gcf(cylinder_to_plsphere, index);
        break;
    default:
        value = false;
    }

    return value;
}

void draw_gcf(std::shared_ptr<P4WinSphere> spherewnd,
              const std::vector<p4orbits::orbits_points> &sep, int color,
              int dashes)
{
    double pcoord[3];

    for (auto it : sep) {
        if (it.dashes && dashes)
            (*plot_l)(spherewnd, pcoord, it.pcoord, color);
        else
            (*plot_p)(spherewnd, it.pcoord, color);
        copy_x_into_y(it.pcoord, pcoord);
    }
}

static void insert_gcf_point(double x0, double y0, double z0, int dashes,
                             int index)
{
    int pcoord[3] = {x0, y0, z0};

    g_VFResults.vf_[index].gcf_points_.push_back(
        p4orbits::orbits_point(CSING, pcoord, dashes, 0, 0));
}

static bool read_gcf(void (*chart)(double, double, double *), int index)
{
    int k;
    FILE *fp;
    double x, y;
    double pcoord[3];
    int d, c;

    fp = fopen(QFile::encodeName(g_ThisVF->getfilename_gcf()), "r");
    if (fp == nullptr)
        return false;

    k = 0;
    while (1) {
        d = 0;
        while (fscanf(fp, "%lf %lf", &x, &y) == 2) {
            k++;
            chart(x, y, pcoord);
            insert_gcf_point(pcoord[0], pcoord[1], pcoord[2], d, index);
            // d=1;
            d = s_GcfDashes;
        }
        for (c = getc(fp); isspace(c);)
            c = getc(fp);
        if (c != ',')
            break;
    }

    fclose(fp);
    return true;
}
