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

#ifndef WIN_VF_H
#define WIN_VF_H

#include "custom.h"
#include "win_find.h"

#include <QBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScrollBar>
#include <QSpinBox>
#include <QWidget>

class QVFParams; // declare them first because both classes defined in
                 // this file need each other

class QVectorFieldDlg : public QWidget
{
    Q_OBJECT

  public:
    QVectorFieldDlg(QFindDlg *startwindow);
    void getDataFromDlg(void);
    void updateDlgData(void);
    ~QVectorFieldDlg();

  private:
    QBoxLayout *mainLayout_;
    QFindDlg *parent;
    QLineEdit *edt_xprime;
    QLineEdit *edt_yprime;
    QLineEdit *edt_gcf;
    QSpinBox *spin_numparams;
    QHBoxLayout *paramLayout;
    QScrollBar *sb_params;
    QVFParams *params;

  public slots:
    void numParamsChanged(int);
};

class QVFParams : public QWidget
{
    Q_OBJECT

  public:
    QVFParams(QVectorFieldDlg *parent, QScrollBar *sb);
    ~QVFParams();
    bool getDataFromDlg(void);
    bool updateDlgData(void);

  private:
    bool datainvalid;
    int currentnumparams;
    int currentshownparams;
    int currentpageindex;

    QScrollBar *sb_params;
    QBoxLayout *mainLayout_;
    QBoxLayout *superLayout;
    QHBoxLayout *Param_Lines[MAXNUMPARAMSSHOWN];
    QLineEdit *Param_Names[MAXNUMPARAMSSHOWN];
    QLabel *Param_Labels[MAXNUMPARAMSSHOWN];
    QLineEdit *Param_Values[MAXNUMPARAMSSHOWN];

    QString S_Labels[MAXNUMPARAMS];
    QString S_Values[MAXNUMPARAMS];

  protected:
    bool focusNextPrevChild(bool next);

  public slots:
    void paramsSliderChanged(int);
};

#endif /* WIN_VF_H */
