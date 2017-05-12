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

#include "win_separatrice.h"

#include "custom.h"
#include "math_separatrice.h"

QString g_CurrentSingularityInfo[4] = {"", "", "", ""};
double g_CurrentSeparatriceEpsilon = 0;

QSepDlg::QSepDlg(QPlotWnd *plt, QWinSphere *sp)
    : QWidget(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint)
{
    //  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    mainSphere_ = sp;
    plotwnd_ = plt;

    btn_start = new QPushButton("&Start integrating sep", this);
    btn_cont = new QPushButton("&Cont integrating sep", this);
    btn_intnext = new QPushButton("&Integrate next sep", this);
    btn_selectnext = new QPushButton("Select &Next sep", this);

    edt_epsilon = new QLineEdit("", this);
    QLabel *lbl1 = new QLabel("&Epsilon = ", this);
    lbl1->setBuddy(edt_epsilon);

    lbl_info[0] = new QLabel("", this);
    lbl_info[1] = new QLabel("", this);
    lbl_info[2] = new QLabel("", this);
    lbl_info[3] = new QLabel("", this);

    // layout

    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);

    mainLayout_->addWidget(lbl_info[0]);
    mainLayout_->addWidget(lbl_info[1]);
    mainLayout_->addWidget(lbl_info[2]);
    mainLayout_->addWidget(lbl_info[3]);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addWidget(lbl1);
    layout3->addWidget(edt_epsilon);

    QHBoxLayout *layout4 = new QHBoxLayout();
    layout4->addWidget(btn_start);
    layout4->addWidget(btn_cont);

    QHBoxLayout *layout5 = new QHBoxLayout();
    layout5->addWidget(btn_intnext);
    layout5->addWidget(btn_selectnext);

    mainLayout_->addLayout(layout3);
    mainLayout_->addLayout(layout4);
    mainLayout_->addLayout(layout5);

    mainLayout_->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout_);

#ifdef TOOLTIPS
    edt_epsilon->setToolTip("Epsilon value for this separatrix\n"
                            "Confirm any change by pressing ENTER");
    btn_start->setToolTip("Start integrating this separatrix");
    btn_cont->setToolTip("Continue integrating this separatrix");
    btn_intnext->setToolTip("Select next separatrix, and start integrating");
    btn_selectnext->setToolTip("Select next separatrix of this singular point");
#endif

    // connections

    QObject::connect(btn_selectnext, SIGNAL(clicked()), this,
                     SLOT(onbtn_selectnext()));
    QObject::connect(btn_intnext, SIGNAL(clicked()), this,
                     SLOT(onbtn_intnext()));
    QObject::connect(btn_start, SIGNAL(clicked()), this, SLOT(onbtn_start()));
    QObject::connect(btn_cont, SIGNAL(clicked()), this, SLOT(onbtn_cont()));
    QObject::connect(edt_epsilon, SIGNAL(returnPressed()), this,
                     SLOT(onepsilon_enter()));

    // finishing

    reset();

    setP4WindowTitle(this, "Separatrices");
}

void QSepDlg::setInitialPoint(void)
{
    QString buf;

    plotwnd_->getDlgData();

    selected = true;
    started = false;
    btn_start->setEnabled(true);
    btn_cont->setEnabled(false);
    btn_selectnext->setEnabled(true);
    btn_intnext->setEnabled(true);

    lbl_info[0]->setText(g_CurrentSingularityInfo[0]);
    lbl_info[1]->setText(g_CurrentSingularityInfo[1]);
    lbl_info[2]->setText(g_CurrentSingularityInfo[2]);
    lbl_info[3]->setText(g_CurrentSingularityInfo[3]);

    buf.sprintf("%g", (float)g_CurrentSeparatriceEpsilon);

    edt_epsilon->setText(buf);
    show();
    //  raise();
}

void QSepDlg::onbtn_selectnext(void)
{
    if (!selected)
        return;

    plotwnd_->getDlgData();

    started = false;

    btn_start->setEnabled(true);
    btn_cont->setEnabled(false);
    btn_selectnext->setEnabled(true);
    btn_intnext->setEnabled(true);

    mainSphere_->prepareDrawing();
    (*select_next_sep)(mainSphere_);
    mainSphere_->finishDrawing();
}

void QSepDlg::onbtn_intnext(void)
{
    if (!selected)
        return;

    plotwnd_->getDlgData();

    started = true;

    btn_start->setEnabled(false);
    btn_cont->setEnabled(true);
    btn_selectnext->setEnabled(true);
    btn_intnext->setEnabled(true);

    mainSphere_->prepareDrawing();
    (*plot_next_sep)(mainSphere_);
    mainSphere_->finishDrawing();
}

void QSepDlg::onbtn_start(void)
{
    if (!selected || started)
        return;

    plotwnd_->getDlgData();

    started = true;
    btn_start->setEnabled(false);
    btn_cont->setEnabled(true);
    btn_selectnext->setEnabled(true);
    btn_intnext->setEnabled(true);

    mainSphere_->prepareDrawing();
    (*start_plot_sep)(mainSphere_);
    mainSphere_->finishDrawing();
}

void QSepDlg::onbtn_cont(void)
{
    if (!selected || !started)
        return;

    plotwnd_->getDlgData();

    mainSphere_->prepareDrawing();
    (*cont_plot_sep)(mainSphere_);
    mainSphere_->finishDrawing();
}

void QSepDlg::reset(void)
{
    lbl_info[0]->setText("no point selected.");
    lbl_info[1]->setText("");
    lbl_info[2]->setText("Use Shift+Left Button");
    lbl_info[3]->setText("in the plot window to select.");

    started = false;
    selected = false;

    btn_start->setEnabled(false);
    btn_cont->setEnabled(false);
    btn_selectnext->setEnabled(false);
    btn_intnext->setEnabled(false);
}

void QSepDlg::SepEvent(int i)
{
    switch (i) {
    case -1:
        setInitialPoint();
        break;
    case 0:
        onbtn_cont();
        break;
    case 1:
        onbtn_start();
        break;
    case 2:
        onbtn_intnext();
        break;
    case 3:
        onbtn_selectnext();
        break;
    }
}

void QSepDlg::markBad(QLineEdit *edt)
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

void QSepDlg::onepsilon_enter(void)
{
    // called when user presses ENTER after changing the epsilon value

    QString s;
    double eps;
    bool ok;

    s = edt_epsilon->text();
    eps = s.toDouble(&ok);
    if (!ok || eps <= 0) {
        markBad(edt_epsilon);
        return;
    }

    g_CurrentSeparatriceEpsilon = eps;
    s.sprintf("%g", (float)eps);
    edt_epsilon->setText(s);

    // pass on to math routines

    if (!selected)
        return;

    started = false;

    btn_start->setEnabled(true);
    btn_cont->setEnabled(false);
    btn_selectnext->setEnabled(true);
    btn_intnext->setEnabled(true);

    mainSphere_->prepareDrawing();
    (*change_epsilon)(mainSphere_, eps);
    mainSphere_->finishDrawing();
}
