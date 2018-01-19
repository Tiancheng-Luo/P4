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

#include "file_tab.h"

#include <memory>
#include <vector>

class P4VFStudy;

class P4ParentStudy : public QObject
{
  public:
    // Constructor and destructor
    P4ParentStudy();
    //~P4ParentStudy();

    std::vector<std::unique_ptr<P4VFStudy>> vf_;
    // shortcut for vf[K]. Must be updated whenever K changes
    // std::shared_ptr<P4VFStudy> vfK_; // TODO is this really used?
    // K_ will be throughout the current vector field selected
    int K_;

    std::vector<p4curveRegions::curveResult> curves_result_;

    int typeofstudy_;
    int typeofview_;                // TYPEOFVIEW_PLANE or TYPEOFVIEW_SPHERE
    bool plotVirtualSingularities_; // true or false
    int p_;
    int q_;
    bool plweights_; // true if p<>1 or q<>1; false if p=q=1
    double config_projection_;

    double double_p_;                  // shortcuts: = (double)p
    double double_q_;                  // = (double)q
    double double_p_plus_q_;           // = (double)(p+q)
    double double_p_minus_1_;          // = (double)(p-1)
    double double_q_minus_1_;          // = (double)(q-1)
    double double_q_minus_p_;          // = (double)(q-p)
    double xmin_, xmax_, ymin_, ymax_; // in case of local study

    // curves
    std::vector<p4curves::curves> curve_vector_;

    // limit cycles and orbits
    std::vector<p4orbits::orbits> first_lim_cycle_;
    std::vector<p4orbits::orbits> first_orbit_;

    // run-time when plotting
    std::shared_ptr<p4orbits::orbits> current_orbit_;
    std::shared_ptr<p4orbits::orbits> current_lim_cycle_;

    double selected_ucoord_[2];

    // FIXME arreglar aquest berenjenal
    // FIXME selected_X_X_ no són índexos de vectors, haurien de ser punters a
    // vectors. Realment no necessitarem els vectors de separatrius autèntics
    // (seps_ i de_seps_) perquè sempre seran simplement punters als vectors de
    // saddle/se/de_points
    std::vector<p4singularities::saddle> saddle_points_;
    std::vector<p4singularities::semi_elementary> se_points_;
    std::vector<p4singularities::degenerate> de_points_;
    std::shared_ptr<p4blowup::sep> selected_sep_;
    std::shared_ptr<p4blowup::blow_up_points> selected_de_sep_;

    int selected_sep_vfindex_;

    double config_currentstep_; // current step size (during integration)
    bool config_dashes_;        // line style (dashes or points)
    bool config_kindvf_;        // true for original VF, false for reduced
    int config_lc_value_;       // number of orbits in the limit cycle window
    int config_lc_numpoints_;   // number of points in the limit cycle window
    double config_hma_;         // maximum step size
    double config_hmi_;         // minimum step size
    double config_branchhmi_;   // minimum step size near branches of separating
                                // curves
    double config_step_;        // step size
    double config_tolerance_;   // tolerance
    int config_intpoints_;      // number of points to integrate

    // Methods
    // void deleteOrbitPoint(P4ORBIT p);
    // void deleteLimitCycle(p4orbits::orbits *);
    // void deleteOrbit(p4orbits::orbits *);

    bool readPiecewiseData(FILE *);
    void examinePositionsOfSingularities();
    bool readTables(QString, bool, bool);
    void dump(QString basename);
    void reset();
    void setupCoordinateTransformations();
    bool readSeparatingCurve(FILE *);

    void clearVFs();

    // coordinate transformation routines, set up when starting the plot

    void (*viewcoord_to_sphere)(double, double, double *);
    bool (*sphere_to_viewcoordpair)(double *, double *, double *, double *,
                                    double *, double *);

    void (*finite_to_viewcoord)(double, double, double *);
    void (*sphere_to_viewcoord)(double, double, double, double *);
    bool (*is_valid_viewcoord)(double, double, double *);
    void (*integrate_sphere_sep)(double, double, double, double *, double *,
                                 int *, int *, int *, int *, double, double);
    void (*U1_to_sphere)(double, double, double *);
    void (*U2_to_sphere)(double, double, double *);
    void (*V1_to_sphere)(double, double, double *);
    void (*V2_to_sphere)(double, double, double *);

    void (*sphere_to_U1)(double, double, double, double *);
    void (*sphere_to_U2)(double, double, double, double *);
    void (*sphere_to_V1)(double, double, double, double *);
    void (*sphere_to_V2)(double, double, double, double *);

    void (*sphere_to_R2)(double, double, double, double *);
    void (*R2_to_sphere)(double, double, double *);
    void (*integrate_sphere_orbit)(double, double, double, double *, double *,
                                   int *, int *, double, double);
    double (*eval_lc)(double *, double, double, double);
    bool (*less2)(double *, double *);
    int (*change_dir)(double *);
};

P4ParentStudy g_VFResults;
