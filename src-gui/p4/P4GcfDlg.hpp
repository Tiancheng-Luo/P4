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

#include <QWidget>

class QBoxLayout;
class QLineEdit;
class QPushButton;
class QRadioButton;

class P4Sphere;
class P4PlotWnd;

class P4GcfDlg : public QWidget
{
    Q_OBJECT

  public:
    P4GcfDlg(P4PlotWnd *, P4Sphere *);
    P4GcfDlg(const P4GcfDlg *other)
        : P4GcfDlg(other->plotwnd_, other->mainSphere_)
    {
    }

    void reset();
    void finishGcfEvaluation();

  private:
    P4PlotWnd *plotwnd_;
    P4Sphere *mainSphere_;

    QPushButton *btn_evaluate_;

    QRadioButton *btn_dots_;
    QRadioButton *btn_dashes_;
    QLineEdit *edt_points_;
    QLineEdit *edt_precis_;
    QLineEdit *edt_memory_;

    QBoxLayout *mainLayout_;

    int evaluating_points_;
    int evaluating_memory_;
    int evaluating_precision_;

  public slots:
    void onbtn_evaluate();
};
