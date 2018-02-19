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

#include <QDialog>

 #include <memory>

class QBoxLayout;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QWidget;

class P4PrintDlg : public QDialog
{
    Q_OBJECT

  private:
    std::unique_ptr<QPushButton> btn_default_;
    std::unique_ptr<QPushButton> btn_epsimage_;
    std::unique_ptr<QPushButton> btn_xfigimage_;
    std::unique_ptr<QPushButton> btn_cancel_;
    std::unique_ptr<QPushButton> btn_jpeg_;
    std::unique_ptr<QBoxLayout> mainLayout_;
    std::unique_ptr<QCheckBox> btn_blackwhite_;

    std::unique_ptr<QLabel> lbl_bgcolor_;
    std::unique_ptr<QRadioButton> btn_whitebg_;
    std::unique_ptr<QRadioButton> btn_blackbg_;

    std::unique_ptr<QLineEdit> edt_resolution_;
    std::unique_ptr<QLineEdit> edt_linewidth_;
    std::unique_ptr<QLineEdit> edt_symbolsize_;

  public:
    P4PrintDlg(QWidget *parent, Qt::WindowFlags f);

    static bool sm_LastBlackWhite;
    static double sm_LastLineWidth;
    static double sm_LastSymbolSize;
    static int sm_LastResolution;

  public slots:
    void onDefaultPrinter(void);
    void onEpsImagePrinter(void);
    void onXfigImagePrinter(void);
    void onJpegImagePrinter(void);
    void onCancel(void);

    bool readDialog(void);
    bool readFloatField(QLineEdit *edt, double *presult, double defvalue,
                        double minvalue, double maxvalue);
    void markBad(QLineEdit *edt);

    int getChosenResolution(void);
    double getChosenLineWidth(void);
    double getChosenSymbolSize(void);
};

#define P4PRINT_NONE 0
#define P4PRINT_DEFAULT 1
#define P4PRINT_EPSIMAGE 2
#define P4PRINT_XFIGIMAGE 3
#define P4PRINT_JPEGIMAGE 4