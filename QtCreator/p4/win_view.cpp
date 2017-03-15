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

#include "win_view.h"

#include "custom.h"
#include "file_tab.h"
#include "file_vf.h"
#include "main.h"
#include "math_p4.h"
#include "p4application.h"

QViewDlg::~QViewDlg() { GetDataFromDlg(); }

QViewDlg::QViewDlg(QWidget *parent)
    : QWidget(parent = nullptr, Qt::Tool | Qt::WindowStaysOnTopHint)
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    QLabel *p4title = new QLabel("View Parameters", this);
    p4title->setFont(*(p4app->TitleFont));

    QLabel *kindlabel = new QLabel("Type of view: ", this);
    kindlabel->setFont(*(p4app->BoldFont));
    btn_sphere = new QRadioButton("Spherical", this);
    btn_plane = new QRadioButton("Planar", this);
    btn_U1 = new QRadioButton("U1", this);
    btn_U2 = new QRadioButton("U2", this);
    btn_V1 = new QRadioButton("V1", this);
    btn_V2 = new QRadioButton("V2", this);

    QLabel *lbl_projection = new QLabel("Projection:", this);
    lbl_projection->setFont(*(p4app->BoldFont));
    edt_projection = new QLineEdit("-1", this);

    QLabel *lbl_x0 = new QLabel("Min. x:", this);
    lbl_x0->setFont(*(p4app->BoldFont));
    edt_x0 = new QLineEdit("-1", this);

    btn_square = new QPushButton("&Square", this);

    QLabel *lbl_y0 = new QLabel("Min. y:", this);
    lbl_y0->setFont(*(p4app->BoldFont));
    edt_y0 = new QLineEdit("-1", this);

    QLabel *lbl_x1 = new QLabel("Max. x", this);
    lbl_x1->setFont(*(p4app->BoldFont));
    edt_x1 = new QLineEdit("1", this);

    QLabel *lbl_y1 = new QLabel("Max. y", this);
    lbl_y1->setFont(*(p4app->BoldFont));
    edt_y1 = new QLineEdit("1", this);

#ifdef TOOLTIPS
    btn_sphere->setToolTip(
        "Spherical projection of the plane (Poincare or Poincare-Lyapunov)");
    btn_plane->setToolTip(
        "Planar view of a compact, rectangular region in the plane");
    btn_U1->setToolTip("View of the U1 chart.  Infinity is shown as a line.");
    btn_U2->setToolTip("View of the U2 chart.  Infinity is shown as a line.");
    btn_V1->setToolTip("View of the V1 chart.  Infinity is shown as a line.");
    btn_V2->setToolTip("View of the V2 chart.  Infinity is shown as a line.");
    edt_projection->setToolTip(
        "Choose projection point of the Poincare sphere.\n"
        "(Only relevant for spherical projection; point must be < 0)");
    edt_x0->setToolTip("Minimum coordinate on the horizontal axis");
    edt_y0->setToolTip("Minimum coordinate on the vertical axis");
    edt_x1->setToolTip("Maximum coordinate on the horizontal axis");
    edt_y1->setToolTip("Maximum coordinate on the vertical axis");
    btn_square->setToolTip("Fills fields MinY, MaxX, MaxY with "
                           "MinX,-MinX,-MinX respectively,\nto make a square "
                           "rectangle around the origin.");
#endif

    // layout

    mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    mainLayout->addWidget(p4title);

    QGridLayout *kindLayout = new QGridLayout();
    kindLayout->addWidget(kindlabel, 0, 0);
    kindLayout->addWidget(btn_sphere, 0, 1);
    kindLayout->addWidget(btn_plane, 0, 2);
    kindLayout->addWidget(btn_U1, 1, 1);
    kindLayout->addWidget(btn_U2, 1, 2);
    kindLayout->addWidget(btn_V1, 2, 1);
    kindLayout->addWidget(btn_V2, 2, 2);

    mainLayout->addLayout(kindLayout);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->addWidget(lbl_projection);
    layout1->addWidget(edt_projection);
    layout1->addStretch(0);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addWidget(lbl_x0);
    layout2->addWidget(edt_x0);
    layout2->addWidget(btn_square);
    layout2->addStretch(0);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addWidget(lbl_y0);
    layout3->addWidget(edt_y0);
    layout3->addStretch(0);

    QHBoxLayout *layout4 = new QHBoxLayout();
    layout4->addWidget(lbl_x1);
    layout4->addWidget(edt_x1);
    layout4->addStretch(0);

    QHBoxLayout *layout5 = new QHBoxLayout();
    layout5->addWidget(lbl_y1);
    layout5->addWidget(edt_y1);
    layout5->addStretch(0);

    mainLayout->addLayout(layout1);
    mainLayout->addLayout(layout2);
    mainLayout->addLayout(layout3);
    mainLayout->addLayout(layout4);
    mainLayout->addLayout(layout5);
    mainLayout->addStretch(0);

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);

    // connections

    QObject::connect(btn_sphere, SIGNAL(toggled(bool)), this,
                     SLOT(btn_sphere_toggled(bool)));
    QObject::connect(btn_plane, SIGNAL(toggled(bool)), this,
                     SLOT(btn_plane_toggled(bool)));
    QObject::connect(btn_U1, SIGNAL(toggled(bool)), this,
                     SLOT(btn_U1_toggled(bool)));
    QObject::connect(btn_U2, SIGNAL(toggled(bool)), this,
                     SLOT(btn_U2_toggled(bool)));
    QObject::connect(btn_V1, SIGNAL(toggled(bool)), this,
                     SLOT(btn_V1_toggled(bool)));
    QObject::connect(btn_V2, SIGNAL(toggled(bool)), this,
                     SLOT(btn_V2_toggled(bool)));
    QObject::connect(btn_square, SIGNAL(clicked()), this,
                     SLOT(btn_square_clicked()));
    QObject::connect(edt_projection, SIGNAL(textChanged(const QString &)), this,
                     SLOT(OnFieldChange(const QString &)));
    QObject::connect(edt_x0, SIGNAL(textChanged(const QString &)), this,
                     SLOT(OnFieldChange(const QString &)));
    QObject::connect(edt_x1, SIGNAL(textChanged(const QString &)), this,
                     SLOT(OnFieldChange(const QString &)));
    QObject::connect(edt_y0, SIGNAL(textChanged(const QString &)), this,
                     SLOT(OnFieldChange(const QString &)));
    QObject::connect(edt_y1, SIGNAL(textChanged(const QString &)), this,
                     SLOT(OnFieldChange(const QString &)));

    // finishing

    UpdateDlgData();

    setP4WindowTitle(this, "View Parameters");
}

void QViewDlg::OnFieldChange(const QString &dummy)
{
    UNUSED(dummy);
    changed = true;
}

void QViewDlg::btn_sphere_toggled(bool on)
{
    changed = true;
    ExclusiveToggle(on, btn_sphere, btn_plane, btn_U1, btn_U2, btn_V1, btn_V2,
                    nullptr);

    if (btn_sphere->isChecked() == false) {
        edt_x0->setEnabled(true);
        edt_y0->setEnabled(true);
        edt_x1->setEnabled(true);
        edt_y1->setEnabled(true);
        edt_projection->setEnabled(false);
        btn_square->setEnabled(true);
    } else {
        edt_x0->setEnabled(false);
        edt_y0->setEnabled(false);
        edt_x1->setEnabled(false);
        edt_y1->setEnabled(false);
        edt_projection->setEnabled(true);
        btn_square->setEnabled(false);
    }
}

void QViewDlg::btn_plane_toggled(bool on)
{
    changed = true;
    ExclusiveToggle(on, btn_plane, btn_sphere, btn_U1, btn_U2, btn_V1, btn_V2,
                    nullptr);

    if (btn_sphere->isChecked() == false) {
        edt_x0->setEnabled(true);
        edt_y0->setEnabled(true);
        edt_x1->setEnabled(true);
        edt_y1->setEnabled(true);
        edt_projection->setEnabled(false);
        btn_square->setEnabled(true);
    } else {
        edt_x0->setEnabled(false);
        edt_y0->setEnabled(false);
        edt_x1->setEnabled(false);
        edt_y1->setEnabled(false);
        edt_projection->setEnabled(true);
        btn_square->setEnabled(false);
    }
}

void QViewDlg::btn_U1_toggled(bool on)
{
    changed = true;
    ExclusiveToggle(on, btn_U1, btn_plane, btn_sphere, btn_U2, btn_V1, btn_V2,
                    nullptr);

    if (btn_sphere->isChecked() == false) {
        edt_x0->setEnabled(true);
        edt_y0->setEnabled(true);
        edt_x1->setEnabled(true);
        edt_y1->setEnabled(true);
        edt_projection->setEnabled(false);
        btn_square->setEnabled(true);
    } else {
        edt_x0->setEnabled(false);
        edt_y0->setEnabled(false);
        edt_x1->setEnabled(false);
        edt_y1->setEnabled(false);
        edt_projection->setEnabled(true);
        btn_square->setEnabled(false);
    }
}

void QViewDlg::btn_U2_toggled(bool on)
{
    changed = true;
    ExclusiveToggle(on, btn_U2, btn_plane, btn_sphere, btn_U1, btn_V1, btn_V2,
                    nullptr);

    if (btn_sphere->isChecked() == false) {
        edt_x0->setEnabled(true);
        edt_y0->setEnabled(true);
        edt_x1->setEnabled(true);
        edt_y1->setEnabled(true);
        edt_projection->setEnabled(false);
        btn_square->setEnabled(true);
    } else {
        edt_x0->setEnabled(false);
        edt_y0->setEnabled(false);
        edt_x1->setEnabled(false);
        edt_y1->setEnabled(false);
        edt_projection->setEnabled(true);
        btn_square->setEnabled(false);
    }
}

void QViewDlg::btn_V1_toggled(bool on)
{
    changed = true;
    ExclusiveToggle(on, btn_V1, btn_plane, btn_sphere, btn_U1, btn_U2, btn_V2,
                    nullptr);

    if (btn_sphere->isChecked() == false) {
        edt_x0->setEnabled(true);
        edt_y0->setEnabled(true);
        edt_x1->setEnabled(true);
        edt_y1->setEnabled(true);
        edt_projection->setEnabled(false);
        btn_square->setEnabled(true);
    } else {
        edt_x0->setEnabled(false);
        edt_y0->setEnabled(false);
        edt_x1->setEnabled(false);
        edt_y1->setEnabled(false);
        edt_projection->setEnabled(true);
        btn_square->setEnabled(false);
    }
}

void QViewDlg::btn_V2_toggled(bool on)
{
    changed = true;
    ExclusiveToggle(on, btn_V2, btn_plane, btn_sphere, btn_U1, btn_U2, btn_V1,
                    nullptr);

    if (btn_sphere->isChecked() == false) {
        edt_x0->setEnabled(true);
        edt_y0->setEnabled(true);
        edt_x1->setEnabled(true);
        edt_y1->setEnabled(true);
        edt_projection->setEnabled(false);
        btn_square->setEnabled(true);
    } else {
        edt_x0->setEnabled(false);
        edt_y0->setEnabled(false);
        edt_x1->setEnabled(false);
        edt_y1->setEnabled(false);
        edt_projection->setEnabled(true);
        btn_square->setEnabled(false);
    }
}

void QViewDlg::btn_square_clicked(void)
{
    double x0;

    if (ReadFloatField(edt_x0, &x0, X_MIN, MIN_FLOAT, MAX_FLOAT) == false) {
        // no error reading field

        if (x0 > 0)
            x0 = -x0;

        QString buf;
        buf.sprintf("%g", (float)(x0));
        edt_x0->setText(buf);
        buf.sprintf("%g", (float)(-x0));
        edt_x1->setText(buf);
        buf.sprintf("%g", (float)(x0));
        edt_y0->setText(buf);
        buf.sprintf("%g", (float)(-x0));
        edt_y1->setText(buf);
    }
}

void QViewDlg::ExclusiveToggle(bool on, QRadioButton *first, ...)
{
    va_list marker;
    QRadioButton *i;
    QRadioButton *onbtn;

    if (on) {
        i = first;
        va_start(marker, first);
        while (i != nullptr) {
            if (i != first)
                if (i->isChecked())
                    i->toggle();
            i = va_arg(marker, QRadioButton *);
        }
        va_end(marker);
        if (first->isChecked() == false)
            first->toggle();
    } else {
        i = first;
        onbtn = nullptr;
        va_start(marker, first);
        while (i != nullptr) {
            if (i->isChecked())
                onbtn = i;
            i = va_arg(marker, QRadioButton *);
        }
        va_end(marker);
        if (onbtn == nullptr || onbtn == first)
            first->toggle();
    }
}

bool QViewDlg::ReadFloatField(QLineEdit *edt, double *presult, double defvalue,
                              double minvalue, double maxvalue)
{
    // returns true in case of error

    QString x, t;
    bool ok;

    t = edt->text();
    *presult = t.toDouble(&ok);
    if (!ok || *presult < minvalue || *presult > maxvalue) {
        MarkBad(edt);
        *presult = defvalue;
        return true;
    }

    x.sprintf("%g", (float)(*presult));
    if (x != t)
        edt->setText(x);

    return false;
}

void QViewDlg::MarkBad(QLineEdit *edt)
{
    QString t;
    int i;

    t = edt->text();
    while ((i = t.indexOf('?')) >= 0)
        t.remove(i, 1);
    t = t.trimmed();
    t.append(" ???");

    edt->setText(t);
}

bool QViewDlg::GetDataFromDlg(void)
{
    if (!changed) {
        return false;
    }

    changed = false;
    if (btn_sphere->isChecked()) {
        if (VFResults.typeofview != TYPEOFVIEW_SPHERE) {
            changed = true;
            VFResults.typeofview = TYPEOFVIEW_SPHERE;
        }
    }
    if (btn_plane->isChecked()) {
        if (VFResults.typeofview != TYPEOFVIEW_PLANE) {
            changed = true;
            VFResults.typeofview = TYPEOFVIEW_PLANE;
        }
    }
    if (btn_U1->isChecked()) {
        if (VFResults.typeofview != TYPEOFVIEW_U1) {
            changed = true;
            VFResults.typeofview = TYPEOFVIEW_U1;
        }
    }
    if (btn_U2->isChecked()) {
        if (VFResults.typeofview != TYPEOFVIEW_U2) {
            changed = true;
            VFResults.typeofview = TYPEOFVIEW_U2;
        }
    }
    if (btn_V1->isChecked()) {
        if (VFResults.typeofview != TYPEOFVIEW_V1) {
            changed = true;
            VFResults.typeofview = TYPEOFVIEW_V1;
        }
    }
    if (btn_V2->isChecked()) {
        if (VFResults.typeofview != TYPEOFVIEW_V2) {
            changed = true;
            VFResults.typeofview = TYPEOFVIEW_V2;
        }
    }

    double oldxmin = VFResults.xmin;
    double oldymin = VFResults.ymin;
    double oldxmax = VFResults.xmax;
    double oldymax = VFResults.ymax;

    changed |=
        ReadFloatField(edt_projection, &(VFResults.config_projection),
                       DEFAULT_PROJECTION, MIN_PROJECTION, MAX_PROJECTION);
    changed |=
        ReadFloatField(edt_x0, &(VFResults.xmin), X_MIN, MIN_FLOAT, MAX_FLOAT);
    changed |=
        ReadFloatField(edt_y0, &(VFResults.ymin), Y_MIN, MIN_FLOAT, MAX_FLOAT);
    changed |=
        ReadFloatField(edt_x1, &(VFResults.xmax), X_MAX, MIN_FLOAT, MAX_FLOAT);
    changed |=
        ReadFloatField(edt_y1, &(VFResults.ymax), Y_MAX, MIN_FLOAT, MAX_FLOAT);

    if (oldxmin != VFResults.xmin || oldymin != VFResults.ymin ||
        oldxmax != VFResults.xmax || oldymax != VFResults.ymax || changed) {
        changed = false;
        return true;
    }

    return false;
}

void QViewDlg::UpdateDlgData(void)
{
    QString buf;

    changed = false;

    if (VFResults.typeofview == TYPEOFVIEW_PLANE)
        ExclusiveToggle(true, btn_plane, btn_sphere, btn_U1, btn_U2, btn_V1,
                        btn_V2, nullptr);
    if (VFResults.typeofview == TYPEOFVIEW_SPHERE)
        ExclusiveToggle(true, btn_sphere, btn_plane, btn_U1, btn_U2, btn_V1,
                        btn_V2, nullptr);
    if (VFResults.typeofview == TYPEOFVIEW_U1)
        ExclusiveToggle(true, btn_U1, btn_sphere, btn_plane, btn_U2, btn_V1,
                        btn_V2, nullptr);
    if (VFResults.typeofview == TYPEOFVIEW_U2)
        ExclusiveToggle(true, btn_U2, btn_sphere, btn_plane, btn_U1, btn_V1,
                        btn_V2, nullptr);
    if (VFResults.typeofview == TYPEOFVIEW_V1)
        ExclusiveToggle(true, btn_V1, btn_sphere, btn_plane, btn_U1, btn_U2,
                        btn_V2, nullptr);
    if (VFResults.typeofview == TYPEOFVIEW_V2)
        ExclusiveToggle(true, btn_V2, btn_sphere, btn_plane, btn_U1, btn_U2,
                        btn_V1, nullptr);

    if (VFResults.typeofstudy == TYPEOFSTUDY_ONE)
        btn_sphere->setEnabled(false);
    else
        btn_sphere->setEnabled(true);

    buf.sprintf("%g", (float)(VFResults.config_projection));
    edt_projection->setText(buf);
    if (VFResults.typeofview == TYPEOFVIEW_SPHERE && !VFResults.plweights)
        edt_projection->setEnabled(true);
    else
        edt_projection->setEnabled(false);

    buf.sprintf("%g", (float)(VFResults.xmin));
    edt_x0->setText(buf);
    buf.sprintf("%g", (float)(VFResults.xmax));
    edt_x1->setText(buf);
    buf.sprintf("%g", (float)(VFResults.ymin));
    edt_y0->setText(buf);
    buf.sprintf("%g", (float)(VFResults.ymax));
    edt_y1->setText(buf);

    if (btn_sphere->isChecked() == false) {
        edt_x0->setEnabled(true);
        edt_y0->setEnabled(true);
        edt_x1->setEnabled(true);
        edt_y1->setEnabled(true);
        btn_square->setEnabled(true);
    } else {
        edt_x0->setEnabled(false);
        edt_y0->setEnabled(false);
        edt_x1->setEnabled(false);
        edt_y1->setEnabled(false);
        btn_square->setEnabled(false);
    }
}