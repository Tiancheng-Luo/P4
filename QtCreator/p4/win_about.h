/*  This file is part of P4
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
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

#ifndef WIN_ABOUT_H
#define WIN_ABOUT_H

#include <QBoxLayout>
#include <QDialog>
#include <QPixmap>
#include <QPushButton>

class QP4AboutDlg : public QDialog
{
    Q_OBJECT

  private:
    QPushButton *btn_ok;
    QPushButton *btn_settings;

    QBoxLayout *mainLayout;
    QPixmap p4image;

  public:
    QP4AboutDlg(QWidget *parent = 0, Qt::WindowFlags f = 0);

  public slots:
    void OnOk(void);
    void OnSettings(void);
};

#endif /* WIN_ABOUT_H */
