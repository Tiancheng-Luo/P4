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

#include "win_find.h"

#include "file_vf.h"

#include "main.h"
#include "p4application.h"
#include "p4settings.h"
#include "win_params.h"
#include "win_vf.h"

#include <QButtonGroup>
#include <QLabel>
#include <QMessageBox>

QFindDlg::~QFindDlg()
{
    getDataFromDlg();

    if (vfWindow_ != nullptr) {
        delete vfWindow_;
        vfWindow_ = nullptr;
    }
    if (paramsWindow_ != nullptr) {
        delete paramsWindow_;
        paramsWindow_ = nullptr;
    }
}

QFindDlg::QFindDlg(QStartDlg *startdlg)
#ifdef DOCK_FINDWINDOW
    : QWidget(startdlg)
#else
    : QWidget()
#endif
{
    parent_ = startdlg;
//  setFont( QFont( FONTSTYLE, FONTSIZE ) );

#ifdef DOCK_FINDWINDOW
    QLabel *p4title = new QLabel("Find and Examine Singular Points", this);
    p4title->setFont(*(p4app->titleFont_));
#endif

    // QLabel *symlabel = new QLabel("Symbolic package: ", this);
    // symlabel->setFont(*(p4app->boldFont_));
    // btn_maple = new QRadioButton("Maple", this);
    // btn_reduce = new QRadioButton("Reduce", this);

    //#ifdef Q_OS_WIN
    // btn_reduce->setEnabled(false); // reduce is not available under windows
    //#endif

    QLabel *actlabel = new QLabel("File Action:  ", this);
    actlabel->setFont(*(p4app->boldFont_));
    btn_actionrun_ = new QRadioButton("Run File", this);
    btn_actionprep_ = new QRadioButton("Prepare File", this);

    QLabel *singpoints = new QLabel("Singular points:  \n\n", this);
    singpoints->setFont(*(p4app->boldFont_));
    btn_all_ = new QRadioButton("All ", this);
    btn_fin_ = new QRadioButton("Finite ", this);
    btn_inf_ = new QRadioButton("Infinite ", this);
    btn_one_ = new QRadioButton("One ", this);

    QLabel *saveall = new QLabel("Save all information: ", this);
    saveall->setFont(*(p4app->boldFont_));
    btn_yes_ = new QRadioButton("Yes", this);
    btn_no_ = new QRadioButton("No", this);

    btn_params_ = new QPushButton("P&arameters", this);
    btn_vf_ = new QPushButton("&Vector Field", this);

    btn_load_ = new QPushButton("&Load", this);
    btn_save_ = new QPushButton("&Save", this);

    if (action_OnlyPrepareFile)
        btn_eval_ = new QPushButton("Pr&epare", this);
    else
        btn_eval_ = new QPushButton("&Evaluate", this);

#ifdef TOOLTIPS
    // btn_maple->setToolTip("Select Maple as the symbolic manipulator");
    // btn_reduce->setToolTip("Select Reduce as the symbolic manipulator.\n"
    //                       "This is only available in the Unix version.");
    btn_actionrun_->setToolTip(
        "Make sure the symbolic manipulator processes the file");
    btn_actionprep_->setToolTip(
        "Do not process the file, only prepare it.\n"
        "Choose this if you want to process the file yourselves\n"
        "from within a terminal window.");
    btn_all_->setToolTip("Study all singularities, finite and infinite");
    btn_fin_->setToolTip("Study only singularities at the finite region");
    btn_inf_->setToolTip("Study only singularities at infinity");
    btn_one_->setToolTip("Study one specific singularity.\n"
                        "Do not forget to specify the coordinates.");
    btn_yes_->setToolTip(
        "The View/Finite and View/Infinite windows will display more details");
    btn_no_->setToolTip(
        "The View/Finite and View/Infinite windows will display no details");
    btn_params_->setToolTip("Opens/closes the parameters window");
    btn_vf_->setToolTip("Opens/closes the vector field window");
    btn_load_->setToolTip("Load the vector field & parameters from disc");
    btn_save_->setToolTip("Save the vector field & parameters to disc");
    btn_eval_->setToolTip("Prepare a file for the symbolic manipulator, and "
                         "optionally process it");
#endif

// layout

#ifdef DOCK_PARAMSWINDOW
    superLayout_ = new QBoxLayout(QBoxLayout::LeftToRight, this);
    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom);
#else
    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);
#endif

#ifdef DOCK_FINDWINDOW
    mainLayout_->addSpacing(8);
    mainLayout_->addWidget(p4title);
#endif

    QHBoxLayout *symLayout = new QHBoxLayout();
    // symLayout->addWidget(symlabel);
    // symLayout->addWidget(btn_maple);
    // symLayout->addWidget(btn_reduce);
    symLayout->addStretch(0);
    mainLayout_->addLayout(symLayout);

    QHBoxLayout *actLayout = new QHBoxLayout();
    actLayout->addWidget(actlabel);
    actLayout->addWidget(btn_actionrun_);
    actLayout->addWidget(btn_actionprep_);
    actLayout->addStretch(0);
    mainLayout_->addLayout(actLayout);

    QHBoxLayout *singlineLayout = new QHBoxLayout();
    singlineLayout->addWidget(singpoints, 0, Qt::AlignBottom);

    QGridLayout *singLayout = new QGridLayout();
    singLayout->addWidget(btn_all_, 0, 0);
    singLayout->addWidget(btn_fin_, 0, 1);
    singLayout->addWidget(btn_inf_, 1, 0);
    singLayout->addWidget(btn_one_, 1, 1);
    singlineLayout->addLayout(singLayout);
    singlineLayout->addStretch(0);

    mainLayout_->addLayout(singlineLayout);

    QHBoxLayout *layout0 = new QHBoxLayout();
    layout0->addWidget(saveall);
    layout0->addWidget(btn_yes_);
    layout0->addWidget(btn_no_);
    layout0->addStretch(0);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->addStretch(0);
    layout1->addWidget(btn_params_);
    layout1->addWidget(btn_vf_);
    layout1->addStretch(0);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addStretch(0);
    layout2->addWidget(btn_load_);
    layout2->addWidget(btn_save_);
    layout2->addStretch(0);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addStretch(0);
    layout3->addWidget(btn_eval_);
    layout3->addStretch(0);

    mainLayout_->addLayout(layout0);
    mainLayout_->addLayout(layout1);
    mainLayout_->addLayout(layout2);
    mainLayout_->addLayout(layout3);

//   mainLayout_->setSizeConstraint(QLayout::SetFixedSize);

#ifdef DOCK_PARAMSWINDOW
    mainLayout_->addStretch(0);
    superLayout_->addLayout(mainLayout_);
    setLayout(superLayout_);
#else
    setLayout(mainLayout_);
#endif

    // connections

    QButtonGroup *btngrp1 = new QButtonGroup(this);
    btngrp1->addButton(btn_all_);
    btngrp1->addButton(btn_fin_);
    btngrp1->addButton(btn_inf_);
    btngrp1->addButton(btn_one_);

    QButtonGroup *btngrp2 = new QButtonGroup(this);
    btngrp2->addButton(btn_yes_);
    btngrp2->addButton(btn_no_);

    // QButtonGroup *btngrp3 = new QButtonGroup(this);
    // btngrp3->addButton(btn_maple);
    // btngrp3->addButton(btn_reduce);

    QButtonGroup *btngrp4 = new QButtonGroup(this);
    btngrp4->addButton(btn_actionrun_);
    btngrp4->addButton(btn_actionprep_);

    // if (ThisVF->symbolicpackage_ == PACKAGE_MAPLE)
    //    btn_maple->toggle();
    // else
    //    btn_reduce->toggle();

    if (action_OnlyPrepareFile)
        btn_actionprep_->toggle();
    else
        btn_actionrun_->toggle();

    switch (ThisVF->typeofstudy_) {
    case TYPEOFSTUDY_ALL:
        btn_all_->toggle();
        break;
    case TYPEOFSTUDY_FIN:
        btn_fin_->toggle();
        break;
    case TYPEOFSTUDY_INF:
        btn_inf_->toggle();
        break;
    case TYPEOFSTUDY_ONE:
        btn_one_->toggle();
        break;
    }

    if (action_SaveAll)
        btn_yes_->toggle();
    else
        btn_no_->toggle();

    // QObject::connect(btn_maple, SIGNAL(toggled(bool)), this,
    //                 SLOT(btn_maple_toggled(bool)));
    // QObject::connect(btn_reduce, SIGNAL(toggled(bool)), this,
    //                 SLOT(btn_reduce_toggled(bool)));
    QObject::connect(btn_actionrun_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_actionrun_toggled(bool)));
    QObject::connect(btn_actionprep_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_actionprep_toggled(bool)));
    QObject::connect(btn_all_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_all_toggled(bool)));
    QObject::connect(btn_one_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_one_toggled(bool)));
    QObject::connect(btn_fin_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_fin_toggled(bool)));
    QObject::connect(btn_inf_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_inf_toggled(bool)));
    QObject::connect(btn_yes_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_yes_toggled(bool)));
    QObject::connect(btn_no_, SIGNAL(toggled(bool)), this,
                     SLOT(btn_no_toggled(bool)));

    QObject::connect(btn_params_, SIGNAL(clicked()), this, SLOT(onBtnParams()));
    QObject::connect(btn_vf_, SIGNAL(clicked()), this, SLOT(onBtnVf()));
    QObject::connect(btn_load_, SIGNAL(clicked()), this, SLOT(onBtnLoad()));
    QObject::connect(btn_save_, SIGNAL(clicked()), this, SLOT(onBtnSave()));
    QObject::connect(btn_eval_, SIGNAL(clicked()), this, SLOT(onBtnEval()));

    // finishing

    vfWindow_ = nullptr;
    paramsWindow_ = nullptr;

#ifdef AUTO_OPEN_VFWINDOW
    onBtnVf();
#endif

#ifdef AUTO_OPEN_PARAMSWINDOW
    onBtnParams();
#endif

    if (ThisVF->evaluating_)
        btn_eval_->setEnabled(false);

#ifndef DOCK_FINDWINDOW
    SetP4WindowTitle(this, "Find");
#endif
}

void QFindDlg::btn_yes_toggled(bool on)
{
    if (on) {
        action_SaveAll = true;
    }
}

void QFindDlg::btn_no_toggled(bool on)
{
    if (on) {
        action_SaveAll = false;
    }
}

void QFindDlg::btn_maple_toggled(bool on)
{
    if (on) {
        if (ThisVF->symbolicpackage_ != PACKAGE_MAPLE) {
            ThisVF->symbolicpackage_ = PACKAGE_MAPLE;
            setMathPackage(PACKAGE_MAPLE);
        }
    }
}

void QFindDlg::btn_reduce_toggled(bool on)
{
    if (on) {
        if (ThisVF->symbolicpackage_ != PACKAGE_REDUCE) {
            ThisVF->symbolicpackage_ = PACKAGE_REDUCE;
            setMathPackage(PACKAGE_REDUCE);
        }
    }
}

void QFindDlg::btn_actionrun_toggled(bool on)
{
    if (on) {
        action_OnlyPrepareFile = false;
        btn_eval_->setText("&Evaluate");
    }
}

void QFindDlg::btn_actionprep_toggled(bool on)
{
    if (on) {
        action_OnlyPrepareFile = true;
        btn_eval_->setText("Pr&epare");
    }
}

void QFindDlg::btn_all_toggled(bool on)
{
    if (on) {
        if (ThisVF->typeofstudy_ != TYPEOFSTUDY_ALL) {
            ThisVF->typeofstudy_ = TYPEOFSTUDY_ALL;
            if (ThisVF->changed_ == false) {
                ThisVF->changed_ = true;
                p4app->signalChanged();
            }
            if (paramsWindow_ != nullptr) {
                paramsWindow_->getDataFromDlg();
                paramsWindow_->updateDlgData();
            }
        }
    }
}

void QFindDlg::btn_fin_toggled(bool on)
{
    if (on) {
        if (ThisVF->typeofstudy_ != TYPEOFSTUDY_FIN) {
            ThisVF->typeofstudy_ = TYPEOFSTUDY_FIN;
            if (ThisVF->changed_ == false) {
                ThisVF->changed_ = true;
                p4app->signalChanged();
            }
            if (paramsWindow_ != nullptr) {
                paramsWindow_->getDataFromDlg();
                paramsWindow_->updateDlgData();
            }
        }
    }
}

void QFindDlg::btn_inf_toggled(bool on)
{
    if (on) {
        if (ThisVF->typeofstudy_ != TYPEOFSTUDY_INF) {
            ThisVF->typeofstudy_ = TYPEOFSTUDY_INF;
            if (ThisVF->changed_ == false) {
                ThisVF->changed_ = true;
                p4app->signalChanged();
            }
            if (paramsWindow_ != nullptr) {
                paramsWindow_->getDataFromDlg();
                paramsWindow_->updateDlgData();
            }
        }
    }
}

void QFindDlg::btn_one_toggled(bool on)
{
    if (on) {
        if (ThisVF->typeofstudy_ != TYPEOFSTUDY_ONE) {
            ThisVF->typeofstudy_ = TYPEOFSTUDY_ONE;
            if (ThisVF->changed_ == false) {
                ThisVF->changed_ = true;
                p4app->signalChanged();
            }
            if (paramsWindow_ != nullptr) {
                paramsWindow_->getDataFromDlg();
                paramsWindow_->updateDlgData();
            }
        }
    }
}

void QFindDlg::exclusiveToggle(bool on, QRadioButton *first, ...)
{
    first->setChecked(on);
}

void QFindDlg::onBtnParams(void)
{
    if (paramsWindow_ == nullptr) {
        paramsWindow_ = new QParamsDlg(this);
        paramsWindow_->show();
#ifdef DOCK_PARAMSWINDOW
        superLayout_->addWidget(paramsWindow_, 0, Qt::AlignTop);
#else
        paramsWindow_->raise();
#endif
    } else {
#ifdef DOCK_PARAMSWINDOW
        delete paramsWindow_;
        paramsWindow_ = nullptr;
#else
        paramsWindow_->show();
        paramsWindow_->raise();
#endif
    }
}

void QFindDlg::onBtnVf(void)
{
    // show find dialog

    if (vfWindow_ == nullptr) {
        vfWindow_ = new QVectorFieldDlg(this);
#ifdef DOCK_VFWINDOW
        vfWindow_->show();
        mainLayout_->addWidget(vfWindow_);
#else
        vfWindow_->raise();
#endif
    } else {
#ifdef DOCK_VFWINDOW
        delete vfWindow_;
        vfWindow_ = nullptr;
#else
        vfWindow_->show();
        vfWindow_->raise();
#endif
    }
}

void QFindDlg::onBtnLoad(void)
{
    if (ThisVF->load() == false) {
        QMessageBox::critical(this, "P4",
                              "Unable to find the input vector field.\n");
    } else {
        // we first signal that the vector field has changed
        // to invalidate all open windows.

        bool oldeval = ThisVF->evaluated_;

        updateDlgData();
        if (ThisVF->changed_ == false) {
            ThisVF->changed_ = true;
            p4app->signalChanged();
        }

        // then, we signal that the vector field is unchanged
        // because a save vector field is not needed when quiting.

        ThisVF->changed_ = false;
        ThisVF->evaluated_ = oldeval;
        p4app->signalLoaded();
    }
}

void QFindDlg::onBtnSave(void)
{
    getDataFromDlg();
    if (ThisVF->changed_)
        ThisVF->cleared_ = false;
    if (ThisVF->cleared_) {
        QMessageBox::critical(this, "P4", "No data has been entered.\n"
                                          "Please specify vector field.\n");
        return;
    }
    if (ThisVF->save() == false) {
        QMessageBox::critical(
            this, "P4", "Unable to save the input vector field.\n"
                        "Please check permissions on the write location.\n");
    }
}

void QFindDlg::onBtnEval(void)
{
    //  int result;

    getDataFromDlg();
    if (ThisVF->getfilename().length() == 0) {
        // error: need to specify filename first

        QMessageBox::critical(this, "P4", "Unable to evaluate.\n"
                                          "Please specify a valid filename.\n");
        return;
    }

    if (ThisVF->changed_)
        ThisVF->cleared_ = false;

    if (ThisVF->cleared_) {
        QMessageBox::critical(this, "P4", "No data has been entered.\n"
                                          "Please specify vector field.\n");
        return;
    }

    /*if (ThisVF->changed_) {
      ThisVF->evaluated_ = false;
        result = QMessageBox::information( this, "P4",
                    "The vector field has been changed since "
                    "the last save.\nSave before evaluation?",
                    "&Yes", "&No", "&Cancel",
                    0, 2 );
        if( result == 2 )
        {
            return;
        }
        if( result == 0 )
        {
            if( ThisVF->save() == false )
            {
                QMessageBox::critical( this, "P4",
                            "Unable to save the input vector field.\n"
                            "Please check permissions on the write location.\n"
                                  );
                return;
            }
        }
    }*/

    if (action_OnlyPrepareFile) {
        ThisVF->evaluated_ = false;
        ThisVF->evaluating_ = false;

        ThisVF->prepare();
    } else {
        ThisVF->evaluated_ = false;
        ThisVF->evaluating_ = true;

        signalEvaluating();
        ThisVF->evaluate();
        //      RunTask( &EvalVF );
    }
}

void QFindDlg::getDataFromDlg(void)
{
    if (vfWindow_ != nullptr) {
        vfWindow_->getDataFromDlg();
    }
    if (paramsWindow_ != nullptr) {
        paramsWindow_->getDataFromDlg();
    }
}

void QFindDlg::updateDlgData(void)
{
    switch (ThisVF->typeofstudy_) {
    case TYPEOFSTUDY_ALL:
        exclusiveToggle(true, btn_all_);
        break;
    case TYPEOFSTUDY_FIN:
        exclusiveToggle(true, btn_fin_);
        break;
    case TYPEOFSTUDY_INF:
        exclusiveToggle(true, btn_inf_);
        break;
    case TYPEOFSTUDY_ONE:
        exclusiveToggle(true, btn_one_);
        break;
    }

    if (vfWindow_ != nullptr) {
        vfWindow_->updateDlgData();
    }
    if (paramsWindow_ != nullptr) {
        paramsWindow_->updateDlgData();
    }
}

void QFindDlg::signalEvaluating(void) { btn_eval_->setEnabled(false); }

void QFindDlg::signalEvaluated(void) { btn_eval_->setEnabled(true); }
