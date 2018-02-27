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

class QBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QString;
class QCheckBox;

class P4ViewDlg : public QWidget
{
    Q_OBJECT

  public:
    P4ViewDlg(bool virtualchk);
    ~P4ViewDlg();
    bool getDataFromDlg();
    void updateDlgData();

  private:
    bool changed_;
    bool haveVirtualCheckBox_;

    std::unique_ptr<QBoxLayout> mainLayout_;

    std::unique_ptr<QRadioButton> btn_sphere_;
    std::unique_ptr<QRadioButton> btn_plane_;
    std::unique_ptr<QRadioButton> btn_U1_;
    std::unique_ptr<QRadioButton> btn_V1_;
    std::unique_ptr<QRadioButton> btn_U2_;
    std::unique_ptr<QRadioButton> btn_V2_;
    std::unique_ptr<QLineEdit> edt_projection_;
    std::unique_ptr<QLineEdit> edt_x0_;
    std::unique_ptr<QLineEdit> edt_x1_;
    std::unique_ptr<QLineEdit> edt_y0_;
    std::unique_ptr<QLineEdit> edt_y1_;
    std::unique_ptr<QCheckBox> chk_plotvirtuals_;
    std::unique_ptr<QPushButton> btn_square_;

    std::unique_ptr<QLabel> lbl_projection_;
    std::unique_ptr<QLabel> lbl_x0_;
    std::unique_ptr<QLabel> lbl_x1_;
    std::unique_ptr<QLabel> lbl_y0_;
    std::unique_ptr<QLabel> lbl_y1_;

    std::unique_ptr<QSpinBox> spin_numpoints_;

    bool readFloatField(QLineEdit *, double &, double, double, double);

  public slots:
    void markBad(QLineEdit *edt);
    void onFieldChange(const QString &dummy);
    void btn_sphere_toggled();
    void btn_plane_toggled();
    void btn_U1_toggled();
    void btn_U2_toggled();
    void btn_V1_toggled();
    void btn_V2_toggled();
    void btn_square_clicked();
    void plotvirtuals_stateChanged(int);
};