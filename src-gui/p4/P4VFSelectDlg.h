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
#include <memory>

class P4FindDlg;
class QBoxLayout;
class QHBoxLayout;
class QLabel;
class QComboBox;
class QPushButton;
class P4SeparatingCurvesDlg;

class P4VFSelectDlg : public QWidget
{
    Q_OBJECT
  public:
    P4VFSelectDlg(std::unique_ptr<P4FindDlg> startwindow);

    std::unique_ptr<QComboBox> cbb_vfselect_;
    std::unique_ptr<QPushButton> btn_add_;
    std::unique_ptr<QPushButton> btn_del_;
    std::unique_ptr<QPushButton> btn_prev_;
    std::unique_ptr<QPushButton> btn_next_;
    std::unique_ptr<QPushButton> btn_p5config_;
    std::unique_ptr<P4SeparatingCurvesDlg> win_curves_;

    void getDataFromDlg(void);
    void updateDlgData(void);

    void closeConfigWindow(void);
    bool checkPlotWindowClosed(void);

  public slots:
    void onBtnAdd(void);
    void onBtnDel(void);
    void onBtnPrev(void);
    void onBtnNext(void);
    void onBtnP5Config(void);
    void onVfSelectionChanged(int);

  private:
    std::unique_ptr<QBoxLayout> mainLayout_;
    std::unique_ptr<P4FindDlg> parent_;
};
