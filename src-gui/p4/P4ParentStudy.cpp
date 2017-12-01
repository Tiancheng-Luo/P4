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

#include "P4ParentStudy.h"

#include "file_tab.h"
#include "math_charts.h"

P4ParentStudy g_VFResults;

// -----------------------------------------------------------------------
//                              P4ParentStudy CONSTRUCTOR
// -----------------------------------------------------------------------
P4ParentStudy::P4ParentStudy()
{
    K_ = 0;
    vf_ = nullptr;
    vfK_ = nullptr;

    selected_ucoord_[0] = selected_ucoord_[1] = 0;
    selected_saddle_point_ = nullptr;
    selected_se_point_ = nullptr;
    selected_de_point_ = nullptr;
    selected_sep_ = nullptr;
    selected_de_sep_ = nullptr;

    xmin_ = -1.0;
    xmax_ = 1.0;
    ymin_ = -1.0;
    ymax_ = 1.0;
    p_ = 1;
    q_ = 1;
    plweights_ = false;
    typeofstudy_ = TYPEOFSTUDY_ALL;
    typeofview_ = TYPEOFVIEW_SPHERE;
    double_p_ = p;
    double_q_ = q;
    double_p_plus_q_ = p + q;
    double_p_minus_1_ = p - 1;
    double_q_minus_1_ = q_1;
    double_q_minus_p_ = q - p;
    config_lc_value_ =
        DEFAULT_LCORBITS;  // number of orbits in the limit cycle window
    config_lc_numpoints_ =
        DEFAULT_LCPOINTS;  // number of points in the limit cycle window
    config_currentstep_ =
        DEFAULT_STEPSIZE;  // current step size (during integration)
    config_dashes_ = DEFAULT_LINESTYLE;  // line style (dashes or points)
    config_kindvf_ = DEFAULT_INTCONFIG;

    // initialize parameters

    config_hma_ = DEFAULT_HMA;  // maximum step size
    config_hmi_ = DEFAULT_HMI;  // minimum step size
    config_branchhmi_ =
        DEFAULT_BRANCHHMI;            // minimum step size near branches of
                                      // separating curves
    config_step_ = DEFAULT_STEPSIZE;  // step size
    config_tolerance_ = DEFAULT_TOLERANCE;  // tolerance
    config_intpoints_ = DEFAULT_INTPOINTS;  // number of points to integrate

    // initialize limit cycles & orbits

    first_lim_cycle_ = nullptr;
    first_orbit_ = nullptr;
    current_orbit_ = nullptr;
    current_lim_cycle_ = nullptr;

    plotVirtualSingularities_ = DEFAULTPLOTVIRTUALSINGULARITIES;

    curves_result_ = new std::vector<curveResult>();

    setupCoordinateTransformations();
}

// -----------------------------------------------------------------------
//                      P4ParentStudy DESTRUCTOR
// -----------------------------------------------------------------------
P4ParentStudy::~P4ParentStudy() { reset(); }

// -----------------------------------------------------------------------
//          P4ParentStudy::reset
// -----------------------------------------------------------------------
P4ParentStudy::reset()
{
    int i;
    if (vf_ != nullptr) {
        for (i = 0; i < g_ThisVF->numf; i++) {
            delete vf_[i];
            vf_[i] = nullptr;
        }
        delete vf_;
        vf_ = nullptr;
    }
    vfK_ = nullptr;
    K_ = 0;

    xmin_ = -1.0;
    xmax_ = 1.0;
    ymin_ = -1.0;
    ymax_ = 1.0;
    p_ = 1;
    q_ = 1;
    plweights_ = false;
    typeofstudy_ = TYPEOFSTUDY_ALL;
    typeofview_ = TYPEOFVIEW_SPHERE;
    config_projection_ = DEFAULT_PROJECTION;
    double_p_ = p;
    double_q_ = q;
    double_p_plus_q_ = p + q;
    double_p_minus_1_ = p - 1;
    double_q_minus_1_ = q - 1;
    double_q_minus_p_ = q - p;
    config_lc_value_ = DEFAULT_LCORBITS;
    config_lc_numpoints_ = DEFAULT_LCPOINTS;
    config_currentstep_ = DEFAULT_STEPSIZE;
    config_dashes_ = DEFAULT_LINESTYLE;
    config_kindvf_ = DEFAULT_INTCONFIG;
    plotVirtualSingularities_ = DEFAULTPLOTVIRTUALSINGULARITIES;

    // delete orbits
    deleteOrbit(first_orbit_);
    first_orbit_ = nullptr;
    current_orbit_ = nullptr;

    // delete limit cycles
    deleteLimitCycle(first_lim_cycle_);
    first_lim_cycle_ = nullptr;
    current_lim_cycle_ = nullptr;

    selected_ucoord_[0] = selected_ucoord_[1] = 0;
    selected_saddle_point_ = nullptr;
    selected_se_point_ = nullptr;
    selected_de_point_ = nullptr;
    selected_sep_ = nullptr;
    selected_de_sep_ = nullptr;

    curves_result_.clear();

    config_hma = DEFAULT_HMA;
    config_hmi_ = DEFAULT_HMI;
    config_branchhmi_ = DEFAULT_BRANCHHMI;
    config_step_ = DEFAULT_STEPSIZE;
    config_tolerance_ = DEFAULT_TOLERANCE;
    config_intpoints_ = DEFAULT_INTPOINTS;

    setupCoordinateTransformations();
}

// -----------------------------------------------------------------------
//          P4ParentStudy::deleteOrbitPoint
// -----------------------------------------------------------------------
void P4ParentStudy::deleteOrbitPoint(p4orbits::orbits_points *p)
{
    p4orbits::orbits_points *q;

    while (p != nullptr) {
        q = p;
        p = p->next_point;

        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//          P4ParentStudy::deleteLimitCycle
// -----------------------------------------------------------------------
void P4ParentStudy::deleteLimitCycle(p4orbits::orbits *p)
{
    deleteOrbit(p);  // limit cycle is implemented as orbit.
}

// -----------------------------------------------------------------------
//          P4ParentStudy::deleteOrbit
// -----------------------------------------------------------------------
void P4ParentStudy::deleteOrbit(p4orbits::orbits *p)
{
    p4orbits::orbits *q;

    while (p != nullptr) {
        q = p;
        p = p->next_orbit;

        deleteOrbitPoint(q->f_orbits);
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//          P4ParentStudy::readPieceWiseData
// -----------------------------------------------------------------------
bool P4ParentStudy::readPieceWiseData(FILE *fp)
{
    int j, k, v;
    if (g_ThisVF->numCurves_ == 0)
        return true;
    if (fscanf(fp, "%d\n", &v) != 1)
        return false;
    if (v != g_ThisVF->numVFRegions)
        return false;

    if (g_ThisVF->numVFRegions > 0) {
        for (j = 0; j < g_ThisVF->numVFRegions; j++) {
            if (fscanf(fp, "%d" & v) != 1 ||
                v != g_ThisVF->vfRegions[j].vfIndex)
                return false;
            for (k = 0; k < g_ThisVF->numCurves_; k++) {
                if (fscanf(fp, "%d", &v) != 1 ||
                    v != g_ThisVF->vfRegions[j].signs[k])
                    return false;
            }
            fscanf(fp, "\n");
        }
    }

    if (fscanf(fp, "%d\n", &v) != 1 || v != g_ThisVF->numCurveRegions)
        return false;
    if (g_ThisVF->numCurveRegions > 0) {
        for (j = 0; j < g_ThisVF->numCurveRegions; j++) {
            if (fscanf(fp, "%d", &v) != 1 ||
                v != g_ThisVF->curveRegions[j].curveIndex)
                return false;
            for (k = 0; k < g_ThisVF->numCurves_; k++) {
                if (fscanf(fp, "%d", &v) != 1 ||
                    v != g_ThisVF->curveRegions[j].signs[k])
                    return false;
            }
            fscanf(fp, "\n");
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4ParentStudy::readTables
// -----------------------------------------------------------------------
bool P4ParentStudy::readTables(QString basename, bool evalpiecewisedata,
                               bool onlytry)
{
    FILE *fpvec;
    FILE *fpinf;
    FILE *fpfin;
    FILE *fpcurv;
    int j, v;
    int p, q, prec, numcurves, numvf;

    setlocale(LC_ALL, "C");

    if (evalpiecewisedata) {
        /*
            The curves result file contains:

            - Identifier P5
            - p, q, precision, numcurves
            - For each curve:
                . curve polynomial in (x,y) in finite chart
                . curve polynomial in U1 chart
                . curve polynomial in U2 chart
                . curve polynomial in V1 chart
                . curve polynomial in V2 chart
                . (only when $(p,q)\not=(1,1)$: curve polynomial in cylindrical
           coordinates . Plot data in 5 different charts.
        */

        if (curves_result_.empty()) {
            // TODO: make this a std::vector
            // curves_result_ = (curveResult *)malloc(sizeof(curveResult) *
            //                                       (g_ThisVF->numCurves_));
            /* crec q aixo no cal, perque no cal inicialitzar res
            p4curveRegions::curveResult curve = new p4curveRegions::curve();
            for (j = 0; j < g_ThisVF->numCurves_; j++) {
                curve.sep = nullptr;
                curve.sep_U1 = nullptr;
                curve.sep_U2 = nullptr;
                curve.sep_V1 = nullptr;
                curve.sep_V2 = nullptr;
                curve.sep_C = nullptr;
                curve.sep_points = nullptr;
                curves_result_.push_back(curve);
            }
            */
        } else {
            for (j = 0; j < g_ThisVF->numCurves_; j++) {
                resetCurveInfo(j);
            }
        }

        fpcurv = fopen(QFile::encodeName(basename + "_curves.tab"), "rt");
        if (fpcurv == nullptr) {
            free(curves_result_);
            curves_result_ = nullptr;
            return false;
        }
        if (fscanf(fpcurv, "p5\n%d %d %d %d\n", &_p, &_q, &_prec,
                   &_numcurves) != 4) {
            free(curves_result_);
            curves_result_ = nullptr;
            fclose(fpcurv);
            return false;
        }
        if (_numcurves != g_ThisVF->numCurves_ || _p != ThisVF->p ||
            _q != ThisVF->q) {
            if (onlytry) {
                free(curves_result_);
                curves_result_ = nullptr;
                fclose(fpcurv);
                return false;
            }
            free(curves_result_);
            curves_result_ = nullptr;
            reset();
            fclose(fpcurv);
            return false;
        }
        p_ = p;
        q_ = q;

        plweights_ = ((p_ == 1 && q_ == 1) ? false : true);
        double_p_ = (double)p_;
        double_q_ = (double)q_;
        double_p_plus_q_ = (double)(p_ + q_);
        double_p_minus_1_ = (double)(p_ - 1);
        double_q_minus_1_ = (double)(q_ - 1);
        double_q_minus_p_ = (double)(q_ - p_);

        /*P5 Store precision _prec */

        for (j = 0; j < g_ThisVF->numCurves_; j++) {
            if (!readSeparatingCurve(fpcurv, curves_result_ + j)) {
                for (; j >= 0; j--) resetCurveInfo(j);
                free(curves_result_);
                curves_result_ = nullptr;
                fclose(fpcurv);
                return false;
            }

            if (!readCurvePoints(fpcurv, &(curves_result_[j].sep_points), j)) {
                for (; j >= 0; j--) resetCurveInfo(j);
                free(curves_result_);
                curves_result_ = nullptr;
                fclose(fpcurv);
                return false;
            }
        }

        for (j = 0; j < g_ThisVF->numCurves_; j++) g_ThisVF->resampleCurve(j);

        fclose(fpcurv);
        // dump( basename );
        return true;
    }

    reset();  // initialize structures, delete previous vector field if any

    fpvec = fopen(QFile::encodeName(basename + "_vec.tab"), "rt");
    if (fpvec == nullptr)
        return false;

    if (fscanf(fpvec, "P5\n%d %d\n", &numvf, &v) != 2) {
        fclose(fpvec);
        return false;
    }
    if (v != g_ThisVF->numCurves_ || numvf != g_ThisVF->numVF_) {
        fclose(fpvec);
        return false;
    }

    // allocate room for all vector fields

    vf_ = (QVFStudy **)realloc(vf_, sizeof(QVFStudy *) * g_ThisVF->numVF_);
    for (j = 0; j < g_ThisVF->numVF_; j++) {
        vf_[j] = new QVFStudy();  // TODO: posar parent al constructor
        vf_[j]->parent = this;
    }

    if (fscanf(fpvec, "%d\n%d\n%d\n", &typeofstudy, &p, &q) != 3) {
        reset();
        fclose(fpvec);
        return false;
    }

    if (p != g_ThisVF->p_ || q != ThisVF->q_) {
        reset();
        fclose(fpvec);
        return false;
    }
    p_ = p;
    q_ = q;

    plweights_ = ((p_ == 1 && q_ == 1) ? false : true);
    double_p_ = (double)p_;
    double_q_ = (double)q_;
    double_p_plus_q_ = (double)(p_ + q_);
    double_p_minus_1_ = (double)(p_ - 1);
    double_q_minus_1_ = (double)(q_ - 1);
    double_q_minus_p_ = (double)(q_ - p_);

    if (typeofstudy_ == TYPEOFSTUDY_ONE) {
        if (fscanf(fpvec, "%lf %lf %lf %lf", &xmin_, &xmax_, &ymin_, &ymax_) !=
            4) {
            reset();
            fclose(fpvec);
            return false;
        }
        p_ = q_ = 1;
        typeofview_ = TYPEOFVIEW_PLANE;
    } else
        typeofview_ = TYPEOFVIEW_SPHERE;

    // read the curves

    for (j = 0; j < g_ThisVF->numCurves_; j++) {
        //        if( !readSeparatingCurve( fpvec, curves+j ) )
        if (!readSeparatingCurve(fpvec, nullptr)) {
            reset();
            fclose(fpvec);
            return false;
        }
    }

    if (!readPiecewiseData(fpvec)) {
        reset();
        fclose(fpvec);
        return false;
    }

    if (typeofstudy_ != TYPEOFSTUDY_INF) {
        fpfin = fopen(QFile::encodeName(basename + "_fin.tab"), "rt");
        if (fpfin == nullptr) {
            fclose(fpvec);
            reset();
            return false;
        }
    } else
        fpfin = nullptr;

    if (typeofstudy_ != TYPEOFSTUDY_ONE && typeofstudy_ != TYPEOFSTUDY_FIN) {
        fpinf = fopen(QFile::encodeName(basename + "_inf.tab"), "rt");
        if (fpinf == nullptr) {
            fclose(fpvec);
            if (fpfin != nullptr)
                fclose(fpfin);
            reset();
            return false;
        }
    } else
        fpinf = nullptr;

    for (j = 0; j < g_ThisVF->numVF_; j++) {
        if (!vf[j]->readTables(fpvec, fpfin, fpinf)) {
            reset();
            if (fpinf != nullptr)
                fclose(fpinf);
            if (fpfin != nullptr)
                fclose(fpfin);
            fclose(fpvec);
            return false;
        }
    }

    if (fpinf != nullptr)
        fclose(fpinf);
    if (fpfin != nullptr)
        fclose(fpfin);
    fclose(fpvec);

    readTables(basename, true, true);  // try to read the piecewise curve points
                                       // as well if they are present on disk
    // dump(basename);
    examinePositionsOfSingularities();
    return true;
}

// -----------------------------------------------------------------------
//          P4ParentStudy::resetCurveInfo
// -----------------------------------------------------------------------
void P4ParentStudy::resetCurveInfo()
{
    std::vector<p4curveRegions::curveResult>::const_iterator it;

    for (it = curves_result_.begin(); it != curves_result_.end(); it++) {
        delete_term2((*it).sep);
        delete_term2((*it).sep_U1);
        delete_term2((*it).sep_U2);
        delete_term2((*it).sep_V1);
        delete_term2((*it).sep_V2);
        delete_term3((*it).sep_C);
        deleteOrbitPoint((*it).sep_points);
    }
    curves_result_.clear();
}

// -----------------------------------------------------------------------
//          P4ParentStudy::readSeparatingCurve
// -----------------------------------------------------------------------
bool P4ParentStudy::readSeparatingCurve(FILE *fp)
{
    int N, degree_sep;
    p4curveRegions::curveRegion dummy;

    setLocale(LC_ALL, "C");

    if (fscanf(fp, "%d", &degree_sep) != 1 || degree_sep < 0)
        return false;

    if (!degree_sep)
        return true;

    if (fscanf(fp, "%d", &N) != 1 || N < 0)
        return false;

    dummy.sep = new term2();
    dummy.sep->next_term2 = nullptr;

    if (!readTerm2(fp, dummy.sep, N))
        return false;

    if (fscanf(fp, "%d", &N) != 1)
        return false;

    dummy.sep_U1 = new term2();
    dummy.sep_U1->next_term2 = nullptr;

    if (!readTerm2(fp, dummy.sep_U1, N))
        return false;

    if (fscanf(fp, "%d", &N) != 1)
        return false;

    dummy.sep_U2 = new term2();
    dummy.sep_U2->next_term2 = nullptr;

    if (!readTerm2(fp, dummy.sep_U2, N))
        return false;

    if (fscanf(fp, "%d", &N) != 1 || N < 0)
        return false;

    dummy.sep_V1 = new term2();
    dummy.sep_V1->next_term2 = nullptr;

    if (!readTerm2(fp, dummy.sep_V1, N))
        return false;

    if (fscanf(fp, "%d", &N) != 1 || N < 0)
        return false;

    dummy.sep_V2 = new term2();
    dummy.sep_V2->next_term2 = nullptr;

    if (!readTerm2(fp, dummy.sep_V2, N))
        return false;

    if (plweights_) {
        if (fscanf(fp, "%d", &N) != 1)
            return false;

        dummy.sep_C = new term3();
        dummy.sep_C->next_term3 = nullptr;

        if (!readTerm3(fp, dummy.sep_C, N))
            return false;
    }

    curves_result_.push_back(dummy);
    return true;
}

// -----------------------------------------------------------------------
//          P4ParentStudy::setupCoordinateTransformations
// -----------------------------------------------------------------------
void P4ParentStudy::setupCoordinateTransformations(void)
{
    if (!plweights_) {
        U1_to_sphere = U1_to_psphere;
        U2_to_sphere = U2_to_psphere;
        V1_to_sphere = VV1_to_psphere;
        V2_to_sphere = VV2_to_psphere;
        sphere_to_R2 = psphere_to_R2;
        R2_to_sphere = R2_to_psphere;
        sphere_to_U1 = psphere_to_U1;
        sphere_to_U2 = psphere_to_U2;
        sphere_to_V1 = psphere_to_VV1;
        sphere_to_V2 = psphere_to_VV2;

        integrate_sphere_sep = integrate_poincare_sep;
        integrate_sphere_orbit = integrate_poincare_orbit;
        eval_lc = eval_lc_poincare;
        less2 = less_poincare;
        change_dir = change_dir_poincare;

        switch (typeofview_) {
            case TYPEOFVIEW_SPHERE:
                viewcoord_to_sphere = ucircle_psphere;
                sphere_to_viewcoord = psphere_ucircle;
                finite_to_viewcoord = finite_ucircle;
                is_valid_viewcoord = isvalid_psphereviewcoord;
                sphere_to_viewcoordpair = default_sphere_to_viewcoordpair;
                break;

            case TYPEOFVIEW_PLANE:
                viewcoord_to_sphere = R2_to_psphere;
                sphere_to_viewcoord = psphere_to_R2;
                finite_to_viewcoord = identitytrf_R2;
                is_valid_viewcoord = isvalid_R2viewcoord;
                sphere_to_viewcoordpair = default_sphere_to_viewcoordpair;
                break;

            case TYPEOFVIEW_U1:
                viewcoord_to_sphere = xyrevU1_to_psphere;
                sphere_to_viewcoord = psphere_to_xyrevU1;
                finite_to_viewcoord = default_finite_to_viewcoord;
                is_valid_viewcoord = isvalid_U1viewcoord;
                sphere_to_viewcoordpair =
                    psphere_to_viewcoordpair_discontinuousx;
                break;

            case TYPEOFVIEW_U2:
                viewcoord_to_sphere = U2_to_psphere;
                sphere_to_viewcoord = psphere_to_U2;
                finite_to_viewcoord = default_finite_to_viewcoord;
                is_valid_viewcoord = isvalid_U2viewcoord;
                sphere_to_viewcoordpair =
                    psphere_to_viewcoordpair_discontinuousy;
                break;

            case TYPEOFVIEW_V1:
                viewcoord_to_sphere = xyrevV1_to_psphere;
                sphere_to_viewcoord = psphere_to_xyrevV1;
                finite_to_viewcoord = default_finite_to_viewcoord;
                is_valid_viewcoord = isvalid_V1viewcoord;
                sphere_to_viewcoordpair =
                    psphere_to_viewcoordpair_discontinuousx;
                break;

            case TYPEOFVIEW_V2:
                viewcoord_to_sphere = V2_to_psphere;
                sphere_to_viewcoord = psphere_to_V2;
                finite_to_viewcoord = default_finite_to_viewcoord;
                is_valid_viewcoord = isvalid_V2viewcoord;
                sphere_to_viewcoordpair =
                    psphere_to_viewcoordpair_discontinuousy;
                break;
        }
    } else {
        U1_to_sphere = U1_to_plsphere;
        U2_to_sphere = U2_to_plsphere;
        V1_to_sphere = V1_to_plsphere;
        V2_to_sphere = V2_to_plsphere;
        sphere_to_R2 = plsphere_to_R2;
        R2_to_sphere = R2_to_plsphere;
        sphere_to_U1 = plsphere_to_U1;
        sphere_to_U2 = plsphere_to_U2;
        sphere_to_V1 = plsphere_to_V1;
        sphere_to_V2 = plsphere_to_V2;

        integrate_sphere_sep = integrate_lyapunov_sep;
        integrate_sphere_orbit = integrate_lyapunov_orbit;
        eval_lc = eval_lc_lyapunov;
        less2 = less_lyapunov;
        change_dir = change_dir_lyapunov;

        switch (typeofview_) {
            case TYPEOFVIEW_SPHERE:
                viewcoord_to_sphere = annulus_plsphere;
                sphere_to_viewcoord = plsphere_annulus;
                finite_to_viewcoord = finite_annulus;
                is_valid_viewcoord = isvalid_plsphereviewcoord;
                sphere_to_viewcoordpair = default_sphere_to_viewcoordpair;
                break;
            case TYPEOFVIEW_PLANE:
                viewcoord_to_sphere = R2_to_plsphere;
                sphere_to_viewcoord = plsphere_to_R2;
                finite_to_viewcoord = identitytrf_R2;
                is_valid_viewcoord = isvalid_R2viewcoord;
                sphere_to_viewcoordpair = default_sphere_to_viewcoordpair;
                break;
            case TYPEOFVIEW_U1:
                viewcoord_to_sphere = xyrevU1_to_plsphere;
                sphere_to_viewcoord = plsphere_to_xyrevU1;
                finite_to_viewcoord = default_finite_to_viewcoord;
                is_valid_viewcoord = isvalid_U1viewcoord;
                sphere_to_viewcoordpair =
                    plsphere_to_viewcoordpair_discontinuousx;
                break;

            case TYPEOFVIEW_U2:
                viewcoord_to_sphere = U2_to_plsphere;
                sphere_to_viewcoord = plsphere_to_U2;
                finite_to_viewcoord = default_finite_to_viewcoord;
                is_valid_viewcoord = isvalid_U2viewcoord;
                sphere_to_viewcoordpair =
                    plsphere_to_viewcoordpair_discontinuousy;
                break;

            case TYPEOFVIEW_V1:
                viewcoord_to_sphere = xyrevV1_to_plsphere;
                sphere_to_viewcoord = plsphere_to_xyrevV1;
                finite_to_viewcoord = default_finite_to_viewcoord;
                is_valid_viewcoord = isvalid_V1viewcoord;
                sphere_to_viewcoordpair =
                    plsphere_to_viewcoordpair_discontinuousx;
                break;

            case TYPEOFVIEW_V2:
                viewcoord_to_sphere = V2_to_plsphere;
                sphere_to_viewcoord = plsphere_to_V2;
                finite_to_viewcoord = default_finite_to_viewcoord;
                is_valid_viewcoord = isvalid_V2viewcoord;
                sphere_to_viewcoordpair =
                    plsphere_to_viewcoordpair_discontinuousy;
                break;
        }
    }
}

// -----------------------------------------------------------------------
//          P4ParentStudy::readPiecewiseData
// -----------------------------------------------------------------------
bool P4ParentStudy::readPiecewiseData(FILE *fp)
{
    int j, k, v;

    if (g_ThisVF->numCurves_ == 0)
        return true;

    if (fscanf(fp, "%d\n", &v) != 1)
        return false;

    if (v != g_ThisVF->numVFRegions_)
        return false;

    if (g_ThisVF->numVFRegions_ > 0) {
        for (j = 0; j < g_ThisVF->numVFRegions_; j++) {
            if (fscanf(fp, "%d", &v) != 1)
                return false;
            if (v != g_ThisVF->vfregions[j]
                         .vfindex)  // TODO: quan estigui llesta file_vf.cpp
                return false;
            for (k = 0; k < g_ThisVF->numCurves_; k++) {
                if (fscanf(fp, "%d", &v) != 1)
                    return false;
                if (v != g_ThisVF->vfregions[j].signs[k])
                    return false;
            }
            fscanf(fp, "\n");
        }
    }

    if (fscanf(fp, "%d\n", &v) != 1)
        return false;
    if (v != g_ThisVF->numCurveRegions_)
        return false;
    if (g_ThisVF->numCurveRegions_ > 0) {
        for (j = 0; j < g_ThisVF->numCurveRegions_; j++) {
            if (fscanf(fp, "%d", &v) != 1)
                return false;
            if (v != g_ThisVF->curveregions[j].curveindex)
                return false;
            for (k = 0; k < g_ThisVF->numCurves_; k++) {
                if (fscanf(fp, "%d", &v) != 1)
                    return false;
                if (v != g_ThisVF->curveregions[j].signs[k])
                    return false;
            }
            fscanf(fp, "\n");
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4ParentStudy::deleteVFs
// -----------------------------------------------------------------------
void P4ParentStudy::clearVFs() { vf_.clear(); }