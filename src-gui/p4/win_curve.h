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

#ifndef WIN_CURVE_H
#define WIN_CURVE_H

#include <QWidget>

class P4PlotWnd;
class P4WinSphere;

class QPushButton;
class QRadioButton;
class QLineEdit;
class QBoxLayout;

class QCurveDlg : public QWidget
{
    Q_OBJECT

  public:
    QCurveDlg(P4PlotWnd *, P4WinSphere *);
    void reset(void);
    void finishCurveEvaluation(void);

  private:
    P4WinSphere *mainSphere_;
    P4PlotWnd *plotwnd_;

    QPushButton *btnEvaluate_;
    QPushButton *btnPlot_;
    QPushButton *btnDelLast_;
    QPushButton *btnDelAll_;

    QRadioButton *btn_dots_;
    QRadioButton *btn_dashes_;
    QLineEdit *edt_curve_;
    QLineEdit *edt_points_;
    QLineEdit *edt_precis_;
    QLineEdit *edt_memory_;

    QBoxLayout *mainLayout_;

    int evaluating_points_;
    int evaluating_memory_;
    int evaluating_precision_;

  public slots:
    void onBtnEvaluate(void);
    void onBtnPlot(void);
    void onBtnDelAll(void);
    void onBtnDelLast(void);
};

#endif /* WIN_CURVE_H */
