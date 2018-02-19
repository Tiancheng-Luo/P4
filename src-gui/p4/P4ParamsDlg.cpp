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

#include "P4ParamsDlg.h"

#include "P4FindDlg.h"
#include "custom.h"
#include "file_vf.h"
#include "main.h"
#include "p4application.h"
#include "p4settings.h"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>

P4ParamsDlg::~P4ParamsDlg() { getDataFromDlg(); }

P4ParamsDlg::P4ParamsDlg(std::shared_ptr<P4FindDlg> finddlg)
    : QWidget(finddlg), parent_{std::move(finddlg)}
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    std::unique_ptr<QLabel> p4title{
        std::make_unique<QLabel>("Find Singular Points Parameters", this)};
    p4title->setFont(*(g_p4app->titleFont_));

    std::unique_ptr<QLabel> symlabel{
        std::make_unique<QLabel>("Calculations: ", this)};
    symlabel->setFont(*(g_p4app->boldFont_));
    btn_alg_ = std::make_unique<QRadioButton>("Algebraic", this);
    btn_num_ = std::make_unique<QRadioButton>("Numeric", this);

    std::unique_ptr<QLabel> seplabel{
        std::make_unique<QLabel>("Test Separatrices:  ", this)};
    seplabel->setFont(*(g_p4app->boldFont_));
    btn_sepyes_ = std::make_unique<QRadioButton>("Yes", this);
    btn_sepno_ = std::make_unique<QRadioButton>("No", this);

    std::unique_ptr<QLabel> lbl_precis{
        std::make_unique<QLabel>("Accuracy:", this)};
    lbl_precis->setFont(*(g_p4app->boldFont_));
    spin_precis_ = std::make_unique<QSpinBox>(this);
    spin_precis_->setMinimum(MINPRECISION);
    spin_precis_->setMaximum(MAXPRECISION);

    std::unique_ptr<QLabel> lbl_precis0{
        std::make_unique<QLabel>("Precision:", this)};
    lbl_precis0->setFont(*(g_p4app->boldFont_));
    spin_precis0_ = std::make_unique<QSpinBox>(this);
    spin_precis0_->setMinimum(MINPRECISION0);
    spin_precis0_->setMaximum(MAXPRECISION0);

    std::unique_ptr<QLabel> lbl_epsilon{
        std::make_unique<QLabel>("Epsilon:", this)};
    lbl_epsilon->setFont(*(g_p4app->boldFont_));
    edt_epsilon_ = std::make_unique<QLineEdit>(g_ThisVF->epsilon_, this);

    std::unique_ptr<QLabel> lbl_level{
        new QLabel("Level of Approximation:", this)};
    lbl_level->setFont(*(g_p4app->boldFont_));
    spin_level_ = std::make_unique<QSpinBox>(this);
    spin_level_->setMinimum(MINLEVEL);
    spin_level_->setMaximum(MAXLEVEL);

    std::unique_ptr<QLabel> lbl_numlevel{
        std::make_unique<QLabel>("Numeric level:", this)};
    lbl_numlevel->setFont(*(g_p4app->boldFont_));
    spin_numlevel_ = std::make_unique<QSpinBox>(this);
    spin_numlevel_->setMinimum(MINNUMLEVEL);
    spin_numlevel_->setMaximum(MAXNUMLEVEL);

    std::unique_ptr<QLabel> lbl_maxlevel{
        std::make_unique<QLabel>("Maximum level:", this)};
    lbl_maxlevel->setFont(*(g_p4app->boldFont_));
    spin_maxlevel_ = std::make_unique<QSpinBox>(this);
    spin_maxlevel_->setMinimum(MINMAXLEVEL);
    spin_maxlevel_->setMaximum(MAXMAXLEVEL);

    std::unique_ptr<QLabel> lbl_weakness{
        new QLabel("Maximum level of weakness:", this)};
    lbl_weakness->setFont(*(g_p4app->boldFont_));
    spin_weakness_ = std::make_unique<QSpinBox>(this);
    spin_weakness_->setMinimum(MINWEAKNESS);
    spin_weakness_->setMaximum(MAXWEAKNESS);

    lbl_p_ = std::make_unique<QLabel>("p:", this);
    lbl_p_->setFont(*(g_p4app->boldFont_));
    spin_p_ = std::make_unique<QSpinBox>(this);
    spin_p_->setMinimum(MINP);
    spin_p_->setMaximum(MAXP);

    lbl_q_ = std::make_unique<QLabel>("q:", this);
    lbl_q_->setFont(*(g_p4app->boldFont_));
    spin_q_ = std::make_unique<QSpinBox>(this);
    spin_q_->setMinimum(MINP);
    spin_q_->setMaximum(MAXP);

    lbl_x0_ = std::make_unique<QLabel>("x0:", this);
    lbl_x0_->setFont(*(g_p4app->boldFont_));
    edt_x0_ = std::make_unique<QLineEdit>("", this);

    lbl_y0_ = std::make_unique<QLabel>("y0:", this);
    lbl_y0_->setFont(*(g_p4app->boldFont_));
    edt_y0_ = std::make_unique<QLineEdit>("", this);

#ifdef TOOLTIPS
    btn_alg_->setToolTip(
        "Let the symbolic manipulator work in algebraic mode.\n"
        "The symbolic package will automatically switch over to\n"
        "numeric calculations whenever a symbolic study is impossible.");
    btn_num_->setToolTip("Let the symbolic manipulator work in numeric mode");
    btn_sepyes_->setToolTip(
        "Enable numeric testing of the Taylor developments "
        "for the separatrices");
    btn_sepno_->setToolTip("Disable separatrice testing");
    spin_precis_->setToolTip(
        "Number of digits required accuracy when checking for zero");
    spin_precis0_->setToolTip(
        "Number of digits of numeric precision of all "
        "calculations.  Put 0 for default double "
        "precision");
    edt_epsilon_->setToolTip(
        "Default epsilon for the separatrices.\n"
        "This may be changed later when plotting individual separatrices");
    spin_level_->setToolTip(
        "Order of Taylor approximation\nThis order is "
        "increased as long as the separatrix test fails.");
    spin_numlevel_->setToolTip(
        "Start working numerically when order of Taylor "
        "approximation reaches this level");
    spin_maxlevel_->setToolTip(
        "Maximum order of Taylor approximation.  Stop "
        "testing separatrices at this order.");
    spin_weakness_->setToolTip(
        "Maximum order of Lyapunov constants to be calculated for weak foci");
    spin_p_->setToolTip("Poincare-Lyapunov weights");
    spin_q_->setToolTip("Poincare-Lyapunov weights");
    edt_x0_->setToolTip(
        "Coordinates of the singular point in case of study of "
        "ONE singularity");
    edt_y0_->setToolTip(
        "Coordinates of the singular point in case of study of "
        "ONE singularity");
#endif

    // layout

    mainLayout_ = std::make_unique<QBoxLayout>(QBoxLayout::TopToBottom, this);

    mainLayout_->addWidget(p4title);

    std::unique_ptr<QHBoxLayout> symLayout{std::make_unique<QHBoxLayout>()};
    symLayout->addWidget(symlabel);
    symLayout->addWidget(btn_alg_);
    symLayout->addWidget(btn_num_);
    mainLayout_->addLayout(symLayout);

    std::unique_ptr<QHBoxLayout> sepLayout{std::make_unique<QHBoxLayout>()};
    sepLayout->addWidget(seplabel);
    sepLayout->addWidget(btn_sepyes_);
    sepLayout->addWidget(btn_sepno_);
    mainLayout_->addLayout(sepLayout);

    std::unique_ptr<QHBoxLayout> layout1{std::make_unique<QHBoxLayout>()};
    layout1->addWidget(lbl_precis);
    layout1->addWidget(spin_precis_);
    layout1->addStretch(0);
    layout1->addWidget(lbl_precis0);
    layout1->addWidget(spin_precis0_);

    std::unique_ptr<QHBoxLayout> layout2{std::make_unique<QHBoxLayout>()};
    layout2->addWidget(lbl_epsilon);
    layout2->addWidget(edt_epsilon_);
    layout2->addStretch(0);

    std::unique_ptr<QHBoxLayout> layout3{std::make_unique<QHBoxLayout>()};
    layout3->addWidget(lbl_level);
    layout3->addWidget(spin_level_);
    layout3->addStretch(0);

    std::unique_ptr<QHBoxLayout> layout4{std::make_unique<QHBoxLayout>()};
    layout4->addWidget(lbl_numlevel);
    layout4->addWidget(spin_numlevel_);
    layout4->addStretch(0);

    std::unique_ptr<QHBoxLayout> layout5{std::make_unique<QHBoxLayout>()};
    layout5->addWidget(lbl_maxlevel);
    layout5->addWidget(spin_maxlevel_);
    layout5->addStretch(0);

    std::unique_ptr<QHBoxLayout> layout6{std::make_unique<QHBoxLayout>()};
    layout6->addWidget(lbl_weakness);
    layout6->addWidget(spin_weakness_);
    layout6->addStretch(0);

    std::unique_ptr<QHBoxLayout> layout7{std::make_unique<QHBoxLayout>()};
    layout7->addWidget(lbl_p_);
    layout7->addWidget(spin_p_);
    layout7->addWidget(lbl_q_);
    layout7->addWidget(spin_q_);
    layout7->addStretch(0);

    std::unique_ptr<QHBoxLayout> layout8{std::make_unique<QHBoxLayout>()};
    layout8->addWidget(lbl_x0_);
    layout8->addWidget(edt_x0_);
    layout8->addStretch(0);

    std::unique_ptr<QHBoxLayout> layout9{std::make_unique<QHBoxLayout>()};
    layout9->addWidget(lbl_y0_);
    layout9->addWidget(edt_y0_);
    layout9->addStretch(0);

    mainLayout_->addLayout(layout1);
    mainLayout_->addLayout(layout2);
    mainLayout_->addLayout(layout3);
    mainLayout_->addLayout(layout4);
    mainLayout_->addLayout(layout5);
    mainLayout_->addLayout(layout6);
    mainLayout_->addLayout(layout7);
    mainLayout_->addLayout(layout8);
    mainLayout_->addLayout(layout9);

    setLayout(mainLayout_);

    btngrp_numeric_ = std::make_unique<QButtonGroup>(this);
    btngrp_numeric_->addButton(btn_alg_);
    btngrp_numeric_->addButton(btn_num_);

    btngrp_testsep_ = std::make_unique<QButtonGroup>(this);
    btngrp_testsep_->addButton(btn_sepyes_);
    btngrp_testsep_->addButton(btn_sepno_);

    // connections
    QObject::connect(btn_alg_, &QRadioButton::toggled, this,
                     &P4ParamsDlg::btn_alg_toggled);
    QObject::connect(btn_num_, &QRadioButton::toggled, this,
                     &P4ParamsDlg::btn_num_toggled);
    QObject::connect(
        spin_level_,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &P4ParamsDlg::onLevelChange);
    QObject::connect(btn_sepno_, &QRadioButton::toggled, this,
                     &P4ParamsDlg::btn_sepno_toggled);
    QObject::connect(btn_sepyes_, &QRadioButton::toggled, this,
                     &P4ParamsDlg::btn_sepyes_toggled);

    // finishing
    updateDlgData();
}

void P4ParamsDlg::onLevelChange(int value)
{
    /*
    spin_maxlevel_->setMinimum(value);
    spin_numlevel_->setMinimum(value);

    if (spin_maxlevel_->value() < value)
        spin_maxlevel_->setValue(value);

    if (spin_numlevel_->value() < value)
        spin_numlevel_->setValue(value);
    */
}

void P4ParamsDlg::btn_alg_toggled(bool on)
{
    if (on) {
        if (!g_ThisVF->hasCommonBool(g_ThisVF->numeric_) ||
            g_ThisVF->commonBool(g_ThisVF->numeric_) != false) {
            g_ThisVF->setCommonBool(g_ThisVF->numeric_, false);
            if (g_ThisVF->changed_ == false) {
                g_ThisVF->changed_ = true;
                g_p4app->signalChanged();
            }
        }
    }
}

void P4ParamsDlg::btn_num_toggled(bool on)
{
    if (on) {
        if (!g_ThisVF->hasCommonBool(g_ThisVF->numeric_) ||
            g_ThisVF->commonBool(g_ThisVF->numeric_) != true) {
            g_ThisVF->setCommonBool(g_ThisVF->numeric_, true);
            if (g_ThisVF->changed_ == false) {
                g_ThisVF->changed_ = true;
                g_p4app->signalChanged();
            }
        }
    }
}

void P4ParamsDlg::btn_sepyes_toggled(bool on)
{
    if (on) {
        if (!g_ThisVF->hasCommonBool(g_ThisVF->testsep_) ||
            g_ThisVF->commonBool(g_ThisVF->testsep_) != true) {
            g_ThisVF->setCommonBool(g_ThisVF->testsep_, true);
            if (g_ThisVF->changed_ == false) {
                g_ThisVF->changed_ = true;
                g_p4app->signalChanged();
            }
        }
    }
}

void P4ParamsDlg::btn_sepno_toggled(bool on)
{
    if (on) {
        if (!g_ThisVF->hasCommonBool(g_ThisVF->testsep_) ||
            g_ThisVF->commonBool(g_ThisVF->testsep_) != false) {
            g_ThisVF->setCommonBool(g_ThisVF->testsep_, false);
            if (g_ThisVF->changed_ == false) {
                g_ThisVF->changed_ = true;
                g_p4app->signalChanged();
            }
        }
    }
}

void P4ParamsDlg::setSpinBoxCommonValue(std::unique_ptr<QSpinBox> &sb,
                                        const std::vector<int> &val, int minval)
{
    if (g_ThisVF->hasCommonInt(val)) {
        sb->setSpecialValueText("");
        sb->setMinimum(minval);
        sb->setValue(g_ThisVF->commonInt(val));
    } else {
        sb->setMinimal(minval - 1);
        sb->setSpecialValueText("##");
        sb->setValue(minval - 1);
    }
}

bool P4ParamsDlg::getSpinBoxCommonValue(std::unique_ptr<QSpinBox> &sb,
                                        std::vector<int> &newval)
{
    int val{sb->value()};
    if (val == sb->minimum()) {
        if (!sb->specialValueText().isEmpty()) {
            // special value: ## -> ignore change
            return false;
        }
    }

    if (g_ThisVF->hasCommonInt(newval)) {
        if (val == g_ThisVF->commonInt(newval))
            return false;
    }

    g_ThisVF->setCommonInt(newval, val);
    return true;
}

void P4ParamsDlg::setLineEditCommonValue(std::unique_ptr<QLineEdit> &le,
                                         const std::vector<QString> &val)
{
    if (g_ThisVF->hasCommonString(val))
        le->setText(g_ThisVF->commonString(val));
    else
        le->setText("##");
}

bool P4ParamsDlg::getLineEditCommonValue(std::unique_ptr<QLineEdit> &le,
                                         std::vector<QString> &newval)
{
    QString val{le->text().trimmed()};

    if (!g_ThisVF->hasCommonString(newval)) {
        for (int i = val.length() - 1; i >= 0; i--) {
            if (val[i] != '#')
                break;
            if (i == 0)
                return false;
        }
        if (!val.compare(g_ThisVF->commonString(newval)))
            return false;
    }

    if (!val.compare(g_ThisVF->commonString(newval)))
        return false;

    g_ThisVF->setCommonString(newval, val);
    return true;
}

void P4ParamsDlg::getDataFromDlg()
{
    QString epsilon;
    QString x0;
    QString y0;
    bool changed{false};

    if (spin_p_->value() != g_ThisVF->p_ || spin_q_->value() != g_ThisVF->q_) {
        g_ThisVF->p_ = spin_p_->value();
        g_ThisVF->q_ = spin_q_->value();
        changed = true;
    }

    x0 = edt_x0_->text().trimmed();
    y0 = edt_y0_->text().trimmed();

    if (x0.compare(g_ThisVF->x0_) != 0 || y0.compare(g_ThisVF->y0_) != 0) {
        g_ThisVF->x0_ = x0;
        g_ThisVF->y0_ = y0;
        changed = true;
    }

    changed |= getSpinBoxCommonValue(spin_level_, g_ThisVF->taylorlevel_);
    changed |= getSpinBoxCommonValue(spin_numlevel_, g_ThisVF->numericlevel_);
    changed |= getSpinBoxCommonValue(spin_maxlevel_, g_ThisVF->maxlevel_);
    changed |= getSpinBoxCommonValue(spin_weakness_, g_ThisVF->weakness_);
    changed |= getSpinBoxCommonValue(spin_precis_, g_ThisVF->precision_);
    changed |= getSpinBoxCommonValue(spin_precis0_, g_ThisVF->precision0_);
    changed |= getLineEditCommonValue(edt_epsilon_, g_ThisVF->epsilon_);

    if (changed) {
        if (g_ThisVF->changed == false) {
            g_ThisVF->changed = true;
            g_p4app->signalChanged();
        }
    }
}

void P4ParamsDlg::updateDlgData()
{
    if (g_ThisVF->hasCommonBool(g_ThisVF->numeric_)) {
        if (g_ThisVF->commonBool(g_ThisVF->numeric_))
            btn_num_->toggle();
        else
            btn_alg_->toggle();
    } else {
        btngrp_numeric_->setExclusive(false);
        btn_num_->setChecked(false);
        btn_alg_->setChecked(false);
        btngrp_numeric_->setExclusive(true);
    }

    if (g_ThisVF->hasCommonBool(g_ThisVF->testsep_)) {
        if (g_ThisVF->commonBool(g_ThisVF->testsep_))
            btn_sepyes_->toggle();
        else
            btn_sepno_->toggle();
    } else {
        btngrp_testsep_->setExclusive(false);
        btn_sepyes_->setChecked(false);
        btn_sepno_->setChecked(false);
        btngrp_testsep_->setExclusive(true);
    }

    setSpinBoxCommonValue(spin_level_, g_ThisVF->taylorlevel_, MINLEVEL);
    setSpinBoxCommonValue(spin_numlevel_, g_ThisVF->numericlevel_, MINNUMLEVEL);
    setSpinBoxCommonValue(spin_maxlevel_, g_ThisVF->maxlevel_, MINMAXLEVEL);
    setSpinBoxCommonValue(spin_weakness_, g_ThisVF->weakness_, MINWEAKNESS);
    setLineEditCommonValue(edt_epsilon_, g_ThisVF->epsilon_);
    setSpinBoxCommonValue(spin_precis_, g_ThisVF->precision_, MINPRECISION);
    setSpinBoxCommonValue(spin_precis0_, g_ThisVF->precision0_, MINPRECISION0);

    if (g_ThisVF->typeofstudy_ == TYPEOFSTUDY_ONE) {
        edt_x0_->setText(g_ThisVF->x0_);
        edt_y0_->setText(g_ThisVF->y0_);
        spin_p_->setValue(g_ThisVF->p_);
        spin_q_->setValue(g_ThisVF->q_);

        spin_p_->setEnabled(false);
        spin_q_->setEnabled(false);
        edt_x0_->setEnabled(true);
        edt_y0_->setEnabled(true);

        lbl_x0_->setFont(*(g_p4app->boldFont_));
        lbl_y0_->setFont(*(g_p4app->boldFont_));
        lbl_p_->setFont(*(g_p4app->standardFont_));
        lbl_q_->setFont(*(g_p4app->standardFont_));
    } else {
        spin_p_->setValue(g_ThisVF->p_);
        spin_q_->setValue(g_ThisVF->q_);
        edt_x0_->setText(g_ThisVF->x0_);
        edt_y0_->setText(g_ThisVF->y0_);

        spin_p_->setEnabled(true);
        spin_q_->setEnabled(true);
        edt_x0_->setEnabled(false);
        edt_y0_->setEnabled(false);

        lbl_p_->setFont(*(g_p4app->boldFont_));
        lbl_q_->setFont(*(g_p4app->boldFont_));
        lbl_x0_->setFont(*(g_p4app->standardFont_));
        lbl_y0_->setFont(*(g_p4app->standardFont_));
    }
}