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

#include <QApplication>
#include <QProcess>

#ifdef Q_OS_WIN
#define USERPLATFORM "WINDOWS"
#else
#define USERPLATFORM "LINUX"
#endif

#include <memory>

class QFont;

class P4Application : public QApplication
{
    Q_OBJECT

  public:
    P4Application(int &argc, char **argv);

    std::unique_ptr<QFont> standardFont_;
    std::unique_ptr<QFont> boldFont_;
    std::unique_ptr<QFont> courierFont_;
    std::unique_ptr<QFont> boldCourierFont_;
    std::unique_ptr<QFont> titleFont_;
    std::unique_ptr<QFont> legendFont_;

  public slots:
    void signalEvaluated(int);
    void signalGcfEvaluated(int);
    void signalCurveEvaluated(int);
    void signalSeparatingCurvesEvaluated(int);
    void signalChanged(void);
    void signalLoaded(void);
    void signalSaved(void);
    void catchProcessError(QProcess::ProcessError);
};

extern std::unique_ptr<P4Application> gP4app;