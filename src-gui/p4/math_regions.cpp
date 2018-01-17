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

#include "math_regions.h"

// ---------------------------------------------------------------------
//  getVFIndex_UU1, getVFIndex_UU2, getVFIndex_VV1, getVFIndex_VV2
// ---------------------------------------------------------------------
//
// When integration starts, for example in a U1 chart in a point (z1,z2)
// very close to infinity (close to z2=0, but z2>0), it may sometimes happen
// that the line of infinity is crossed (to a point z2<0).  The resulting
// point lies in the U1 chart, but in a wrong part of it.  We say that
// it lies in the VV1-part: this point will be mapped to the symmetric point
// in the V1-chart.
//
// So:
//
//   * a point in the UU1 chart lies in the wrong part of the V1-chart, and is
//   in fact a U1-point
//   * a point in the UU2 chart lies in the wrong part of the V2-chart, and is
//   in fact a U2-point
//   * a point in the VV1 chart lies in the wrong part of the U1-chart, and is
//   in fact a V1-point
//   * a point in the VV2 chart lies in the wrong part of the U2-chart, and is
//   in fact a V2-point
//
// The relation between the symmetric charts is simple:
//              (z1,z2) ---> (-z1,-z2).
//
// Note: these charts are only used when p=q=1.
int P4InputVF::getVFIndex_UU1(const double *yy)
{
    double y[2];
    y[0] = -yy[0];
    y[1] = -yy[1];
    return getVFIndex_U1(y);
}

int P4InputVF::getVFIndex_UU2(const double *yy)
{
    double y[2];
    y[0] = -yy[0];
    y[1] = -yy[1];
    return getVFIndex_U2(y);
}

int P4InputVF::getVFIndex_VV1(const double *yy)
{
    double y[2];
    y[0] = -yy[0];
    y[1] = -yy[1];
    return getVFIndex_V1(y);
}

int P4InputVF::getVFIndex_VV2(const double *yy)
{
    double y[2];
    y[0] = -yy[0];
    y[1] = -yy[1];
    return getVFIndex_V2(y);
}

// ---------------------------------------------------------------------
//  isInsideRegion_R2, isInsideRegion_R2_epsilon
// ---------------------------------------------------------------------
//
// Checks if a points lies in a region determined by a set of signs.
// The point lies in the finite region.
//
// The _epsilon variant increases the region by an epsilon amount before
// checking
bool isInsideRegion_R2(const int *signs, const double *ucoord)
{
    int k;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        if (eval_term2(g_VFResults.curves_result_[k].sep, ucoord) < 0) {
            if (signs[k] > 0)
                return false;
        } else {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

bool isInsideRegion_R2_epsilon(const int *signs, const double *ucoord,
                               double epsilon)
{
    int k;
    double v;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        v = eval_term2(g_VFResults.curves_result_[k].sep, ucoord);
        if (v < -epsilon) {
            if (signs[k] > 0)
                return false;
        }
        if (v > epsilon) {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------
//  isInsideRegion_U1, isInsideRegion_U1_epsilon
// ---------------------------------------------------------------------
//
// Checks if a points lies in a region determined by a set of signs.
// The point lies in the U1 chart.
// This is only used when p=q=1
//
// The _epsilon variant increases the region by an epsilon amount before
// checking
bool isInsideRegion_U1(const int *signs, const double *z1z2)
{
    int k;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        if (eval_term2(g_VFResults.curves_result_[k].sep_U1, z1z2) < 0) {
            if (signs[k] > 0)
                return false;
        } else {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

bool isInsideRegion_U1_epsilon(const int *signs, const double *z1z2,
                               double epsilon)
{
    int k;
    double v;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        v = eval_term2(g_VFResults.curves_result_[k].sep_U1, z1z2);
        if (v < -epsilon) {
            if (signs[k] > 0)
                return false;
        }
        if (v > epsilon) {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------
//  isInsideRegion_V1, isInsideRegion_V1_epsilon
// ---------------------------------------------------------------------
//
// Checks if a points lies in a region determined by a set of signs.
// The point lies in the V1 chart.
// This is only used when p=q=1
//
// The _epsilon variant increases the region by an epsilon amount before
// checking
bool isInsideRegion_V1(const int *signs, const double *z1z2)
{
    int k;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        if (eval_term2(g_VFResults.curves_result_[k].sep_V1, z1z2) < 0) {
            if (signs[k] > 0)
                return false;
        } else {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

bool isInsideRegion_V1_epsilon(const int *signs, const double *z1z2,
                               double epsilon)
{
    int k;
    double v;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        v = eval_term2(g_VFResults.curves_result_[k].sep_V1, z1z2);
        if (v < -epsilon) {
            if (signs[k] > 0)
                return false;
        }
        if (v > epsilon) {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------
//  isInsideRegion_U2, isInsideRegion_U2_epsilon
// ---------------------------------------------------------------------
//
// Checks if a points lies in a region determined by a set of signs.
// The point lies in the U2 chart.
// This is only used when p=q=1
//
// The _epsilon variant increases the region by an epsilon amount before
// checking
//

bool isInsideRegion_U2(const int *signs, const double *z1z2)
{
    int k;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        if (eval_term2(g_VFResults.curves_result_[k].sep_U2, z1z2) < 0) {
            if (signs[k] > 0)
                return false;
        } else {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}
bool isInsideRegion_U2_epsilon(const int *signs, const double *z1z2,
                               double epsilon)
{
    int k;
    double v;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        v = eval_term2(g_VFResults.curves_result_[k].sep_U2, z1z2);
        if (v < -epsilon) {
            if (signs[k] > 0)
                return false;
        }
        if (v > epsilon) {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------
//                      isInsideRegion_V2
//                      isInsideRegion_V2_epsilon
// ---------------------------------------------------------------------
//
// Checks if a points lies in a region determined by a set of signs.
// The point lies in the V2 chart.
// This is only used when p=q=1
//
// The _epsilon variant increases the region by an epsilon amount before
// checking
bool isInsideRegion_V2(const int *signs, const double *z1z2)
{
    int k;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        if (eval_term2(g_VFResults.curves_result_[k].sep_V2, z1z2) < 0) {
            if (signs[k] > 0)
                return false;
        } else {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

bool isInsideRegion_V2_epsilon(const int *signs, const double *z1z2,
                               double epsilon)
{
    int k;
    double v;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        v = eval_term2(g_VFResults.curves_result_[k].sep_V2, z1z2);
        if (v < -epsilon) {
            if (signs[k] > 0)
                return false;
        }
        if (v > epsilon) {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------
//                      isInsideRegion_cyl
//                      isInsideRegion_cyl_epsilon
// ---------------------------------------------------------------------
//
// Checks if a points lies in a region determined by a set of signs.
// The point lies in the part near infinity, in the case of Poincare-
// Lyapunov compactification ((p,q) is not (1,1)).
//
// The _epsilon variant increases the region by an epsilon amount before
// checking
bool isInsideRegion_cyl(const int *signs, const double *rtheta)
{
    int k;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        if (eval_term3(g_VFResults.curves_result_[k].sep_C, rtheta) < 0) {
            if (signs[k] > 0)
                return false;
        } else {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

bool isInsideRegion_cyl_epsilon(const int *signs, const double *rtheta,
                                double epsilon)
{
    int k;
    double v;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        v = eval_term3(g_VFResults.curves_result_[k].sep_C, rtheta);
        if (v < -epsilon) {
            if (signs[k] > 0)
                return false;
        }
        if (v > epsilon) {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------
//                      isInsideRegion_sphere
//                      isInsideRegion_sphere_epsilon
// ---------------------------------------------------------------------
//
// Checks if a points lies in a region determined by a set of signs.
// We don't know where the point lies, so we have to determine this first.
//
// The _epsilon variant increases the region by an epsilon amount before
// checking
//
bool isInsideRegion_sphere(const int *signs, const double *pcoord)
{
    double ucoord[2], theta;

    if (!g_VFResults.plweights_) {
        if (pcoord[2] > ZCOORD) {
            psphere_to_R2(pcoord[0], pcoord[1], pcoord[2], ucoord);
            return isInsideRegion_R2(signs, ucoord);
        } else {
            theta = atan2(fabs(pcoord[1]), fabs(pcoord[0]));
            if (theta < PI_DIV4 && theta > -PI_DIV4) {
                if (pcoord[0] > 0) {
                    psphere_to_U1(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return isInsideRegion_U1(signs, ucoord);
                } else {
                    psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return isInsideRegion_V1(signs, ucoord);
                }
            } else {
                if (pcoord[1] > 0) {
                    psphere_to_U2(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return isInsideRegion_U2(signs, ucoord);
                } else {
                    psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return isInsideRegion_V2(signs, ucoord);
                }
            }
        }
    } else {
        ucoord[0] = pcoord[1];
        ucoord[1] = pcoord[2];

        if (pcoord[0])
            return isInsideRegion_R2(signs, ucoord);
        else
            return isInsideRegion_cyl(signs, ucoord);
    }
}

bool isInsideRegion_sphere_epsilon(const int *signs, const double *pcoord,
                                   double epsilon)
{
    double ucoord[2], theta;

    if (!g_VFResults.plweights_) {
        if (pcoord[2] > ZCOORD) {
            psphere_to_R2(pcoord[0], pcoord[1], pcoord[2], ucoord);
            return isInsideRegion_R2_epsilon(signs, ucoord, epsilon);
        } else {
            theta = atan2(fabs(pcoord[1]), fabs(pcoord[0]));
            if (theta < PI_DIV4 && theta > -PI_DIV4) {
                if (pcoord[0] > 0) {
                    psphere_to_U1(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return isInsideRegion_U1_epsilon(signs, ucoord, epsilon);
                } else {
                    psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return isInsideRegion_V1_epsilon(signs, ucoord, epsilon);
                }
            } else {
                if (pcoord[1] > 0) {
                    psphere_to_U2(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return isInsideRegion_U2_epsilon(signs, ucoord, epsilon);
                } else {
                    psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return isInsideRegion_V2_epsilon(signs, ucoord, epsilon);
                }
            }
        }
    } else {
        ucoord[0] = pcoord[1];
        ucoord[1] = pcoord[2];

        if (pcoord[0])
            return isInsideRegion_R2_epsilon(signs, ucoord, epsilon);
        else
            return isInsideRegion_cyl_epsilon(signs, ucoord, epsilon);
    }
}

// ---------------------------------------------------------------------
//          eval_curve
// ---------------------------------------------------------------------
//
// A polynom is evaluated in a point pcoord=(X,Y,Z).  Only the sign of
// the resulting polynomial is important, so therefore we can replace
// the polynomial by a rescaled one if the point lies close to infinity.
//
// So we first determine where the point lies.
double eval_curve(p4curveRegions::curveResult c, const double *pcoord)
{
    double ucoord[2], theta;
    if (!g_VFResults.plweights_) {
        if (pcoord[2] > ZCOORD) {
            psphere_to_R2(pcoord[0], pcoord[1], pcoord[2], ucoord);
            return eval_term2(c.sep, ucoord);
        } else {
            theta = atan2(fabs(pcoord[1], fabs(pcoord[0])));
            if (theta < PI_DIV4 && theta > -PI_DIV4) {
                if (pcoord[0] > 0) {
                    psphere_to_U1(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return eval_term2(c.sep_U1, ucoord);
                } else {
                    psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return eval_term2(c.sep_V1, ucoord);
                }
            } else {
                if (pcoord[1] > 0) {
                    psphere_to_U2(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return eval_term2(c.sep_U2, ucoord);
                } else {
                    psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return eval_term2(c.sep_V2, ucoord);
                }
            }
        }
    } else {
        ucoord[0] = pcoord[1];
        ucoord[1] = pcoord[2];
        if (pcoord[0])
            return eval_term2(c.sep, ucoord);
        else
            return eval_term3(c.sep_C, ucoord);
    }
}

// ---------------------------------------------------------------------
//          describeRegion
// ---------------------------------------------------------------------
//
// included for debugging region
QString describeRegion(double *pcoord)
{
    QString s;
    int k;
    double v;

    s = "";
    for (k = 0; k < g_ThisVF->numcurves; k++) {
        v = eval_curve(g_VFResults.curves_result_[k], pcoord);
        if (fabs(v) <= 1.e-8)
            s += "0";
        else if (v < 0)
            s += "-";
        else
            s += "+";
    }
    return s;
}

// ---------------------------------------------------------------------
//          isInTheSameRegion
// ---------------------------------------------------------------------
bool isInTheSameRegion(double *testpt, double *refpos)
{
    int i;
    if (refpos == nullptr || g_VFResults.curves_result_.empty())
        return true;

    for (i = g_ThisVF->numCurves_ - 1; i >= 0; i--) {
        if (eval_curve(g_VFResults.curves_result_[i], refpos) < 0) {
            if (eval_curve(g_VFResults.curves_result_[i], testpt) > 1e-8)
                return false;
        } else {
            if (eval_curve(g_VFResults.curves_result_[i], testpt) < -1e-8)
                return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------
//          isARealSingularity
// ---------------------------------------------------------------------
bool isARealSingularity(double *pcoord, int vfindex)
{
    if (g_VFResults.curves == nullptr)
        return false;

    for (int i = g_ThisVF->numVFRegions_ - 1; i >= 0; i--) {
        if (g_ThisVF->vfRegions_[i].vfIndex == vfindex)
            if (isInsideRegion_sphere_epsilon(g_ThisVF->vfRegions_[i].signs,
                                              pcoord, 1e-8))
                return true;
    }
    return false;
}

bool isARealSingularity(double x0, double y0, int chart, int vfindex)
{
    double pcoord[3];

    switch (chart) {
    case CHART_R2:
        MATHFUNC(R2_to_sphere)(x0, y0, pcoord);
        break;
    case CHART_U1:
        MATHFUNC(U1_to_sphere)(x0, y0, pcoord);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(x0, y0, pcoord);
        break;
    case CHART_U2:
        MATHFUNC(U2_to_sphere)(x0, y0, pcoord);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(x0, y0, pcoord);
        break;
    default:
        return false;
    }

    return isARealSingularity(pcoord, vfindex);
}

// ---------------------------------------------------------------------
//          pSphereDistance
// ---------------------------------------------------------------------
//
// Calculates somehow the distance between two points on the sphere
double pSphereDistance(double *p, double *q)
{
    double theta1, theta2;
    double rp[2], rq[2];
    double dist;

    if (p[2] == 0) {
        if (q[2] != 0)
            return 1.0; // one is finite, the other is infinite

        theta1 = atan2(p[1], p[0]);
        theta2 = atan2(q[1], q[0]);

        dist = fabs(theta1 - theta2);
        if (fabs(theta1 - theta2 - TWOPI) < dist)
            dist = fabs(theta1 - theta2 - TWOPI);
        if (fabs(theta1 - theta2 + TWOPI) < dist)
            dist = fabs(theta1 - theta2 + TWOPI);

        return dist;
    }

    if (q[2] == 0)
        return 1.0; // one is finite, the other is infinite

    // both are finite:

    psphere_to_R2(p[0], p[1], p[2], rp);
    psphere_to_R2(q[0], q[1], q[2], rq);

    dist =
        (rp[0] - rq[0]) * (rp[0] - rq[0]) + (rp[1] - rq[1]) * (rp[1] - rq[1]);
    return sqrt(dist);
}

// ---------------------------------------------------------------------
//          plSphereDistance
// ---------------------------------------------------------------------
//
// Calculates somehow the distance between two points on the sphere
static double plSphereDistance(double *p, double *q)
{
    double theta1, theta2;
    double rp[2], rq[2];
    double dist;

    if (p[0] == 1 && p[1] == 0) {
        if (!(q[0] == 1 && q[1] == 0))
            return 1.0; // one is finite, the other is infinite

        theta1 = p[2];
        theta2 = q[2];

        dist = fabs(theta1 - theta2);
        if (fabs(theta1 - theta2 - TWOPI) < dist)
            dist = fabs(theta1 - theta2 - TWOPI);
        if (fabs(theta1 - theta2 + TWOPI) < dist)
            dist = fabs(theta1 - theta2 + TWOPI);

        return dist;
    }

    if (q[0] == 1 && q[1] == 0)
        return 1.0; // one is finite, the other is infinite

    // both are finite:

    plsphere_to_R2(p[0], p[1], p[2], rp);
    plsphere_to_R2(q[0], q[1], q[2], rq);

    dist =
        (rp[0] - rq[0]) * (rp[0] - rq[0]) + (rp[1] - rq[1]) * (rp[1] - rq[1]);
    return sqrt(dist);
}

// ---------------------------------------------------------------------
//          markSingularity
// ---------------------------------------------------------------------
// saddle version
void markSingularity(p4singularities::saddle &s,
                     std::vector<positionitem> &plist, int &numpos, int vfindex,
                     bool plweights)
{
    double pcoord[3];
    int N{numpos};

    switch (s.chart) {
    case CHART_R2:
        MATHFUNC(R2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_U1:
        MATHFUNC(U1_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_U2:
        MATHFUNC(U2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    default:
        return;
    }

    //    Debug( "Sing (x0,y0)=(%f,%f) Chart=%d P=(%f,%f,%f)",
    //    s.x0,s.y0,s.chart,pcoord[0],pcoord[1],pcoord[2]);

    if (!isARealSingularity(pcoord, vfindex)) {
        s.position = POSITION_VIRTUAL;
        return;
    }

    for (int k = 0; k < N; k++) {
        if (plweights_) {
            if (plSphereDistance(pcoord, plist[k].pcoord) < 1e-8) {
                //                Debug( "coinciding" );
                s.position = POSITION_COINCIDING;
                plist[k].s.position = POSITION_COINCIDING_MAIN;
                return;
            }
        } else {
            if (pSphereDistance(pcoord, plist[k].pcoord) < 1e-8) {
                //                Debug( "coinciding" );
                s.position = POSITION_COINCIDING;
                plist[k].s.position = POSITION_COINCIDING_MAIN;
                return;
            }
        }
    }
    //    Debug( "standalone" );
    s.position = POSITION_STANDALONE;

    positionitem newPosition;
    newPosition.pcoord[0] = pcoord[0];
    newPosition.pcoord[1] = pcoord[1];
    newPosition.pcoord[2] = pcoord[2];
    // this automatically casts the saddle as a genericsingularity
    newPosition.s = s;

    plist.push_back(newPosition);

    numpos = N + 1;
}

// ---------------------------------------------------------------------
//          markSingularity
// ---------------------------------------------------------------------
// semi elementary version
void markSingularity(p4singularities::semi_elementary s,
                     std::vector<positionitem> &plist, int &numpos, int vfindex,
                     bool plweights)
{
    double pcoord[3];
    int N{numpos};

    switch (s.chart) {
    case CHART_R2:
        MATHFUNC(R2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_U1:
        MATHFUNC(U1_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_U2:
        MATHFUNC(U2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    default:
        return;
    }

    //    Debug( "Sing (x0,y0)=(%f,%f) Chart=%d P=(%f,%f,%f)",
    //    s.x0,s.y0,s.chart,pcoord[0],pcoord[1],pcoord[2]);

    if (!isARealSingularity(pcoord, vfindex)) {
        s.position = POSITION_VIRTUAL;
        return;
    }

    for (int k = 0; k < N; k++) {
        if (plweights_) {
            if (plSphereDistance(pcoord, plist[k].pcoord) < 1e-8) {
                //                Debug( "coinciding" );
                s.position = POSITION_COINCIDING;
                plist[k].s.position = POSITION_COINCIDING_MAIN;
                return;
            }
        } else {
            if (pSphereDistance(pcoord, plist[k].pcoord) < 1e-8) {
                //                Debug( "coinciding" );
                s.position = POSITION_COINCIDING;
                plist[k].s.position = POSITION_COINCIDING_MAIN;
                return;
            }
        }
    }
    //    Debug( "standalone" );
    s.position = POSITION_STANDALONE;

    positionitem newPosition;
    newPosition.pcoord[0] = pcoord[0];
    newPosition.pcoord[1] = pcoord[1];
    newPosition.pcoord[2] = pcoord[2];
    newPosition.s = s;

    plist.push_back(newPosition);

    numpos = N + 1;
}

// ---------------------------------------------------------------------
//          markSingularity
// ---------------------------------------------------------------------
// node version
void markSingularity(p4singularities::node s, std::vector<positionitem> &plist,
                     int &numpos, int vfindex, bool plweights)
{
    double pcoord[3];
    int N{numpos};

    switch (s.chart) {
    case CHART_R2:
        MATHFUNC(R2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_U1:
        MATHFUNC(U1_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_U2:
        MATHFUNC(U2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    default:
        return;
    }

    //    Debug( "Sing (x0,y0)=(%f,%f) Chart=%d P=(%f,%f,%f)",
    //    s.x0,s.y0,s.chart,pcoord[0],pcoord[1],pcoord[2]);

    if (!isARealSingularity(pcoord, vfindex)) {
        s.position = POSITION_VIRTUAL;
        return;
    }

    for (int k = 0; k < N; k++) {
        if (plweights_) {
            if (plSphereDistance(pcoord, plist[k].pcoord) < 1e-8) {
                //                Debug( "coinciding" );
                s.position = POSITION_COINCIDING;
                plist[k].s.position = POSITION_COINCIDING_MAIN;
                return;
            }
        } else {
            if (pSphereDistance(pcoord, plist[k].pcoord) < 1e-8) {
                //                Debug( "coinciding" );
                s.position = POSITION_COINCIDING;
                plist[k].s.position = POSITION_COINCIDING_MAIN;
                return;
            }
        }
    }
    //    Debug( "standalone" );
    s.position = POSITION_STANDALONE;

    positionitem newPosition;
    newPosition.pcoord[0] = pcoord[0];
    newPosition.pcoord[1] = pcoord[1];
    newPosition.pcoord[2] = pcoord[2];
    newPosition.s = s;

    plist.push_back(newPosition);

    numpos = N + 1;
}

// ---------------------------------------------------------------------
//          markSingularity
// ---------------------------------------------------------------------
// strong focus version
void markSingularity(p4singularities::strong_focus s,
                     std::vector<positionitem> &plist, int &numpos, int vfindex,
                     bool plweights)
{
    double pcoord[3];
    int N{numpos};

    switch (s.chart) {
    case CHART_R2:
        MATHFUNC(R2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_U1:
        MATHFUNC(U1_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_U2:
        MATHFUNC(U2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    default:
        return;
    }

    //    Debug( "Sing (x0,y0)=(%f,%f) Chart=%d P=(%f,%f,%f)",
    //    s.x0,s.y0,s.chart,pcoord[0],pcoord[1],pcoord[2]);

    if (!isARealSingularity(pcoord, vfindex)) {
        s.position = POSITION_VIRTUAL;
        return;
    }

    for (int k = 0; k < N; k++) {
        if (plweights_) {
            if (plSphereDistance(pcoord, plist[k].pcoord) < 1e-8) {
                //                Debug( "coinciding" );
                s.position = POSITION_COINCIDING;
                plist[k].s.position = POSITION_COINCIDING_MAIN;
                return;
            }
        } else {
            if (pSphereDistance(pcoord, plist[k].pcoord) < 1e-8) {
                //                Debug( "coinciding" );
                s.position = POSITION_COINCIDING;
                plist[k].s.position = POSITION_COINCIDING_MAIN;
                return;
            }
        }
    }
    //    Debug( "standalone" );
    s.position = POSITION_STANDALONE;

    positionitem newPosition;
    newPosition.pcoord[0] = pcoord[0];
    newPosition.pcoord[1] = pcoord[1];
    newPosition.pcoord[2] = pcoord[2];
    newPosition.s = s;

    plist.push_back(newPosition);

    numpos = N + 1;
}

// ---------------------------------------------------------------------
//          markSingularity
// ---------------------------------------------------------------------
// weak focus version
void markSingularity(p4singularities::weak_focus s,
                     std::vector<positionitem> &plist, int &numpos, int vfindex,
                     bool plweights)
{
    double pcoord[3];
    int N{numpos};

    switch (s.chart) {
    case CHART_R2:
        MATHFUNC(R2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_U1:
        MATHFUNC(U1_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_U2:
        MATHFUNC(U2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    default:
        return;
    }

    //    Debug( "Sing (x0,y0)=(%f,%f) Chart=%d P=(%f,%f,%f)",
    //    s.x0,s.y0,s.chart,pcoord[0],pcoord[1],pcoord[2]);

    if (!isARealSingularity(pcoord, vfindex)) {
        s.position = POSITION_VIRTUAL;
        return;
    }

    for (int k = 0; k < N; k++) {
        if (plweights_) {
            if (plSphereDistance(pcoord, plist[k].pcoord) < 1e-8) {
                //                Debug( "coinciding" );
                s.position = POSITION_COINCIDING;
                plist[k].s.position = POSITION_COINCIDING_MAIN;
                return;
            }
        } else {
            if (pSphereDistance(pcoord, plist[k].pcoord) < 1e-8) {
                //                Debug( "coinciding" );
                s.position = POSITION_COINCIDING;
                plist[k].s.position = POSITION_COINCIDING_MAIN;
                return;
            }
        }
    }
    //    Debug( "standalone" );
    s.position = POSITION_STANDALONE;

    positionitem newPosition;
    newPosition.pcoord[0] = pcoord[0];
    newPosition.pcoord[1] = pcoord[1];
    newPosition.pcoord[2] = pcoord[2];
    newPosition.s = s;

    plist.push_back(newPosition);

    numpos = N + 1;
}

// ---------------------------------------------------------------------
//          markSingularity
// ---------------------------------------------------------------------
// degenerate version
void markSingularity(p4singularities::degenerate s,
                     std::vector<positionitem> &plist, int &numpos, int vfindex,
                     bool plweights)
{
    double pcoord[3];
    int N{numpos};

    switch (s.chart) {
    case CHART_R2:
        MATHFUNC(R2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_U1:
        MATHFUNC(U1_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_V1:
        MATHFUNC(V1_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_U2:
        MATHFUNC(U2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    case CHART_V2:
        MATHFUNC(V2_to_sphere)(s.x0, s.y0, pcoord);
        break;
    default:
        return;
    }

    //    Debug( "Sing (x0,y0)=(%f,%f) Chart=%d P=(%f,%f,%f)",
    //    s.x0,s.y0,s.chart,pcoord[0],pcoord[1],pcoord[2]);

    if (!isARealSingularity(pcoord, vfindex)) {
        s.position = POSITION_VIRTUAL;
        return;
    }

    for (int k = 0; k < N; k++) {
        if (plweights_) {
            if (plSphereDistance(pcoord, plist[k].pcoord) < 1e-8) {
                //                Debug( "coinciding" );
                s.position = POSITION_COINCIDING;
                plist[k].s.position = POSITION_COINCIDING_MAIN;
                return;
            }
        } else {
            if (pSphereDistance(pcoord, plist[k].pcoord) < 1e-8) {
                //                Debug( "coinciding" );
                s.position = POSITION_COINCIDING;
                plist[k].s.position = POSITION_COINCIDING_MAIN;
                return;
            }
        }
    }
    //    Debug( "standalone" );
    s.position = POSITION_STANDALONE;

    positionitem newPosition;
    newPosition.pcoord[0] = pcoord[0];
    newPosition.pcoord[1] = pcoord[1];
    newPosition.pcoord[2] = pcoord[2];
    newPosition.s = s;

    plist.push_back(newPosition);

    numpos = N + 1;
}