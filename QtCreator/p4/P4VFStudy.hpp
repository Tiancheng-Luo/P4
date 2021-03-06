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

#include <QObject>

#include "structures.hpp"

class QTextEdit;

class P4ParentStudy;

class P4VFStudy : public QObject
{
    Q_OBJECT

  public:
    // Constructor
    P4VFStudy(P4ParentStudy *parent = nullptr);

    P4ParentStudy *parent_;

    // general information

    bool singinf_{false};
    int dir_vec_field_{1};

    // vector field in various charts
    // FIXME arreglar tots els llocs on s'usen
    P4Polynom::term2 *f_vec_field_[2]{nullptr, nullptr};
    P4Polynom::term2 *vec_field_U1_[2]{nullptr, nullptr};
    P4Polynom::term2 *vec_field_U2_[2]{nullptr, nullptr};
    P4Polynom::term2 *vec_field_V1_[2]{nullptr, nullptr};
    P4Polynom::term2 *vec_field_V2_[2]{nullptr, nullptr};
    P4Polynom::term3 *vec_field_C_[2]{nullptr, nullptr};

    // singular points and their properties:
    P4Singularities::saddle *firstSaddlePoint_{nullptr};
    P4Singularities::semi_elementary *firstSePoint_{nullptr};
    P4Singularities::node *firstNodePoint_{nullptr};
    P4Singularities::strong_focus *firstSfPoint_{nullptr};
    P4Singularities::weak_focus *firstWfPoint_{nullptr};
    P4Singularities::degenerate *firstDePoint_{nullptr};

    // Greatest common factor if present:
    P4Polynom::term2 *gcf_{nullptr};
    P4Polynom::term2 *gcf_U1_{nullptr};
    P4Polynom::term2 *gcf_U2_{nullptr};
    P4Polynom::term2 *gcf_V1_{nullptr};
    P4Polynom::term2 *gcf_V2_{nullptr};
    P4Polynom::term3 *gcf_C_{nullptr};
    P4Orbits::orbits_points *gcf_points_{nullptr};

    // isoclines
    std::vector<P4Curves::isoclines> isocline_vector_;

    /* CLASS METHODS */
    void reset();

    // reading of the Maple/Reduce results
    bool readTables(FILE *, FILE *, FILE *);

    bool readGCF(FILE *fp);               // TODO
    bool readIsoclines(QString basename); // TODO

    bool readVectorField(FILE *fp, P4Polynom::term2 *vf[]);
    bool readVectorFieldCylinder(FILE *fp, P4Polynom::term3 *vf[]);

    bool readPoints(FILE *fp);
    bool readSaddlePoint(FILE *fp);
    bool readSemiElementaryPoint(FILE *fp);
    bool readStrongFocusPoint(FILE *fp);
    bool readWeakFocusPoint(FILE *fp);
    bool readDegeneratePoint(FILE *fp);
    bool readNodePoint(FILE *fp);
    bool readBlowupPoints(FILE *fp, P4Blowup::blow_up_points *b, int n);
    bool readTransformations(FILE *fp, P4Blowup::transformations *trans, int n);

    void setupCoordinateTransformations(); // see math_p4.cpp

    void dump(QTextEdit &m);

  private:
    void dumpSeparatrices(QTextEdit &m, P4Blowup::sep *separ, int margin);
    void dumpSingularities(QTextEdit &m, P4Singularities::saddle *p,
                           bool longversion);
    void dumpSingularities(QTextEdit &m, P4Singularities::degenerate *p,
                           bool longversion);
    void dumpSingularities(QTextEdit &m, P4Singularities::strong_focus *p,
                           bool longversion);
    void dumpSingularities(QTextEdit &m, P4Singularities::weak_focus *p,
                           bool longversion);
    void dumpSingularities(QTextEdit &m, P4Singularities::node *p,
                           bool longversion);
    void dumpSingularities(QTextEdit &m, P4Singularities::semi_elementary *p,
                           bool longversion);
};
