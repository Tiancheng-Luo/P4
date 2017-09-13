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

#include "win_isoclines.h"

#include "custom.h"
#include "file_vf.h"
#include "math_gcf.h"
#include "math_isoclines.h"
#include "math_polynom.h"

#include <QButtonGroup>
#include <QMessageBox>

QIsoclinesDlg::QIsoclinesDlg(QPlotWnd *plt, QWinSphere *sp)
    : QWidget(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint), mainSphere_(sp),
      plotwnd_(plt)
{
    edt_value_ = new QLineEdit("", this);
    QLabel *lbl0 = new QLabel("&Value = ", this);
    lbl0->setBuddy(edt_value_);

    QButtonGroup *btngrp = new QButtonGroup(this);
    btn_dots_ = new QRadioButton("Dots", this);
    btn_dashes_ = new QRadioButton("Dashes", this);
    btngrp->addButton(btn_dots_);
    btngrp->addButton(btn_dashes_);
    QLabel *lbl1 = new QLabel("Appearance: ", this);

    edt_points_ = new QLineEdit("", this);
    QLabel *lbl2 = new QLabel("Num. Points: ", this);

    edt_precis_ = new QLineEdit("", this);
    QLabel *lbl3 = new QLabel("Precision: ", this);

    edt_memory_ = new QLineEdit("", this);
    QLabel *lbl4 = new QLabel("Max. Memory: ", this);

    btnEvaluate_ = new QPushButton("&Evaluate", this);
    btnPlot_ = new QPushButton("&Plot", this);
    btnDelLast_ = new QPushButton("&Delete Last Isocline", this);
    btnDelAll_ = new QPushButton("Delete &All Isoclines", this);

#ifdef TOOLTIPS
    edt_value_->setToolTip("Value of isoclines to plot.\nCan be 0 for 0-slope "
                           "isoclines, and \"inf\"\n(without quotes, case "
                           "insensitive) for\ninfinite-slope isoclines.");
    btnEvaluate_->setToolTip("Evaluate isoclines at the selected value.");
    btnPlot_->setToolTip("Plot isocline.");
    btnDelLast_->setToolTip("Delete last isocline drawn");
    btnDelAll_->setToolTip("Delete all isoclines (separatrices remain)");
    QString ttip;
    ttip = QString::fromStdString("Number of points. Must be between " +
                                  std::to_string(MIN_CURVEPOINTS) + " and " +
                                  std::to_string(MAX_CURVEPOINTS));
    edt_points_->setToolTip(ttip);
    ttip = QString::fromStdString("Required precision. Must be between " +
                                  std::to_string(MIN_CURVEPRECIS) + " and " +
                                  std::to_string(MAX_CURVEPRECIS));
    edt_precis_->setToolTip(ttip);
    ttip = QString::fromStdString("Maximum amount of memory (in kilobytes) "
                                  "spent on plotting GCF.\nMust be between " +
                                  std::to_string(MIN_CURVEMEMORY) + " and " +
                                  std::to_string(MAX_CURVEMEMORY));
    edt_memory_->setToolTip(ttip);
#endif

    // layout
    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QHBoxLayout *layout0 = new QHBoxLayout();
    layout0->addWidget(lbl1);
    layout0->addWidget(btn_dots_);
    layout0->addWidget(btn_dashes_);

    QGridLayout *layout1 = new QGridLayout();
    layout1->addWidget(lbl0, 0, 0);
    layout1->addWidget(edt_value_, 0, 1);
    layout1->addWidget(lbl2, 1, 0);
    layout1->addWidget(edt_points_, 1, 1);
    layout1->addWidget(lbl3, 2, 0);
    layout1->addWidget(edt_precis_, 2, 1);
    layout1->addWidget(lbl4, 3, 0);
    layout1->addWidget(edt_memory_, 3, 1);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addWidget(btnEvaluate_);
    layout2->addStretch(0);
    layout2->addWidget(btnDelLast_);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addWidget(btnPlot_);
    layout3->addStretch(0);
    layout3->addWidget(btnDelAll_);

    mainLayout_->addLayout(layout0);
    mainLayout_->addLayout(layout1);
    mainLayout_->addLayout(layout2);
    mainLayout_->addLayout(layout3);

    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout_);

    // connections
    QObject::connect(btnEvaluate_, SIGNAL(clicked()), this,
                     SLOT(onBtnEvaluate()));
    QObject::connect(btnPlot_, SIGNAL(clicked()), this, SLOT(onBtnPlot()));
    QObject::connect(btnDelAll_, SIGNAL(clicked()), this, SLOT(onBtnDelAll()));
    QObject::connect(btnDelLast_, SIGNAL(clicked()), this,
                     SLOT(onBtnDelLast()));

    // finishing

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    if (!g_VFResults.isocline_vector_.empty()) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }

    setP4WindowTitle(this, "Plot Isoclines");
}

void QIsoclinesDlg::onBtnEvaluate(void)
{
    bool ok;
    if (edt_value_->text().isNull() && edt_value_->text().isEmpty()) {
        QMessageBox::information(
            this, "P4",
            "The value field has to be filled with a valid number.");
        return;
    } else {
        QString val = edt_value_->text();
        g_ThisVF->getDataFromDlg();
        val = g_ThisVF->convertMapleUserParametersLabelsToValues(val);
        val.toDouble(&ok);
        if (!ok) {
            QMessageBox::information(
                this, "P4",
                "The value field has to be filled with a valid number.\n(P4 "
                "does not parse mathematical expressions, so\nmake sure to "
                "enter a value and not a string of\noperations)");
            return;
        }
    }
    if ((g_ThisVF->xdot_ == "0" || g_ThisVF->xdot_.isEmpty()) &&
        (g_ThisVF->ydot_ == "0" || g_ThisVF->ydot_.isEmpty())) {
        QMessageBox::information(this, "P4", "Check that the vector field is "
                                             "correctly introduced.\nIf you "
                                             "used an input file, make sure "
                                             "you pressed\nthe Load button.");
        return;
    }
    if (edt_value_->text().trimmed() == "0") {
        g_ThisVF->isoclines_ = g_ThisVF->ydot_;
    } else if (edt_value_->text().trimmed().toLower() == "inf") {
        g_ThisVF->isoclines_ = g_ThisVF->xdot_;
    } else {
        g_ThisVF->isoclines_ = "(" + g_ThisVF->ydot_ + ")-(" +
                               edt_value_->text().trimmed() + ")*(" +
                               g_ThisVF->xdot_ + ")";
    }

    // FIRST: create filename_vecisoclines.tab for transforming the isoclines
    // QString to a list of P4POLYNOM2
    g_ThisVF->isoclinesDlg_ = this;
    g_ThisVF->evaluateIsoclinesTable();
    btnPlot_->setEnabled(true);
    plotwnd_->getDlgData();
}

void QIsoclinesDlg::onBtnPlot(void)
{
    bool dashes, result;
    int points, precis, memory;

    bool ok;
    QString buf;

    dashes = btn_dashes_->isChecked();

    ok = true;

    buf = edt_points_->text();
    points = buf.toInt();
    if (points < MIN_CURVEPOINTS || points > MAX_CURVEPOINTS) {
        buf += " ???";
        edt_points_->setText(buf);
        ok = false;
    }

    buf = edt_precis_->text();
    precis = buf.toInt();
    if (precis < MIN_CURVEPRECIS || precis > MAX_CURVEPRECIS) {
        buf += " ???";
        edt_precis_->setText(buf);
        ok = false;
    }

    buf = edt_memory_->text();
    memory = buf.toInt();
    if (memory < MIN_CURVEMEMORY || memory > MAX_CURVEMEMORY) {
        buf += " ???";
        edt_memory_->setText(buf);
        ok = false;
    }

    if (!ok) {
        QMessageBox::information(
            this, "P4", "One of the fields has a value that is out of bounds.\n"
                        "Please correct before continuing.\n");
        return;
    }

    // SECOND: read the resulting file and store the list
    if (!g_VFResults.readIsoclines(g_ThisVF->getbarefilename())) {
        QMessageBox::critical(this, "P4", "Cannot read isoclines.\n"
                                          "Please check the input field!\n");
        return;
    }
    // THIRD: evaluate isoclines with given parameters {dashes, points, memory}.

    evaluating_points_ = points;
    evaluating_memory_ = memory;
    evaluating_precision_ = precis;

    btnPlot_->setEnabled(false);

    g_ThisVF->isoclinesDlg_ = this;

    result = evalIsoclinesStart(mainSphere_, dashes, precis, points);
    if (!result) {
        btnPlot_->setEnabled(true);
        QMessageBox::critical(this, "P4",
                              "An error occured while plotting the "
                              "isoclines.\nThe singular locus may not "
                              "be visible, or may be partially "
                              "visible.");
        return;
    }

    btnDelAll_->setEnabled(true);
    btnDelLast_->setEnabled(true);
}

void QIsoclinesDlg::onBtnDelAll(void)
{
    plotwnd_->getDlgData();

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);
    btnDelAll_->setEnabled(false);
    btnDelLast_->setEnabled(false);

    g_VFResults.isocline_vector_.clear();

    mainSphere_->refresh();
}

void QIsoclinesDlg::onBtnDelLast(void)
{
    plotwnd_->getDlgData();

    deleteLastIsocline(mainSphere_);

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    if (g_VFResults.isocline_vector_.empty()) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }
}

void QIsoclinesDlg::reset(void)
{
    QString buf;

    edt_value_->setText("");

    buf.sprintf("%d", DEFAULT_CURVEPOINTS);
    edt_points_->setText(buf);

    buf.sprintf("%d", DEFAULT_CURVEMEMORY);
    edt_memory_->setText(buf);

    buf.sprintf("%d", DEFAULT_CURVEPRECIS);
    edt_precis_->setText(buf);

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    // if (g_VFResults.first_isoclines_ != nullptr) {
    if (!g_VFResults.isocline_vector_.empty()) {
        btnDelLast_->setEnabled(true);
        btnDelAll_->setEnabled(true);
    }

    if (g_VFResults.config_dashes_)
        btn_dashes_->toggle();
    else
        btn_dots_->toggle();
}

void QIsoclinesDlg::finishIsoclinesEvaluation()
{
    bool result;

    if (btnPlot_->isEnabled() == true)
        return; // not busy??

    result = evalIsoclinesContinue(evaluating_precision_, evaluating_points_);

    if (result) {
        btnPlot_->setEnabled(false);
        result = evalIsoclinesFinish(); // return false in case an error occured
        if (!result) {
            QMessageBox::critical(this, "P4",
                                  "An error occured while plotting "
                                  "the isoclines.\nThe singular locus "
                                  "may not be visible, or may "
                                  "be partially visible.");
        }
    }
}