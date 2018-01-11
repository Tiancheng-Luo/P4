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

#include "P4InputVF.h"

#include <QFile>
#include <QTextStream>

P4InputVF *g_ThisVF = nullptr;

/*
    THIS FILE IMPLEMENTS READING AND LOADING THE VECTOR FIELD, THE PARAMETERS
   FOR REDUCE/MAPLE
    AND THE OVERALL CONFIGURATION.

    filename.inp        input vector field (read and written by P4)
    filename.red        file read by reduce
    filename.run        batch file written by P4, read by the operating system
   to execute reduce/maple

    filename_inf.res    results of search at infinity, in text form
    filename_fin.res    results of search in the finite region, in text form


    IT DOES NOT IMPLEMENT THE READING OF THE RESULTS FROM MAPLE/REDUCE (THE .TAB
   FILES).

*/

/*
    Different files:
 search at the finite region, in text form

    filename_inf.tab
    filename_fin.tab
    filename_vec.tab
*/

/*
    Structure of .inp file:

    - integer 0,1,2,3  (type of singularities: all, finite, ...)
    - parameters:
        * integer 0,1 symbolic/numeric
        * integer precision
        * string epsilon
        * integer testsep
        * integer taylorlevel
        * integer numericlevel
        * integer maxlevel
        *_ integer weakness
        * only if type of singularity is ONE:
            + string x0
            + string y0
        * otherwhise:
            + integer P
            + integer Q

    - vector field:
        * string xdot
        * string ydot
        * string gcf
        * integer numparams
        * for each parameter:
            + string label
            + string value

    - optional: integer precision0
*/

// -----------------------------------------------------------------------
//          CONSTRUCTOR
// -----------------------------------------------------------------------
P4InputVF::P4InputVF()
{
    processText_ = nullptr;
    terminateProcessButton_ = nullptr;
    clearProcessButton_ = nullptr;
    evalProcess_ = nullptr;

    gcfDlg_ = nullptr;

    numeric_ = nullptr;
    precision_ = nullptr;
    epsilon_ = nullptr;
    testsep_ = nullptr;
    taylorlevel_ = nullptr;
    numericlevel_ = nullptr;
    maxlevel_ = nullptr;
    weakness_ = nullptr;

    numparams = 0;
    parvalue_ = nullptr;

    xdot_ = nullptr;
    ydot_ = nullptr;
    gcf_ = nullptr;

    numVF_ = 0;
    numCurves_ = 0;
    numPointsCurve_ = nullptr;
    curves_ = nullptr;
    numVFRegions_ = 0;
    numCurveRegions_ = 0;

    numSelected_ = 0;
    selected_ = nullptr;

    cleared_ = true;
    changed_ = false;
    evaluated_ = false;

    filename_ = DEFAULTFILENAME;

    reset(1);
}

// -----------------------------------------------------------------------
//          DESTRUCTOR
// -----------------------------------------------------------------------
P4InputVF::~P4InputVF()
{
    if (outputWindow_ != nullptr) {
        delete outputWindow_;
        outputWindow_ = nullptr;
    }
    // remove curve auxiliary files
    removeFile(getfilename_curvetable());
    removeFile(getfilename_curve());
    removeFile(getPrepareCurveFileName());
    // remove isoclines files too
    removeFile(getfilename_isoclinestable());
    removeFile(getfilename_isoclines());
    removeFile(getPrepareIsoclinesFileName());

    reset(0);
}

// -----------------------------------------------------------------------
//          P4InputVF::reset
// -----------------------------------------------------------------------
void P4InputVF::reset(int n)
{
    int i;

    g_VFResults.clearVFs();
    g_VFResults.vfK_ = nullptr;
    g_VFResults.K_ = 0;

    // symbolicpackage_ = PACKAGE_MAPLE;//getMathPackage();
    typeofstudy_ = DEFAULTTYPE;
    x0_ = DEFAULTX0;
    y0_ = DEFAULTY0;
    p_ = DEFAULTP;
    q_ = DEFAULTQ;

    // clear current vectors
    numeric_.clear();
    precision_.clear();
    epsilon_.clear();
    testsep_.clear();
    taylorlevel_.clear();
    numericlevel_.clear();
    maxlevel_.clear();
    weakness_.clear();

    xdot_.clear();
    ydot_.clear();
    gcf_.clear();

    // refill vectors with default values
    for (i = 0; i < n; i++) {
        numeric_.push_back(DEFAULTNUMERIC);
        precision_.push_back(DEFAULTPRECISION);
        epsilon_.push_back(QString(DEFAULTEPSILON));
        testsep_.push_back(DEFAULTTESTSEP);
        taylorlevel_.push_back(DEFAULTLEVEL);
        numericlevel_.push_back(DEFAULTNUMLEVEL);
        maxlevel_.push_back(DEFAULTMAXLEVEL);
        weakness_.push_back(DEFAULTWEAKNESS);
        xdot_.push_back(QString(DEFAULTXDOT));
        ydot_.push_back(QString(DEFAULTYDOT));
        gcf_.push_back(QString(DEFAULTGCF));
    }

    parvalue_.clear();
    parvalue_.swap(std::vector<std::vector<QString>>());

    curves_.clear();
    g_VFResults.resetCurveInfo();  // TODO: mirar si he de canviar alguna cosa
    numCurves_ = 0;

    numPointsCurve_.clear();

    if (n > 0) {
        for (i = 0; i < n; i++) {
            parvalue_.push_back(std::vector<QString>());
        }
    }

    numparams_ = 0;
    for (i = 0; i < MAXNUMPARAMS; i++)
        parlabel_[i] = "";

    changed_ = false;
    evaluated_ = false;
    evaluating_ = false;
    evaluatingPiecewiseConfig_ = false;
    cleared_ = true;

    curveRegions_.clear();
    numCurveRegions_ = 0;

    vfRegions_.clear();
    numVFRegions_ = 0;

    if (n > 0) {
        numSelected_ = 1;
        selected_.push_back(0);

        if (n == 1) {
            p4InputVFRegions::vfRegion region(0, nullptr);
            vfRegions_.push_back(region);
            numVFRegions_ = 1;
        }
    } else {
        numSelected_ = 0;
        selected_ = nullptr;
    }
    numVF_ = n;
}

// -----------------------------------------------------------------------
//          P4InputVF::load
// -----------------------------------------------------------------------
bool P4InputVF::load()
{
    emit loadSignal();

    FILE *fp;
    QString fname;
    char scanbuf[2560];
    int i, k, c;
    int flag_numeric, flag_testsep, aux;

    fname = getfilename();
    if (fname.length() == 0)
        return false;

    fp = fopen(QFile::encodeName(fname), "rt");
    if (fp == nullptr)
        return false;

    cleared_ = false;

    fscanf(fp, "%[^\n]\n", scanbuf);
    if (strcmp(scanbuf, "P5")) {
        // compatibility mode
        reset(1);

        fclose(fp);
        fp = fopen(QFile::encodeName(fname), "rt");

        if (fscanf(fp, "%d\n", &typeofstudy_) != 1 ||
            fscanf(fp, "%d\n", &flag_numeric) != 1 ||
            fscanf(fp, "%d\n", &aux) != 1 ||
            fscanf(fp, "%[^\n]\n", scanbuf) != 1 ||
            fscanf(fp, "%d\n", &flag_testsep) != 1) {
            reset(1);
            fclose(fp);
            return false;
        } else {
            bool value;
            value = ((flag_numeric == 0) ? false : true);
            numeric_.push_back(value);
            precision_.push_back(aux);
            epsilon_.push_back(QString(scanbuf));
            value = ((flag_testsep == 0) ? false : true);
            testsep_.push_back(value);
        }
        if (fscanf(fp, "%d\n", &aux) != 1) {
            reset(1);
            fclose(fp);
            return false;
        } else {
            taylorlevel_.push_back(aux);
        }
        if (fscanf(fp, "%d\n", &aux) != 1) {
            reset(1);
            fclose(fp);
            return false;
        } else {
            numericlevel_.push_back(aux);
        }
        if (fscanf(fp, "%d\n", &aux) != 1) {
            reset(1);
            fclose(fp);
            return false;
        } else {
            maxlevel_.push_back(aux);
        }
        if (fscanf(fp, "%d\n", &aux) != 1) {
            reset(1);
            fclose(fp);
            return false;
        } else {
            weakness_.push_back(aux);
        }

        if (typeofstudy_ == TYPEOFSTUDY_ONE) {
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            x0_ = scanbuf;
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            y0_ = scanbuf;
            p_ = 1;
            q_ = 1;
        } else {
            x0_ = DEFAULTX0;
            y0_ = DEFAULTY0;
            if (fscanf(fp, "%d\n", &p_) != 1 || fscanf(fp, "%d\n", &q_) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
        }

        if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        xdot_.push_back(QString(scanbuf));
        if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        ydot_.push_back(QString(scanbuf));
        if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        gcf_.push_back(QString(scanbuf));

        if (xdot_[0] == "(null)")
            xdot_[0] = "";
        if (ydot_[0] == "(null)")
            ydot_[0] = "";
        if (gcf_[0] == "(null)")
            gcf_[0] = "";
        if (x0_ == "(null)")
            x0_ = "";
        if (y0_ == "(null)")
            y0_ = "";
        if (epsilon_[0] == "(null)")
            epsilon_[0] = "";

        if (fscanf(fp, "%d\n", &numparams_) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        for (i = 0; i < numparams_; i++) {
            if (fscanf(fp, "%s", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            parlabel_[i] = scanbuf;
            while ((c = fgetc(fp)) == '\n')
                ;
            ungetc(c, fp);
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            parvalue_[0].push_back(QString(scanbuf));
        }
        for (i = numparams_; i < MAXNUMPARAMS; i++) {
            parlabel_[i] = QString();
            parvalue_[0][i].push_back(QString());
        }
    } else {
        QString _x0;
        QString _y0;
        int _typeofstudy;
        int _p;
        int _q;
        int n;

        if (fscanf(fp, "%d\n", &_typeofstudy) != 1) {
            reset(1);
            fclose(fp);
            return false;
        }
        if (_typeofstudy == TYPEOFSTUDY_ONE) {
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            _x0 = scanbuf;
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            _y0 = scanbuf;
            if (_x0 == "(null)")
                _x0 = "";
            if (_y0 == "(null)")
                _y0 = "";
            _p = 1;
            _q = 1;
        } else {
            _x0 = DEFAULTX0;
            _y0 = DEFAULTY0;
            if (fscanf(fp, "%d\n", &_p) != 1 || fscanf(fp, "%d\n", &_q) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
        }

        if (fscanf(fp, "%d\n", &n) != 1 || n <= 0) {
            reset(1);
            fclose(fp);
            return false;
        }

        reset(n);
        x0_ = _x0;
        y0_ = _y0;
        p_ = _p;
        q_ = _q;
        typeofstudy_ = _typeofstudy;

        g_VFResults.resetCurveInfo();

        if (fscanf(fp, "%d\n", &numCurves_) != 1 || numCurves_ < 0) {
            reset(1);
            fclose(fp);
            return false;
        }

        if (numCurves_ > 0) {
            int npoints;
            for (k = 0; k < numCurves_; k++) {
                if (fscanf(fp, "%[^\n]\n", scanbuf) != 1 ||
                    fscanf(fp, "%d\n", &npoints) != 1) {
                    reset(1);
                    fclose(fp);
                    return false;
                }
                curves_.push_back(QString(scanbuf));
                numPointsCurve_.push_back(npoints);
            }
        } else {
            curves_.clear();
            numPointsCurve_.clear();
        }

        if (fscanf(fp, "%d\n", &numparams_) != 1 || numparams_ < 0 ||
            numparams_ > MAXNUMPARAMS) {
            reset(1);
            fclose(fp);
            return false;
        }
        for (i = 0; i < numparams_; i++) {
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            parlabel_[i] = scanbuf;
        }
        for (i = numparams_; i < MAXNUMPARAMS; i++)
            parlabel_[i] = QString();

        if (fscanf(fp, "%d\n", &numVFRegions_) != 1 || numVFRegions_ < 0) {
            reset(1);
            fclose(fp);
            return false;
        }
        vfRegions_.clear();
        for (i = 0; i < numVFRegions_; i++) {
            // read index
            int indx;
            if (fscanf(fp, "%d", &indx) != 1 || indx < 0 || indx >= numVF_) {
                reset(1);
                fclose(fp);
                return false;
            }
            // read signs
            std::vector<int> sgns;
            int aux;
            for (k = 0; k < numCurves_; k++) {
                if (fscanf(fp, "%d", &aux) != 1 || (aux != 1 && aux != -1)) {
                    reset(1);
                    fclose(fp);
                    return false;
                }
                sgns.push_back(aux);
            }
            // sgns could be an empty list
            p4InputVFRegions::vfRegion region(indx, sgns);
            vfRegions_.push_back(region);
        }

        if (fscanf(fp, "%d\n", &numCurveRegions_) != 1 ||
            numCurveRegions_ < 0) {
            reset(1);
            fclose(fp);
            return false;
        }
        curveRegions_.clear();
        for (i = 0; i < numCurveRegions_; i++) {
            // read index
            int indx;
            if (fscanf(fp, "%d", &indx) != 1 || indx < 0 ||
                indx >= numCurves_) {
                reset(1);
                fclose(fp);
                return false;
            }
            // read signs
            std::vector<int> sgns;
            int aux;
            for (k = 0; k < numCurves_; k++) {
                if (fscanf(fp, "%d", &aux) != 1 || aux > 1 || aux < -1) {
                    reset(1);
                    fclose(fp);
                    return false;
                }
                sgns.push_back(aux);
            }
            // sgns could be an empty list
            p4InputVFRegions::curveRegion region(indx, sgns);
            curveRegions_.push_back(region);
        }

        for (k = 0; k < numVF_; k++) {
            if (fscanf(fp, "%d\n", &flag_numeric) != 1 ||
                fscanf(fp, "%d\n", &aux) != 1 ||
                fscanf(fp, "%[^\n]\n", scanbuf) != 1 ||
                fscanf(fp, "%d\n", &flag_testsep) != 1) {
                reset(1);
                fclose(fp);
                return false;
            } else {
                bool value;
                value = ((flag_numeric == 0) ? false : true);
                numeric_.push_back(value);
                precision_.push_back(aux);
                epsilon_.push_back(QString(scanbuf));
                value = ((flag_testsep == 0) ? false : true);
                testsep_.push_back(value);
            }
            if (fscanf(fp, "%d\n", &aux) != 1) {
                reset(1);
                fclose(fp);
                return false;
            } else {
                taylorlevel_.push_back(aux);
            }
            if (fscanf(fp, "%d\n", &aux) != 1) {
                reset(1);
                fclose(fp);
                return false;
            } else {
                numericlevel_.push_back(aux);
            }
            if (fscanf(fp, "%d\n", &aux) != 1) {
                reset(1);
                fclose(fp);
                return false;
            } else {
                maxlevel_.push_back(aux);
            }
            if (fscanf(fp, "%d\n", &aux) != 1) {
                reset(1);
                fclose(fp);
                return false;
            } else {
                weakness_.push_back(aux);
            }

            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            xdot_.push_back(QString(scanbuf));
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            ydot_.push_back(QString(scanbuf));
            if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                reset(1);
                fclose(fp);
                return false;
            }
            gcf_.push_back(QString(scanbuf));

            if (xdot_[k] == "(null)")
                xdot_[k] = "";
            if (ydot_[k] == "(null)")
                ydot_[k] = "";
            if (gcf_[k] == "(null)")
                gcf_[k] = "";
            if (epsilon_[k] == "(null)")
                epsilon_[k] = "";

            for (i = 0; i < numparams_; i++) {
                if (fscanf(fp, "%[^\n]\n", scanbuf) != 1) {
                    reset(1);
                    fclose(fp);
                    return false;
                }
                parvalue_[k].push_back(QString(scanbuf));
            }
            for (i = numparams_; i < MAXNUMPARAMS; i++)
                parvalue_[k].push_back(QString());
        }
    }

    fclose(fp);

    changed_ = false;
    cleared_ = false;
    evaluated_ = checkevaluated();
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::checkevaluated
// -----------------------------------------------------------------------
bool P4InputVF::checkevaluated()
{
    // if the evaluate files are already found on disc, then set the flag to
    // true.

    QFileInfo *fi;
    QFileInfo *fifin;
    QFileInfo *fiinf;
    QFileInfo *fivec;

    QDateTime dt;
    QDateTime dtfin;
    QDateTime dtinf;
    QDateTime dtvec;

    fi = new QFileInfo(getfilename());
    dt = fi->lastModified();
    delete fi;
    fi = nullptr;

    fivec = new QFileInfo(getbarefilename() + "_vec.tab");
    if (fivec->isFile() == false) {
        delete fivec;
        fivec = nullptr;
        return false;
    }
    dtvec = fivec->lastModified();
    if (dtvec.secsTo(dt) > 0 || dtvec.daysTo(dt) > 0)
        return false;

    if (typeofstudy_ != TYPEOFSTUDY_INF) {
        fifin = new QFileInfo(getbarefilename() + "_fin.tab");
        if (fifin->isFile() == false) {
            delete fifin;
            fifin = nullptr;
            return false;
        }
        dtfin = fifin->lastModified();
        delete fifin;
        fifin = nullptr;
        if (dtfin.secsTo(dt) > 0 || dtfin.daysTo(dt) > 0)
            return false;
    }

    if (typeofstudy_ == TYPEOFSTUDY_INF || typeofstudy_ == TYPEOFSTUDY_ALL) {
        fiinf = new QFileInfo(getbarefilename() + "_inf.tab");
        if (fiinf->isFile() == false) {
            delete fiinf;
            fiinf = nullptr;
            return false;
        }
        dtinf = fiinf->lastModified();
        delete fiinf;
        fiinf = nullptr;
        if (dtinf.secsTo(dt) > 0 || dtinf.daysTo(dt) > 0)
            return false;
    }

    if (fi != nullptr) {
        delete fi;
        fi = nullptr;
    }
    if (fifin != nullptr) {
        delete fifin;
        fifin = nullptr;
    }
    if (fiinf != nullptr) {
        delete fiinf;
        fiinf = nullptr;
    }
    if (fivec != nullptr) {
        delete fivec;
        fivec = nullptr;
    }

    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::save
// -----------------------------------------------------------------------
bool P4InputVF::save()
{
    int i, k;

    QSettings settings(getbarefilename().append(".conf"),
                       QSettings::NativeFormat);
    settings.clear();
    emit saveSignal();

    QString fname = getfilename();
    if (fname.isEmpty())
        return false;
    QFile file(QFile::encodeName(getfilename()));
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&file);

        out << "P5\n";
        out << typeofstudy_ << "\n";
        if (typeofstudy_ == TYPEOFSTUDY_ONE) {
            if (x0_.isEmpty())
                out << "(null)\n";
            else
                out << x0_ << "\n";
            if (y0_.isEmpty())
                out << "(null)\n";
            else
                out << y0_ << "\n";
        } else {
            out << p_ << "\n";
            out << q_ << "\n";
        }

        out << numVF_ << "\n";
        out << numCurves_ << "\n";
        for (i = 0; i < numCurves_; i++) {
            if (curves_[i].isEmpty())
                out << "(null)\n";
            else
                out << curves_[i] << "\n";
            out << numPointsCurve_[i] << "\n";
        }

        out << numparams_ << "\n";
        for (i = 0; i < numparams_; i++) {
            if (parlabel_[i].isEmpty())
                out << "(null)\n";
            else
                out << parlabel_[i] << "\n";
        }

        out << numVFRegions_ << "\n";
        for (i = 0; i < numVFRegions_; i++) {
            out << vfRegions_[i].vfIndex;
            for (k = 0; k < numCurves_; k++)
                out << vfRegions_[i].signs[k];
            out << "\n";
        }

        out << numCurveRegions_ << "\n";
        for (i = 0; i < numCurveRegions_; i++) {
            out << curveRegions_[i].curveIndex;
            for (k = 0; k < numCurves_; k++)
                out << curveRegions_[i].signs[k];
            out << "\n";
        }

        for (i = 0; i < numVF_; i++) {
            out << numeric_[i] << "\n";
            out << precision_[i] << "\n";
            if (epsilon_[i].isEmpty())
                out << "(null)\n";
            else
                out << epsilon_[i] << "\n";
            out << testsep_[i] << "\n";
            out << taylorlevel_[i] << "\n";
            out << numericlevel_[i] << "\n";
            out << maxlevel_[i] << "\n";
            out << weakness_[i] << "\n";
            if (xdot_[i].isEmpty())
                out << "(null)\n";
            else
                out << xdot_[i] << "\n";
            if (ydot_[i].isEmpty())
                out << "(null)\n";
            else
                out << ydot_[i] << "\n";
            if (gcf_[i].isEmpty())
                out << "(null)\n";
            else
                out << gcf_[i] << "\n";

            for (k = 0; k < numparams_; k++) {
                if (parvalue_[i][k].isEmpty())
                    out << "(null)\n";
                else
                    out << parvalue_[i][k] << "\n";
            }
        }
        out.flush();
        file.close();
        changed_ = false;
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::getbarefilename
// -----------------------------------------------------------------------
QString P4InputVF::getbarefilename() const
{
    int slash1;
    int slash2;
    int slash3;
    int dot;
    QString fname;

    fname = filename.trimmed();

    if (fname.length() == 0) {
        return "";
    }

    slash1 = fname.lastIndexOf('/');
    slash2 = fname.lastIndexOf('\\');
    slash3 = fname.lastIndexOf(':');
    dot = fname.lastIndexOf('.');

    if (dot <= slash1 && dot <= slash2 && dot <= slash3) {
        // there is no dot present, or at least not in the
        // last part of the file name.
        return fname;
    }

    return fname.left(dot);
}

// -----------------------------------------------------------------------
//          P4InputVF::getbarefilename
// -----------------------------------------------------------------------
QString P4InputVF::getfilename() const
{
    int slash1;
    int slash2;
    int slash3;
    int dot;
    QString fname;

    fname = filename.trimmed();

    if (fname.length() != 0) {
        slash1 = fname.lastIndexOf('/');
        slash2 = fname.lastIndexOf('\\');
        slash3 = fname.lastIndexOf(':');
        dot = fname.lastIndexOf('.');

        if (dot <= slash1 && dot <= slash2 && dot <= slash3) {
            // there is no dot present, or at least not in the
            // last part of the file name.

            return fname.append(".inp");
        }
    }
    return fname;
}

// -----------------------------------------------------------------------
//          P4InputVF::getfilename_xxx
// -----------------------------------------------------------------------
QString P4InputVF::getfilename_finresults() const
{
    return getbarefilename().append("_fin.res");
}
QString P4InputVF::getfilename_infresults() const
{
    return getbarefilename().append("_inf.res");
}
QString P4InputVF::getfilename_fintable() const
{
    return getbarefilename().append("_fin.tab");
}
QString P4InputVF::getfilename_inftable() const
{
    return getbarefilename().append("_inf.tab");
}
QString P4InputVF::getfilename_vectable() const
{
    return getbarefilename().append("_vec.tab");
}
QString P4InputVF::getfilename_gcf() const
{
    return getbarefilename().append("_gcf.tab");
}
// only used in case of reduce: contains reduce output, no gcf data and is
// deleted immediately.
/*QString P4InputVF::getfilename_gcfresults() const
{
    return getbarefilename().append("_gcf.res");
}*/
QString P4InputVF::getfilename_curveresults() const
{
    return getbarefilename().append("_curves.tab");
}
/*QString P4InputVF::getreducefilename() const
{
    return getbarefilename().append(".red");
}*/
QString P4InputVF::getmaplefilename() const
{
    return getbarefilename().append(".txt");
}
QString P4InputVF::getrunfilename() const
{
    return getbarefilename().append(".run");
}
// curve filenames
QString P4InputVF::getfilename_curvetable() const
{
    return getbarefilename().append("_veccurve.tab");
}
QString P4InputVF::getfilename_curve() const
{
    return getbarefilename().append("_curve.tab");
}
QString P4InputVF::getPrepareCurveFileName() const
{
    return getbarefilename().append("_curve_prep.txt");
}
// isoclines filenames
QString P4InputVF::getfilename_isoclinestable() const
{
    return getbarefilename().append("_vecisoclines.tab");
}
QString P4InputVF::getfilename_isoclines() const
{
    return getbarefilename().append("_isoclines.tab");
}
QString P4InputVF::getPrepareIsoclinesFileName() const
{
    return getbarefilename().append("_isoclines_prep.txt");
}

// -----------------------------------------------------------------------
//          P4InputVF::fileExists
// -----------------------------------------------------------------------
bool P4InputVF::fileExists(QString fname)
{
    QFile fp(QFile::encodeName(fname));
    if (fp.exists())
        return true;
    else
        return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareMapleParameters
// -----------------------------------------------------------------------

void P4InputVF::prepareMapleParameters(QTextStream &fp)
{
    QString s;
    int i;

    fp << "user_numeric_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        fp << booleanString(numeric_[i]);
        if (i == numVF_ - 1)
            fp << " ]:\n";
        else
            fp << ", ";
    }

    fp << "epsilon_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        fp << epsilon_[i];
        if (i == numVF_ - 1)
            fp << " ]:\n";
        else
            fp << ", ";
    }

    fp << "test_sep_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        fp << booleanString(testsep_[i]);
        if (i == numVF_ - 1)
            fp << " ]:\n";
        else
            fp << ", ";
    }

    fp << "user_precision_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        fp << precision_[i];
        if (i == numVF_ - 1)
            fp << " ]:\n";
        else
            fp << ", ";
    }

    fp << "taylor_level_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        fp << taylorlevel_[i];
        if (i == numVF_ - 1)
            fp << " ]:\n";
        else
            fp << ", ";
    }

    fp << "numeric_level_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        fp << numericlevel_[i];
        if (i == numVF_ - 1)
            fp << " ]:\n";
        else
            fp << ", ";
    }

    fp << "max_level_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        fp << maxlevel_[i];
        if (i == numVF_ - 1)
            fp << " ]:\n";
        else
            fp << ", ";
    }

    fp << "weakness_level_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        fp << weakness_[i];
        if (i == numVF_ - 1)
            fp << " ]:\n";
        else
            fp << ", ";
    }

    if (typeofstudy == TYPEOFSTUDY_ONE) {
        fp << "user_p := 1:\n";
        fp << "user_q := 1:\n";

        s = x0.toUtf8();
        fp << "x0 := " << x0_ << ":\n";
        fp << "y0 := " << y0_ << ":\n";
        s.sprintf("x_min := x0+(%f):\n", (float)(X_MIN));
        fp << s;
        s.sprintf("x_max := x0+(%f):\n", (float)(X_MAX));
        fp << s;
        s.sprintf("y_min := y0+(%f):\n", (float)(Y_MIN));
        fp << s;
        s.sprintf("y_max := y0+(%f):\n", (float)(Y_MAX));
        fp << s;
    } else {
        s.sprintf("user_p := %d:\n", p_);
        fp << s;
        s.sprintf("user_q := %d:\n", q_);
        fp << s;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareMapleVectorField
// -----------------------------------------------------------------------
void P4InputVF::prepareMapleVectorField(QTextStream &fp)
{
    QString myxdot;
    QString myydot;
    QString mygcf;
    QString lbl;
    QString val;
    int k, i;

    fp << "user_f_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        myxdot = convertMapleUserParameterLabels(*(xdot_[i]));  // FIXME
        myydot = convertMapleUserParameterLabels(*(ydot_[i]));
        fp << "[" << myxdot << "," << myydot << "]";
        if (i == numVF_ - 1)
            fp << " ]:\n";
        else
            fp << ", ";
    }

    fp << "user_gcf_pieces := [ ";
    for (i = 0; i < numVF_; i++) {
        mygcf = convertMapleUserParameterLabels(*(gcf_[i]));
        fp << mygcf;
        if (i == numVF_ - 1)
            fp << " ]:\n";
        else
            fp << ", ";
    }

    fp << "user_parameters := [ ";
    for (k = 0; k < numparams_; k++) {
        lbl = convertMapleUserParameterLabels(parlabel_[k]);
        fp << lbl;
        if (k == numparams_ - 1)
            fp << " ]:\n";
        else
            fp << ", ";
    }
    if (numparams_ == 0)
        fp << " ]:\n";

    for (k = 0; k < numparams_; k++) {
        lbl = convertMapleUserParameterLabels(parlabel_[k]);

        if (lbl.length() == 0)
            continue;

        fp << lbl << "_pieces := [ ";

        for (i = 0; i < numVF_; i++) {
            val = convertMapleUserParameterLabels(*(parvalue_[i][k]));  // FIXME
            if (!numeric_[i])
                fp << val;
            else
                fp << "evalf(" << val << ")";
            if (i == numVF_ - 1)
                fp << " ]:\n";
            else
                fp << ", ";
        }
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareMapleCurves (P5 version)
// -----------------------------------------------------------------------
void P4InputVF::prepareMapleCurves(QTextStream &fp)
{
    int i;
    QString s;

    fp << "user_curves := [ ";
    if (numCurves_ > 0)
        for (i = 0; i < numCurves_; i++) {
            s = convertMapleUserParameterLabels(*(curves_[i]));
            fp << s;
            if (i == numCurves_ - 1)
                fp << " ]:\n";
            else
                fp << ", ";
        }
    else
        fp << "]:\n";

    fp << "user_numpointscurves :=[ ";
    if (numCurves_ > 0)
        for (i = 0; i < numCurves_; i++) {
            fp << numPointsCurve_[i];
            if (i == numCurves_ - 1)
                fp << " ]:\n";
            else
                fp << ", ";
        }
    else
        fp << "]:\n";
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareMapleCurve (P4 version)
// -----------------------------------------------------------------------
void P4InputVF::prepareMapleCurve(QTextStream &fp)  // FIXME
{
    QString mycurve;
    QString lbl;
    QString val;
    int k;

    mycurve = convertMapleUserParameterLabels(curve_);
    fp << "user_curve := " << mycurve << ":\n";

    for (k = 0; k < numparams_; k++) {
        lbl = convertMapleUserParameterLabels(parlabel_[k]);
        val = convertMapleUserParameterLabels(parvalue_[k]);

        if (lbl.length() == 0)
            continue;

        if (!numeric_) {
            fp << lbl << " := " << val << ":\n";
        } else {
            fp << lbl << " := evalf( " << val << " ):\n";
        }
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareMapleisoclines
// -----------------------------------------------------------------------
void P4InputVF::prepareMapleIsoclines(QTextStream &fp)
{
    QString myisoclines;
    QString lbl;
    QString val;
    int k;

    myisoclines = convertMapleUserParameterLabels(isoclines_);
    fp << "user_isoclines := " << myisoclines << ":\n";

    for (k = 0; k < numparams_; k++) {
        lbl = convertMapleUserParameterLabels(parlabel_[k]);
        val = convertMapleUserParameterLabels(parvalue_[k]);

        if (lbl.length() == 0)
            continue;

        if (!numeric_) {
            fp << lbl << " := " << val << ":\n";
        } else {
            fp << lbl << " := evalf( " << val << " ):\n";
        }
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareMaplePiecewiseConfig
// -----------------------------------------------------------------------

void P4InputVF::prepareMaplePiecewiseConfig(QTextStream &fp)
{
    int i, k;

    fp << "vfregions := [ ";
    if (numVFRegions_ == 0)
        fp << "]:\n";
    else {
        for (i = 0; i < numVFRegions_; i++) {
            fp << "[" << vfregions[i].vfindex << ", [ ";
            for (k = 0; k < numCurves_; k++) {
                fp << vfregions[i].signs[k];
                if (k < numCurves_ - 1)
                    fp << ",";
            }
            if (i == numVFRegions_ - 1)
                fp << "] ] ]:\n";
            else
                fp << "] ],\n    ";
        }
    }

    fp << "curveregions := [ ";
    if (numCurveRegions_ == 0)
        fp << "]:\n";
    else {
        for (i = 0; i < numCurveRegions_; i++) {
            fp << "[" << curveRegions_[i].curveindex << ", [ ";
            for (k = 0; k < numCurves_; k++) {
                fp << curveRegions_[i].signs[k];
                if (k < numCurves_ - 1)
                    fp << ",";
            }
            if (i == numCurveRegions_ - 1)
                fp << "] ] ]:\n";
            else
                fp << "] ],\n    ";
        }
    }
}

// -----------------------------------------------------------------------
//          indexOfWordInString
// -----------------------------------------------------------------------
//
//  If the user gives a vector field with user-parameters such as "alpha" or
//  "b", we add a suffix to these qualifier names and change them to "alpha_" or
//  "b_", in order to avoid mixing with internal variables.
static int indexOfWordInString(const QString *src, const QString *word,
                               int start = 0)
{
    int i, j;

    while ((i = src->indexOf(*word, start)) != -1) {
        // we have found word as a substring.  The index i is an index from the
        // very beginning of string (not depending of start)

        start = i + 1;  // do not find this substring next loop

        // check if the substring is the beginning of a word:
        j = i;
        if (j > 0)
            if ((*src)[j - 1].isLetter() || (*src)[j - 1] == '_' ||
                (*src)[j - 1].isDigit())
                continue;  // no: it is part of a bigger word, so continue...

        // check if the substring is the end of a word;
        j = i + word->length();
        if (j < src->length())
            if ((*src)[j].isLetter() || (*src)[j] == '_' || (*src)[j].isDigit())
                continue;  // no: it is part of a bigger word, so continue...

        // ok: we have a word: stop looping.
        break;
    }
    return i;
}

// -----------------------------------------------------------------------
//          P4InputVF::convertMapleUserParameterLabels
// -----------------------------------------------------------------------
QString P4InputVF::convertMapleUserParameterLabels(QString src)
{
    QString s;
    QString t;
    QString p, newlabel;
    int i, k;

    s = src;
    for (k = 0; k < numparams_; k++) {
        p = parlabel_[k];
        newlabel = p + "_";

        if (p.length() == 0)
            continue;

        t = "";
        while (1) {
            i = indexOfWordInString(&s, &p);
            if (i == -1)
                break;

            t += s.left(i);
            t += newlabel;
            s = s.mid(i + p.length());
        }
        s = t + s;
    }

    return s;
}

// -----------------------------------------------------------------------
//          P4InputVF::convertMapleUserParametersLabelsToValues
// -----------------------------------------------------------------------
QString P4InputVF::convertMapleUserParametersLabelsToValues(QString src)
{
    QString t, p, newlabel;
    int i, k;
    QString s = src;
    for (k = 0; k < numparams_; k++) {
        p = parlabel_[k];
        newlabel = parvalue_[k];
        if (p.length() == 0)
            continue;
        t = "";
        while (1) {
            i = indexOfWordInString(&s, &p);
            if (i == -1)
                break;
            t += s.left(i);
            t += newlabel;
            s = s.mid(i + p.length());
        }
        s = t + s;
    }
    return s;
}

#ifdef Q_OS_WIN
extern QByteArray win_GetShortPathName(QByteArray f);
#endif

// -----------------------------------------------------------------------
//          maplepathformat
// -----------------------------------------------------------------------
//
// replace all backslashes \ by \\ (ANSI C format)
//
// Under windows, avoid spaces by using the short path form.

QByteArray maplepathformat(const QString fname)
{
    QByteArray ba_fname;

    ba_fname = QFile::encodeName(fname);
#ifdef Q_OS_WIN
    if (ba_fname.length() == 0) {
        return ba_fname;
    }
    if (ba_fname[ba_fname.length() - 1] != '\\' &&
        ba_fname[ba_fname.length() - 1] != '/') {
        // last character is not a path separator

        // no need to convert to short form
        // (this is to avoid error: conversion to short forms does not work
        // for files that possibly do not yet exist)

        ba_fname.replace("\\", "\\\\");
        return ba_fname;
    }

    ba_fname = win_GetShortPathName(ba_fname);
#endif
    ba_fname.replace("\\", "\\\\");
    return ba_fname;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareFile
// -----------------------------------------------------------------------

void P4InputVF::prepareFile(QTextStream &fp, bool prepareforcurves)
{
    QString bsaveall;
    QString name_vectab;
    QString name_inftab;
    QString name_fintab;
    QString name_infres;
    QString name_finres;
    QString name_curves;
    QString mainreduce;
    QString s;

    QString mainmaple;
    QString user_bindir;
    QString user_tmpdir;
    QString user_lypexe;
    QString user_lypexe_mpf;
    QString user_sepexe;
    QString user_platform;
    QString user_removecmd;
    QString user_simplify;
    QString user_simplifycmd;
    QString user_sumtablepath;
    QString user_exeprefix;

    QByteArray ba_mainmaple;
    QByteArray ba_user_bindir;
    QByteArray ba_user_tmpdir;
    QByteArray ba_user_sumtablepath;
    QByteArray ba_name_vectab;
    QByteArray ba_name_fintab;
    QByteArray ba_name_inftab;
    QByteArray ba_name_finres;
    QByteArray ba_name_infres;
    QByteArray ba_name_curves;

    user_exeprefix = "";

    mainmaple = getP4MaplePath();
    user_bindir = getP4BinPath();
    user_tmpdir = getP4TempPath();
    user_sumtablepath = getP4SumTablePath();

    mainmaple += QDir::separator();
    if (user_bindir != "")
        user_bindir += QDir::separator();
    if (user_tmpdir != "")
        user_tmpdir += QDir::separator();
    if (user_sumtablepath != "")
        user_sumtablepath += QDir::separator();
    user_platform = USERPLATFORM;
#ifdef Q_WS_WIN
    user_removecmd = "cmd /c del";
    user_exeprefix = "cmd /c ";
#else
    user_removecmd = "rm";
    user_exeprefix = "";
#endif
    mainmaple += MAINMAPLEFILE;
#ifdef Q_WS_WIN
    user_lypexe = "lyapunov.exe";
    user_lypexe_mpf = "lyapunov_mpf.exe";
    user_sepexe = "separatrice.exe";
#else
    user_lypexe = "lyapunov";
    user_lypexe_mpf = "lyapunov_mpf";
    user_sepexe = "separatrice";
#endif

    ba_mainmaple = maplepathformat(mainmaple);
    ba_user_bindir = maplepathformat(user_bindir);
    ba_user_tmpdir = maplepathformat(user_tmpdir);
    ba_user_sumtablepath = maplepathformat(user_sumtablepath);

    if (numeric_[0])
        user_simplify = "false";
    else
        user_simplify = "true";

    user_simplifycmd = MAPLE_SIMPLIFY_EXPRESSIONS;

    fp << "restart;\n";
    fp << "read( \"" << ba_mainmaple << "\" );\n";
    fp << "user_bindir := \"" << ba_user_bindir << "\":\n";
    fp << "user_tmpdir := \"" << ba_user_tmpdir << "\":\n";
    fp << "user_lypexe := \"" << user_lypexe << "\":\n";
    fp << "user_lypexe_mpf := \"" << user_lypexe_mpf << "\":\n";
    fp << "user_sepexe := \"" << user_sepexe << "\":\n";
    fp << "user_exeprefix := \"" << user_exeprefix << "\":\n";
    fp << "user_platform := \"" << user_platform << "\":\n";
    fp << "user_sumtablepath := \"" << ba_user_sumtablepath << "\":\n";
    fp << "user_removecmd := \"" << user_removecmd << "\":\n";
    fp << "user_simplify := " << user_simplify << ":\n";
    fp << "user_simplifycmd := " << user_simplifycmd << ":\n";

    bsaveall = booleanString(action_SaveAll);

    name_vectab = getfilename_vectable();
    name_fintab = getfilename_fintable();
    name_inftab = getfilename_inftable();
    name_finres = getfilename_finresults();
    name_infres = getfilename_infresults();
    name_curves = getfilename_curveresults();

    if (prepareforcurves) {
        removeFile(name_curves);
    } else {
        removeFile(name_vectab);
        removeFile(name_fintab);
        removeFile(name_inftab);
        removeFile(name_infres);
        removeFile(name_finres);
    }

    ba_name_vectab = maplepathformat(name_vectab);
    ba_name_fintab = maplepathformat(name_fintab);
    ba_name_inftab = maplepathformat(name_inftab);
    ba_name_finres = maplepathformat(name_finres);
    ba_name_infres = maplepathformat(name_infres);
    ba_name_curves = maplepathformat(name_curves);

    fp << "all_crit_points := " << typeofstudy_ << ":\n";
    fp << "save_all := " << bsaveall << ":\n";

    fp << "vec_table := \"" << ba_name_vectab << "\":\n";
    fp << "finite_table := \"" << ba_name_fintab << "\":\n";
    fp << "finite_res := \"" << ba_name_finres << "\":\n";
    fp << "infinite_table := \"" << ba_name_inftab << "\":\n";
    fp << "infinite_res := \"" << ba_name_infres << "\":\n";
    fp << "user_curvesfile := \"" << ba_name_curves << "\":\n";

    prepareMapleVectorField(fp);
    prepareMapleParameters(fp);
    prepareMapleCurves(fp);
    prepareMaplePiecewiseConfig(fp);

    if (prepareforcurves) {
        fp << "user_precision := 8:\ntry FindAllCurves() catch:\n"
              "printf( \"! Error (\%a) \%a\\n\", lastexception[1], "
              "lastexception[2] );\n"
              "finally: closeallfiles();\n"
              "if normalexit=0 then `quit`(0); else `quit(1)` end if: end "
              "try:\n";
    } else {
        fp << "try p5main() catch:\n"
              "printf( \"! Error (\%a) \%a\\n\", lastexception[1], "
              "lastexception[2] );\n"
              "finally: closeallfiles();\n"
              "if normalexit=0 then `quit`(0); else `quit(1)` end if: end "
              "try:\n";
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareCurveFile
// -----------------------------------------------------------------------
void P4InputVF::prepareCurveFile(QTextStream &fp)
{
    QString name_curvetab;
    QString s;

    QString mainmaple;
    QString user_bindir;
    QString user_tmpdir;
    QString user_platform;
    QString user_removecmd;
    QString user_simplify;
    QString user_simplifycmd;
    QString user_sumtablepath;
    QString user_exeprefix;

    QByteArray ba_mainmaple;
    QByteArray ba_user_bindir;
    QByteArray ba_user_tmpdir;
    QByteArray ba_name_curvetab;

    user_exeprefix = "";

    mainmaple = getP4MaplePath();
    user_bindir = getP4BinPath();
    user_tmpdir = getP4TempPath();
    user_sumtablepath = getP4SumTablePath();

    mainmaple += QDir::separator();
    if (user_bindir != "")
        user_bindir += QDir::separator();
    if (user_tmpdir != "")
        user_tmpdir += QDir::separator();
    user_platform = USERPLATFORM;
#ifdef Q_OS_WIN
    user_removecmd = "cmd /c del";
    user_exeprefix = "cmd /c ";
#else
    user_removecmd = "rm";
    user_exeprefix = "";
#endif
    mainmaple += MAINMAPLEFILE;

    ba_mainmaple = maplepathformat(mainmaple);
    ba_user_bindir = maplepathformat(user_bindir);
    ba_user_tmpdir = maplepathformat(user_tmpdir);

    if (numeric_[0])
        user_simplify = "false";
    else
        user_simplify = "true";

    user_simplifycmd = MAPLE_SIMPLIFY_EXPRESSIONS;

    fp << "restart;\n";
    fp << "read( \"" << ba_mainmaple << "\" );\n";
    fp << "user_bindir := \"" << ba_user_bindir << "\":\n";
    fp << "user_tmpdir := \"" << ba_user_tmpdir << "\":\n";
    fp << "user_exeprefix := \"" << user_exeprefix << "\":\n";
    fp << "user_platform := \"" << user_platform << "\":\n";
    fp << "user_removecmd := \"" << user_removecmd << "\":\n";
    fp << "user_simplify := " << user_simplify << ":\n";
    fp << "user_simplifycmd := " << user_simplifycmd << ":\n";
    fp << "all_crit_points := " << typeofstudy_ << ":\n";

    name_curvetab = getfilename_curvetable();
    removeFile(name_curvetab);
    ba_name_curvetab = maplepathformat(name_curvetab);
    fp << "curve_table := \"" << ba_name_curvetab << "\":\n";

    prepareMapleCurve(fp);
    prepareMapleParameters(fp);

    fp << "try prepareCurve() catch:\n"
          "printf( \"! Error (\%a) \%a\\n\", lastexception[1], "
          "lastexception[2] );\n"
          "finally: closeallfiles();\n"
          "if normalexit=0 then `quit`(0); else `quit(1)` end if: end "
          "try:\n";
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareIsoclinesFile
// -----------------------------------------------------------------------
void P4InputVF::prepareIsoclinesFile(QTextStream &fp)
{
    QString name_isoclinestab;
    QString s;

    QString mainmaple;
    QString user_bindir;
    QString user_tmpdir;
    QString user_platform;
    QString user_removecmd;
    QString user_simplify;
    QString user_simplifycmd;
    QString user_sumtablepath;
    QString user_exeprefix;

    QByteArray ba_mainmaple;
    QByteArray ba_user_bindir;
    QByteArray ba_user_tmpdir;
    QByteArray ba_name_isoclinestab;

    user_exeprefix = "";

    mainmaple = getP4MaplePath();
    user_bindir = getP4BinPath();
    user_tmpdir = getP4TempPath();
    user_sumtablepath = getP4SumTablePath();

    mainmaple += QDir::separator();
    if (user_bindir != "")
        user_bindir += QDir::separator();
    if (user_tmpdir != "")
        user_tmpdir += QDir::separator();
    user_platform = USERPLATFORM;
#ifdef Q_OS_WIN
    user_removecmd = "cmd /c del";
    user_exeprefix = "cmd /c ";
#else
    user_removecmd = "rm";
    user_exeprefix = "";
#endif
    mainmaple += MAINMAPLEFILE;

    ba_mainmaple = maplepathformat(mainmaple);
    ba_user_bindir = maplepathformat(user_bindir);
    ba_user_tmpdir = maplepathformat(user_tmpdir);

    if (numeric_[0])
        user_simplify = "false";
    else
        user_simplify = "true";

    user_simplifycmd = MAPLE_SIMPLIFY_EXPRESSIONS;

    fp << "restart;\n";
    fp << "read( \"" << ba_mainmaple << "\" );\n";
    fp << "user_bindir := \"" << ba_user_bindir << "\":\n";
    fp << "user_tmpdir := \"" << ba_user_tmpdir << "\":\n";
    fp << "user_exeprefix := \"" << user_exeprefix << "\":\n";
    fp << "user_platform := \"" << user_platform << "\":\n";
    fp << "user_removecmd := \"" << user_removecmd << "\":\n";
    fp << "user_simplify := " << user_simplify << ":\n";
    fp << "user_simplifycmd := " << user_simplifycmd << ":\n";
    fp << "all_crit_points := " << typeofstudy_ << ":\n";

    name_isoclinestab = getfilename_isoclinestable();
    removeFile(name_isoclinestab);
    ba_name_isoclinestab = maplepathformat(name_isoclinestab);
    fp << "isoclines_table := \"" << ba_name_isoclinestab << "\":\n";

    prepareMapleIsoclines(fp);
    prepareMapleParameters(fp);

    fp << "try prepareIsoclines() catch:\n"
          "printf( \"! Error (\%a) \%a\\n\", lastexception[1], "
          "lastexception[2] );\n"
          "finally: closeallfiles();\n"
          "if normalexit=0 then `quit`(0); else `quit(1)` end if: end "
          "try:\n";
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluate
// -----------------------------------------------------------------------

void P4InputVF::evaluate()
{
    QString filedotmpl;
    QString s, e;
    QProcess *proc;

    // possible clean up after last GCF evaluation
    if (evalProcess_ != nullptr) {
        delete evalProcess_;
        evalProcess_ = nullptr;
    }

    prepare();

    filedotmpl = getmaplefilename();

    s = getMapleExe();
    if (s.isEmpty())
        s = "";
    else
        s = s.append(" \"").append(filedotmpl).append("\"");

    if (outputWindow_ == nullptr || processText_ == nullptr)
        createProcessWindow();
    else {
        processText_->append(
            "\n\n--------------------------------------"
            "-----------------------------------------"
            "\n\n");
        terminateProcessButton_->setEnabled(true);
        outputWindow_->show();
        outputWindow_->raise();
    }

    proc = new QProcess(this);
    proc->setWorkingDirectory(QDir::currentPath());

    connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
            g_p4app, &P4Application::signalEvaluated);
    connect(proc, &QProcess::readyReadStandardOutput, this,
            &P4InputVF::readProcessStdout);
#ifdef QT_QPROCESS_OLD
    connect(proc,
            static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                &QProcess::error),
            this, &P4InputVF::catchProcessError);
#else
    connect(proc, &QProcess::errorOccurred, this,
            &P4InputVF::catchProcessError);
#endif

    processfailed_ = false;
    QString pa = "External Command: " + getMapleExe() + " " + filedotmpl;
    processText_->append(pa);
    proc->start(getMapleExe(), QStringList(filedotmpl), QIODevice::ReadWrite);

    if (proc->state() != QProcess::Running &&
        proc->state() != QProcess::Starting) {
        processfailed_ = true;
        delete proc;
        proc = nullptr;
        evalProcess_ = nullptr;
        evalFile_ = "";
        evalFile2_ = "";
        g_p4app->signalEvaluated(-1);
        terminateProcessButton_->setEnabled(false);
    } else {
        evalProcess_ = proc;
        evalFile_ = filedotmpl;
        evalFile2_ = "";
        evaluating_ = true;
        evaluatingGcf_ = false;
        evaluatingPiecewiseConfig_ = false;
        evaluatingCurve_ = false;
        evaluatingIsoclines_ = false;
    }
}
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluateCurveTable
// -----------------------------------------------------------------------
void P4InputVF::evaluateCurveTable()
{
    QString filedotmpl;
    QString s, e;
    QProcess *proc;

    // possible clean up after last evaluation
    if (evalProcess_ != nullptr) {
        delete evalProcess_;
        evalProcess_ = nullptr;
    }

    prepareCurve();
    filedotmpl = getPrepareCurveFileName();

    s = getMapleExe();
    if (s.isEmpty())
        s = "";
    else
        s = s.append(" \"").append(filedotmpl).append("\"");

    /* Here a window for displaying the output text of the Maple process
     * is created */
    if (outputWindow_ == nullptr)
        createProcessWindow();
    else {
        processText_->append(
            "\n\n--------------------------------------"
            "-----------------------------------------"
            "\n\n");
        terminateProcessButton_->setEnabled(true);
        outputWindow_->show();
        outputWindow_->raise();
    }

    proc = new QProcess(this);
    proc->setWorkingDirectory(QDir::currentPath());
    connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
            g_p4app, &P4Application::signalCurveEvaluated);
    connect(proc, &QProcess::readyReadStandardOutput, this,
            &P4InputVF::readProcessStdout);
#ifdef QT_QPROCESS_OLD
    connect(proc,
            static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                &QProcess::error),
            g_p4app, &P4Application::catchProcessError);
#else
    connect(proc, &QProcess::errorOccurred, g_p4app,
            &P4Application::catchProcessError);
#endif

    processfailed_ = false;
    QString pa = "External Command: " + getMapleExe() + " " + filedotmpl;
    processText_->append(pa);
    proc->start(getMapleExe(), QStringList(filedotmpl), QIODevice::ReadWrite);

    if (proc->state() != QProcess::Running &&
        proc->state() != QProcess::Starting) {
        processfailed_ = true;
        delete proc;
        proc = nullptr;
        evalProcess_ = nullptr;
        evalFile_ = "";
        evalFile2_ = "";
        g_p4app->signalEvaluated(-1);
        terminateProcessButton_->setEnabled(false);
    } else {
        evalProcess_ = proc;
        evalFile_ = filedotmpl;
        evalFile2_ = "";
        evaluating_ = true;
        evaluatingGcf_ = false;
        evaluatingPiecewiseConfig_ = false;
        evaluatingCurve_ = true;
        evaluatingIsoclines_ = false;
    }
}
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluateIsoclinesTable
// -----------------------------------------------------------------------
void P4InputVF::evaluateIsoclinesTable()
{
    QString filedotmpl;
    QString s, e;
    // QProcess *proc;

    evaluatingGcf_ = false;
    evaluatingCurve_ = false;
    evaluatingIsoclines_ = false;
    // possible clean up after last Curve evaluation
    if (evalProcess_ != nullptr) {
        delete evalProcess_;
        evalProcess_ = nullptr;
    }

    prepareIsoclines();
    filedotmpl = getPrepareIsoclinesFileName();

    s = getMapleExe();
    if (s.isNull()) {
        s = "";
    } else {
        s = s.append(" ");
        if (filedotmpl.contains(' ')) {
            s = s.append("\"");
            s = s.append(filedotmpl);
            s = s.append("\"");
        } else
            s = s.append(filedotmpl);

        /* Here a window for displaying the output text of the Maple process
         * is created */
        if (outputWindow_ == nullptr || processText_ == nullptr)
            createProcessWindow();
        else {
            processText_->append(
                "\n\n--------------------------------------"
                "-----------------------------------------"
                "\n\n");
            terminateProcessButton_->setEnabled(true);
            outputWindow_->show();
            outputWindow_->raise();
        }

        QProcess *proc;
        if (evalProcess_ != nullptr) {  // re-use process of last GCF
            proc = evalProcess_;
            disconnect(proc, SIGNAL(finished(int)), g_p4app, 0);
            connect(proc,
                    static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                    g_p4app, &P4Application::signalCurveEvaluated);
        } else {
            proc = new QProcess(this);
            connect(proc,
                    static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                    g_p4app, &P4Application::signalCurveEvaluated);
            connect(proc, &QProcess::readyReadStandardOutput, this,
                    &P4InputVF::readProcessStdout);
#ifdef QT_QPROCESS_OLD
            connect(proc,
                    static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                        &QProcess::error),
                    g_p4app, &P4Application::catchProcessError);
#else
            connect(proc, &QProcess::errorOccurred, g_p4app,
                    &P4Application::catchProcessError);
#endif
        }

        proc->setWorkingDirectory(QDir::currentPath());

        processfailed_ = false;
        QString pa = "External Command: ";
        pa += getMapleExe();
        pa += " ";
        pa += filedotmpl;
        processText_->append(pa);
        proc->start(getMapleExe(), QStringList(filedotmpl),
                    QIODevice::ReadWrite);

        if (proc->state() != QProcess::Running &&
            proc->state() != QProcess::Starting) {
            processfailed_ = true;
            delete proc;
            proc = nullptr;
            evalProcess_ = nullptr;
            evalFile_ = "";
            evalFile2_ = "";
            g_p4app->signalEvaluated(-1);
            terminateProcessButton_->setEnabled(false);
        } else {
            evalProcess_ = proc;
            evalFile_ = filedotmpl;
            evalFile2_ = "";
        }
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::catchProcessError
// -----------------------------------------------------------------------
void P4InputVF::catchProcessError(QProcess::ProcessError prerr)
{
    processfailed_ = true;
    switch (prerr) {
        case QProcess::FailedToStart:
            processError_ = "Failed to start";
            break;
        case QProcess::Crashed:
            processError_ = "Crash";
            break;
        case QProcess::Timedout:
            processError_ = "Time-out";
            break;
        case QProcess::WriteError:
        case QProcess::ReadError:
        case QProcess::UnknownError:
        default:
            processError_ = "Unknown error";
            break;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::finishEvaluation
// -----------------------------------------------------------------------
void P4InputVF::finishEvaluation(int exitCode)
{
    UNUSED(exitCode);
    if (!evalFile_.isNull() && evalFile_ != "") {
        removeFile(evalFile_);
        evalFile_ = "";
    }

    if (!evalFile2_.isNull() && evalFile2_ != "") {
        removeFile(evalFile2_);
        evalFile2_ = "";
    }

    if (terminateProcessButton_ != nullptr)
        terminateProcessButton_->setEnabled(false);

    if (evalProcess_ != nullptr) {
        if (processText_ != nullptr) {
            outputWindow_->show();
            outputWindow_->raise();
            QString buf;
            buf =
                "\n----------------------------------------------------------"
                "----"
                "-----------------\n";
            processText_->append(buf);
            if (evalProcess_ != nullptr) {
                if (evalProcess_->state() == QProcess::Running) {
                    evalProcess_->terminate();
                    QTimer::singleShot(5000, evalProcess_, SLOT(kill()));
                    buf = "Kill signal sent to process.\n";
                } else {
                    if (!processfailed_)
                        buf.sprintf("The process finished normally (%d)\n",
                                    evalProcess_->exitCode());
                    else {
                        buf.sprintf("The process stopped abnormally (%d : ",
                                    evalProcess_->exitCode());
                        buf += processError_;
                        buf += ")\n";
                    }
                }
            } else {
                if (processfailed_)
                    buf =
                        "The following error occured: " + processError_ + "\n";
                else
                    buf = "";
            }
            processText_->append(buf);
        }
    }
    /*if (processText_ != nullptr) {
        //      processText_->hide();
        if (processText_->isActiveWindow()) {
            if (!evaluatingGcf_)
                g_p4stardlg->activateWindow();
            else {
                if (gcfDlg != nullptr)
                    gcfDlg->activateWindow();
            }
        }
    }*/

    if (evaluatingGcf_)
        finishGcfEvaluation();
    if (evaluatingCurve_)
        finishCurveEvaluation();
    if (evaluatingIsoclines_)
        finishIsoclinesEvaluation();
    if (evaluatingPiecewiseConfig_)
        finishCurvesEvaluation();
}

// -----------------------------------------------------------------------
//          P4InputVF::finishGcfEvaluation
// -----------------------------------------------------------------------
void P4InputVF::finishGcfEvaluation()
{
    evaluatingGcf_ = false;
    if (gcfDlg_ != nullptr) {
        gcfDlg_->finishGcfEvaluation();

        //        QP4Event * e = new QP4Event(
        //        (QEvent::Type)TYPE_SIGNAL_EVALUATED, nullptr );
        //        g_p4app->postEvent( g_p4stardlg, e );
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::finishCurveEvaluation
// -----------------------------------------------------------------------
void P4InputVF::finishCurveEvaluation()
{
    evaluatingCurve_ = false;
    if (curveDlg_ != nullptr) {
        curveDlg_->finishCurveEvaluation();
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::finishIsoclinesEvaluation
// -----------------------------------------------------------------------
void P4InputVF::finishIsoclinesEvaluation()
{
    evaluatingIsoclines_ = false;
    if (isoclinesDlg_ != nullptr) {
        isoclinesDlg_->finishIsoclinesEvaluation();
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepare
// -----------------------------------------------------------------------
void P4InputVF::prepare()
{
    QString filedotmpl = getmaplefilename();
    QFile file(QFile::encodeName(filedotmpl));
    if (file.open(QFile::WriteOnly)) {
        QTextStream fp(&file);
        prepareFile(fp);
        fp.flush();
        file.close();
    }
    // TODO: what if P4 cannot open the file?
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareCurve
// -----------------------------------------------------------------------
void P4InputVF::prepareCurve()
{
    QString filedotmpl = getPrepareCurveFileName();
    QFile file(QFile::encodeName(filedotmpl));
    if (file.open(QFile::WriteOnly)) {
        QTextStream fp(&file);
        prepareCurveFile(fp);
        fp.flush();
        file.close();
    }
    // TODO: what if P4 cannot open the file?
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareIsoclines
// -----------------------------------------------------------------------
void P4InputVF::prepareIsoclines()
{
    QString filedotmpl = getPrepareIsoclinesFileName();
    QFile file(QFile::encodeName(filedotmpl));
    if (file.open(QFile::WriteOnly)) {
        QTextStream fp(&file);
        prepareCurveFile(fp);
        fp.flush();
        file.close();
    }
    // TODO: what if P4 cannot open the file?
}

// -----------------------------------------------------------------------
//          P4InputVF::readProcessStdout
// -----------------------------------------------------------------------
void P4InputVF::readProcessStdout()
{
    QByteArray t;
    QByteArray line;
    int i, j;

    if (evalProcess_ == nullptr || processText_ == nullptr)
        return;

    while (1) {
        t = evalProcess_->readAllStandardOutput();
        if (t.length() == 0)
            break;

        i = t.indexOf('\n');
        j = t.indexOf('\r');
        while (i >= 0 || j >= 0) {
            if ((j < i && j != -1) || i == -1) {
                line = t.left(j);
                if (i == j + 1)
                    t = t.mid(j + 2);
                else
                    t = t.mid(j + 1);  // treat CR+LF as one lineend
            } else {
                line = t.left(i);
                t = t.mid(i + 1);
            }
            processText_->append(line);
            i = t.indexOf('\n');
            j = t.indexOf('\r');
        }
        if (t.length() != 0)
            processText_->append(t);
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::onTerminateButton
// -----------------------------------------------------------------------
void P4InputVF::onTerminateButton()
{
    QString buf;
    if (evalProcess_ != nullptr) {
        if (evalProcess_->state() == QProcess::Running) {
            buf =
                "\n----------------------------------------------------------"
                "---------------------\n";
            processText_->append(buf);
            evalProcess_->terminate();
            QTimer::singleShot(2000, evalProcess_, SLOT(kill()));
            buf = "Kill signal sent to process.\n";
            processText_->append(buf);
            processfailed_ = true;
            processError_ = "Terminated by user";
        }
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::createProcessWindow
// -----------------------------------------------------------------------
void P4InputVF::createProcessWindow()
{
    if (outputWindow_ != nullptr) {
        if (outputWindow_->isVisible() == false)
            outputWindow_->show();
        outputWindow_->activateWindow();
        outputWindow_->raise();
        return;
    }

    outputWindow_ = new QWidget(nullptr);
    outputWindow_->setWindowTitle("Output window");
    outputWindow_->resize(530, 344);
    if (g_p4smallicon != nullptr)
        outputWindow_->setWindowIcon(*g_p4smallicon);

    QVBoxLayout *vLayout = new QVBoxLayout(outputWindow_);
    vLayout->setSpacing(3);
    vLayout->setContentsMargins(5, 5, 5, 5);
    QVBoxLayout *vLayout2 = new QVBoxLayout();
    vLayout2->setSpacing(3);

    processText_ = new QTextEdit(outputWindow_);
    processText_->setLineWrapMode(QTextEdit::FixedColumnWidth);
    processText_->setWordWrapMode(QTextOption::WrapAnywhere);
    processText_->setFont(*(g_p4app->courierFont_));
    processText_->setLineWrapColumnOrWidth(82);
    processText_->setReadOnly(true);
    processText_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vLayout2->addWidget(processText_);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setSpacing(6);

    hLayout->addStretch();
    terminateProcessButton_ = new QPushButton("Terminate");
    terminateProcessButton_->setFont(*(g_p4app->boldFont_));
    terminateProcessButton_->setToolTip(
        "Terminates the process.  First tries to send a "
        "safe signal to the process.\nIf this does not "
        "work, then after 2 seconds, the program is "
        "abruptly terminated.");
    hLayout->addWidget(terminateProcessButton_);

    clearProcessButton_ = new QPushButton("Clear");
    clearProcessButton_->setFont((*g_p4app->boldFont_));
    clearProcessButton_->setToolTip("Clears this window");
    hLayout->addWidget(clearProcessButton_);

    vLayout2->addLayout(hLayout);
    vLayout->addLayout(vLayout2);

    outputWindow_->show();

    QObject::connect(terminateProcessButton_, &QPushButton::clicked, this,
                     &P4InputVF::onTerminateButton);
    QObject::connect(clearProcessButton_, &QPushButton::clicked, this, [=]() {
        if (processText_ != nullptr)
            processText_->clear();
    });
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluateGcf
// -----------------------------------------------------------------------
bool P4InputVF::evaluateGcf()
{
    // NOTICE: the code for reduce implementations can be found in older
    // versions of P4, it is removed from 7.0.2 onwards
    QString filedotmpl = getmaplefilename();

    // QString s = getMapleExe().append(" \"").append(filedotmpl).append("\"");

    if (processText_ == nullptr)
        createProcessWindow();
    else {
        processText_->append(
            "\n\n------------------------------------------"
            "-------------------------------------\n\n");
        terminateProcessButton_->setEnabled(true);
        outputWindow_->show();
        outputWindow_->raise();
    }

    QProcess *proc;
    if (evalProcess_ != nullptr) {  // re-use process of last GCF
        proc = evalProcess_;
        disconnect(proc, SIGNAL(finished(int)), g_p4app, 0);
        connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                g_p4app, &P4Application::signalCurveEvaluated);
    } else {
        proc = new QProcess(this);
        connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                g_p4app, &P4Application::signalCurveEvaluated);
#ifdef QT_QPROCESS_OLD
        connect(proc,
                static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                    &QProcess::error),
                g_p4app, &P4Application::catchProcessError);
#else
        connect(proc, &QProcess::errorOccurred, g_p4app,
                &P4Application::catchProcessError);
#endif
        connect(proc, &QProcess::readyReadStandardOutput, this,
                &P4InputVF::readProcessStdout);
    }

    proc->setWorkingDirectory(QDir::currentPath());

    processfailed_ = false;
    QString pa = "External Command: ";
    pa += getMapleExe();
    pa += " ";
    pa += filedotmpl;
    processText_->append(pa);
    proc->start(getMapleExe(), QStringList(filedotmpl), QIODevice::ReadWrite);
    if (proc->state() != QProcess::Running &&
        proc->state() != QProcess::Starting) {
        processfailed_ = true;
        delete proc;
        proc = nullptr;
        evalProcess_ = nullptr;
        evalFile_ = "";
        evalFile2_ = "";
        g_p4app->signalCurveEvaluated(-1);
        terminateProcessButton_->setEnabled(false);
        return false;
    } else {
        evalProcess_ = proc;
        evalFile_ = filedotmpl;
        evalFile2_ = "";
        evaluatingGcf_ = true;
        evaluating_ = true;
        evaluatingPiecewiseConfig_ = false;
        return true;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareGcf
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating GCF in plane/U1/U2 charts.  This
// is only called in case of Poincare-compactification (weights p=q=1)

bool P4InputVF::prepareGcf(P4POLYNOM2 f, double y1, double y2, int precision,
                           int numpoints)
{
    int i;
    QFile file(QFile::encodeName(getmaplefilename()));
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&file);

        QString mainmaple =
            getP4MaplePath().append(QDir::separator()).append(MAINMAPLEGCFFILE);
        QByteArray ba_mainmaple = maplepathformat(mainmaple);

        QString user_file = getfilename_gcf();
        QByteArray ba_user_file = maplepathformat(user_file);
        removeFile(user_file);

        QString user_platform = USERPLATFORM;

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << ba_user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := " << -1.0 << ":\n";
        out << "user_x2 := " << 1.0 << ":\n";
        out << "user_y1 := " << y1 << ":\n";
        out << "user_y2 := " << y2 << ":\n";
        out << "u := x:\n";
        out << "v := y:\n";
        out << "user_f := ");
        for (i = 0; f != nullptr; i++) {
            out << printterm2(buf, f, (i == 0) ? true : false, "x", "y");
            f = f->next_term2;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
            << "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareGcf_LyapunovCyl
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating GCF in charts near infinity.  This
// is only called in case of Poincare-Lyapunov compactification (weights (p,q)
// !=(1,1))

bool P4InputVF::prepareGcf_LyapunovCyl(double theta1, double theta2,
                                       int precision, int numpoints, int index)
{
    int i;
    QFile file(QFile::encodeName(getmaplefilename()));
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&file);

        P4POLYNOM f = g_VFResults.vf_[index]->gcf_C_;

        QString mainmaple =
            getP4MaplePath().append(QDir::separator()).append(MAINMAPLEGCFFILE);
        QByteArray ba_mainmaple = maplepathformat(mainmaple);

        QString user_file = getfilename_gcf();
        removeFile(user_file);
        QByteArray ba_user_file = maplepathformat(user_file);

        QString user_platform = USERPLATFORM;

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << ba_user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := " << 0.0 << ":\n";
        out << "user_x2 := " << 1.0 << ":\n";
        out << "user_y1 := " << theta1 << ":\n";
        out << "user_y2 := " << theta2 << ":\n";
        out << "u := cos(y):\n";
        out << "v := sin(y):\n";
        out << "user_f := ";
        for (i = 0; f != nullptr; i++) {
            out << printterm3(buf, f, (i == 0) ? true : false, "x", "U", "V");
            f = f->next_term3;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
            << "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareGcf_LyapunovR2
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating GCF in charts near infinity.  This
// is only called in case of Poincare-Lyapunov compactification (weights (p,q)
// !=(1,1))
//
// same as preparegcf, except for the "u := " and "v := " assignments,
// and the fact that one always refers to the same function g_VFResults.gcf_,
// and the fact that the x and y intervals are [0,1] and [0,2Pi] resp.

bool P4InputVF::prepareGcf_LyapunovR2(int precision, int numpoints, int index)
{
    int i;
    QFile file(QFile::encodeName(getmaplefilename()));
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&file);

        P4POLYNOM2 f = g_VFResults.vf_[index]->gcf_;

        QString mainmaple =
            getP4MaplePath().append(QDir::separator()).append(MAINMAPLEGCFFILE);
        QByteArray ba_mainmaple = maplepathformat(mainmaple);

        QString user_file = getfilename_gcf();
        QByteArray ba_user_file = maplepathformat(user_file);
        removeFile(user_file);

        QString user_platform = USERPLATFORM;

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << ba_user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := 0.0:\n", (float)0.0);
        out << "user_x2 := 1.0:\n", (float)1.0);
        out << "user_y1 := 0.0:\n", (float)0.0);
        out << "user_y2 := " << TWOPI << ":\n";
        ;
        out << "u := x*cos(y):\n";
        out << "v := x*sin(y):\n";
        out << "user_f := ";
        for (int i = 0; f != nullptr; i++) {
            out << printterm2(buf, f, (i == 0) ? true : false, "U", "V");
            f = f->next_term2;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
            << "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluateCurve
// -----------------------------------------------------------------------
bool P4InputVF::evaluateCurve()
{
    QString filedotmpl;
    QString s;

    filedotmpl = getmaplefilename();

    s = getMapleExe();
    if (s.isEmpty()) {
        s = "";
    } else {
        s = s.append(" \"").append(filedotmpl).append("\"");
    }

    if (processText_ == nullptr)
        createProcessWindow();
    else {
        processText_->append(
            "\n\n------------------------------------------"
            "-------------------------------------\n\n");
        terminateProcessButton_->setEnabled(true);
        outputWindow_->show();
        outputWindow_->raise();
    }

    QProcess *proc;
    // re-use process of last GCF
    if (evalProcess_ != nullptr) {
        proc = evalProcess_;
        disconnect(proc, SIGNAL(finished(int)), g_p4app, 0);
        connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                g_p4app, &P4Application::signalCurveEvaluated);
    } else {
        proc = new QProcess(this);
        connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                g_p4app, &P4Application::signalCurveEvaluated);
#ifdef QT_QPROCESS_OLD
        connect(proc,
                static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                    &QProcess::error),
                g_p4app, &P4Application::catchProcessError);
#else
        connect(proc, &QProcess::errorOccurred, g_p4app,
                &P4Application::catchProcessError);
#endif
        connect(proc, &QProcess::readyReadStandardOutput, this,
                &P4InputVF::readProcessStdout);
    }

    proc->setWorkingDirectory(QDir::currentPath());

    processfailed_ = false;
    QString pa = "External Command: ";
    pa += getMapleExe();
    pa += " ";
    pa += filedotmpl;
    processText_->append(pa);
    proc->start(getMapleExe(), QStringList(filedotmpl), QIODevice::ReadWrite);
    if (proc->state() != QProcess::Running &&
        proc->state() != QProcess::Starting) {
        processfailed_ = true;
        delete proc;
        proc = nullptr;
        evalProcess_ = nullptr;
        evalFile_ = "";
        evalFile2_ = "";
        g_p4app->signalCurveEvaluated(-1);
        terminateProcessButton_->setEnabled(false);
        return false;
    } else {
        evalProcess_ = proc;
        evalFile_ = filedotmpl;
        evalFile2_ = "";
        evaluatingCurve_ = true;

        return true;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareCurve
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating curve in plane/U1/U2 charts.  This
// is only called in case of Poincare-compactification (weights p=q=1)
bool P4InputVF::prepareCurve(P4POLYNOM2 f, double y1, double y2, int precision,
                             int numpoints)
{
    int i;
    QFile file(QFile::encodeName(getmaplefilename()));
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QString mainmaple =
            getP4MaplePath().append(QDir::separator()).append(MAINMAPLEGCFFILE);
        QByteArray ba_mainmaple = maplepathformat(mainmaple);

        QString user_platform = USERPLATFORM;
        QString user_file = getfilename_curve();
        QByteArray ba_user_file = maplepathformat(user_file);
        removeFile(user_file);

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << ba_user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := -1.0:\n";
        out << "user_x2 := 1.0:\n";
        out << "user_y1 := " << y1 << ":\n";
        out << "user_y2 := " << y2 << ":\n";
        out << "u := x:\n";
        out << "v := y:\n";
        out << "user_f := ";
        for (i = 0; f != nullptr; i++) {
            out << printterm2(buf, f, (i == 0) ? true : false, "x", "y");
            f = f->next_term2;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
            << "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();

        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareCurve_LyapunovCyl
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating a curve in charts near infinity.  This
// is only called in case of Poincare-Lyapunov compactification (weights (p,q)
// !=(1,1))
bool P4InputVF::prepareCurve_LyapunovCyl(double theta1, double theta2,
                                         int precision, int numpoints,
                                         int index)
{
    int i;
    QFile file(QFile::encodeName(getmaplefilename()));
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&file);

        P4POLYNOM3 f = g_VFResults.vf_[index]->curve_vector_.back().c;

        QString mainmaple =
            getP4MaplePath().append(QDir::separator).append(MAINMAPLEGCFFILE);
        QByteArray ba_mainmaple = maplepathformat(mainmaple);

        QString user_platform = USERPLATFORM;

        QString user_file = getfilename_curve();
        QByteArray ba_user_file = maplepathformat(user_file);
        removeFile(user_file);

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << ba_user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := 0.0:\n";
        out << "user_x2 := 1.0:\n";
        out << "user_y1 := " << theta1 << ":\n";
        out << "user_y2 := " << theta2 << ":\n";
        out << "u := cos(y):\n";
        out << "v := sin(y):\n";
        out << "user_f := ";
        for (i = 0; f != nullptr; i++) {
            out << printterm3(buf, f, (i == 0) ? true : false, "x", "U", "V");
            f = f->next_term3;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
               "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareCurve_LyapunovR2
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating a curve in charts near infinity.  This
// is only called in case of Poincare-Lyapunov compactification (weights (p,q)
// !=(1,1))
//
// same as preparegcf, except for the "u := " and "v := " assignments,
// and the fact that one always refers to the same function g_VFResults.gcf_,
// and the fact that the x and y intervals are [0,1] and [0,2Pi] resp.
bool P4InputVF::prepareCurve_LyapunovR2(int precision, int numpoints, int index)
{
    int i;
    QFile file(QFile::encodeName(getmaplefilename()));
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&file);

        P4POLYNOM2 f = g_VFResults.vf_[index]->curve_vector_.back().r2;

        QString mainmaple =
            getP4MaplePath().append(QDir::separator).append(MAINMAPLEGCFFILE);
        QByteArray ba_mainmaple = maplepathformat(mainmaple);

        QString user_platform = USERPLATFORM;

        QString user_file = getfilename_curve();
        QByteArray ba_user_file = maplepathformat(user_file);
        removeFile(user_file);

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << ba_user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := 0.0:\n";
        out << "user_x2 := 1.0:\n";
        out << "user_y1 := 0.0:\n";
        out << "user_y2 := " << TWOPI << ":\n";
        out << "u := x*cos(y):\n";
        out << "v := x*sin(y):\n";
        out << "user_f := ";
        for (i = 0; f != nullptr; i++) {
            out << printterm2(buf, f, (i == 0) ? true : false, "U", "V");
            f = f->next_term2;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
               "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluateIsoclines
// -----------------------------------------------------------------------
bool P4InputVF::evaluateIsoclines()
{
    QString filedotmpl;
    QString s;
    QProcess *proc;

    evaluatingGcf_ = false;
    evaluating

        filedotmpl = getmaplefilename();

    s = getMapleExe();
    s = s.append(" ");
    if (filedotmpl.contains(' ')) {
        s = s.append("\"");
        s = s.append(filedotmpl);
        s = s.append("\"");
    } else
        s = s.append(filedotmpl);

    if (processText_ == nullptr)
        createProcessWindow();
    else {
        processText_->append(
            "\n\n------------------------------------------"
            "-------------------------------------\n\n");
        terminateProcessButton_->setEnabled(true);
        outputWindow_->show();
        outputWindow_->raise();
    }

    if (evalProcess_ != nullptr) {  // re-use process of last GCF
        proc = evalProcess_;
        disconnect(proc, SIGNAL(finished(int)), g_p4app, 0);
        connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                g_p4app, &P4Application::signalCurveEvaluated);
    } else {
        proc = new QProcess(this);
        connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                g_p4app, &P4Application::signalCurveEvaluated);
#ifdef QT_QPROCESS_OLD
        connect(proc,
                static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                    &QProcess::error),
                g_p4app, &P4Application::catchProcessError);
#else
        connect(proc, &QProcess::errorOccurred, g_p4app,
                &P4Application::catchProcessError);
#endif
        connect(proc, &QProcess::readyReadStandardOutput, this,
                &P4InputVF::readProcessStdout);
    }

    proc->setWorkingDirectory(QDir::currentPath());

    processfailed_ = false;
    QString pa = "External Command: ";
    pa += getMapleExe();
    pa += " ";
    pa += filedotmpl;
    processText_->append(pa);
    proc->start(getMapleExe(), QStringList(filedotmpl), QIODevice::ReadWrite);
    if (proc->state() != QProcess::Running &&
        proc->state() != QProcess::Starting) {
        processfailed_ = true;
        delete proc;
        proc = nullptr;
        evalProcess_ = nullptr;
        evalFile_ = "";
        evalFile2_ = "";
        g_p4app->signalCurveEvaluated(-1);
        terminateProcessButton_->setEnabled(false);
        return false;
    } else {
        evalProcess_ = proc;
        evalFile_ = filedotmpl;
        evalFile2_ = "";
        evaluatingIsoclines_ = true;

        return true;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareIsoclines
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating isoclines in plane/U1/U2 charts.
// This is only called in case of Poincare-compactification (weights p=q=1)
bool P4InputVF::prepareIsoclines(P4POLYNOM2 f, double y1, double y2,
                                 int precision, int numpoints)
{
    int i;
    QFile file(QFile::encodeName(getmaplefilename()));
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&file);

        QString mainmaple =
            getP4MaplePath().append(QDir::separator()).append(MAINMAPLEGCFFILE);
        QByteArray ba_mainmaple = maplepathformat(mainmaple);

        QString user_platform = USERPLATFORM;

        QString user_file = getfilename_isoclines();
        QByteArray ba_user_file = maplepathformat(user_file);
        removeFile(user_file);

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := 1.0:\n";
        out << "user_x2 := -1.0:\n";
        out << "user_y1 := " << y1 << ":\n";
        out << "user_y2 := " << y2 << ":\n";
        out << "u := x:\n";
        out << "v := y:\n";
        out << "user_f := ";
        for (i = 0; f != nullptr; i++) {
            out << printterm2(buf, f, (i == 0) ? true : false, "x", "y");
            f = f->next_term2;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
            << "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareIsoclines_LyapunovCyl
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating a curve in charts near infinity.  This
// is only called in case of Poincare-Lyapunov compactification (weights (p,q)
// !=(1,1))
bool P4InputVF::prepareIsoclines_LyapunovCyl(double theta1, double theta2,
                                             int precision, int numpoints,
                                             int index)
{
    int i;
    QFile file(QFile::encodeName(getmaplefilename()));
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&file);

        P4POLYNOM3 f = g_VFResults.vf_[index]->isocline_vector_.back().c;

        QString mainmaple =
            getP4MaplePath().append(QDir::separator()).append(MAINMAPLEGCFFILE);
        QByteArray ba_mainmaple = maplepathformat(mainmaple);

        QString user_platform = USERPLATFORM;

        QString user_file = getfilename_isoclines();
        QByteArray ba_user_file = maplepathformat(user_file);
        removeFile(user_file);

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := 0.0:\n";
        out << "user_x2 := 1.0:\n";
        out << "user_y1 := " << theta1 << ":\n";
        out << "user_y2 := " << theta2 << ":\n";
        out << "u := cos(y):\n";
        out << "v := sin(y):\n";
        out << "user_f := ";
        for (i = 0; f != nullptr; i++) {
            out << printterm3(buf, f, (i == 0) ? true : false, "x", "U", "V");
            f = f->next_term3;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
            << "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//              PREPAREISOCLINES_LYAPUNOVR2
// -----------------------------------------------------------------------
//
// Prepare files in case of calculating an isocline in charts near infinity.
// This is only called in case of Poincare-Lyapunov compactification
// (weights (p,q) !=(1,1))
//
// same as preparegcf, except for the "u := " and "v := " assignments,
// and the fact that one always refers to the same function g_VFResults.gcf_,
// and the fact that the x and y intervals are [0,1] and [0,2Pi] resp.
bool P4InputVF::prepareIsoclines_LyapunovR2(int precision, int numpoints,
                                            int index)
{
    int i;
    QFile file(QFile::encodeName(getmaplefilename()));
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&file);

        P4POLYNOM2 f = g_VFResults.vf_[index]->isocline_vector_.back().r2;

        QString mainmaple =
            getP4MaplePath().append(QDir::separator()).append(MAINMAPLEGCFFILE);
        QByteArray ba_mainmaple = maplepathformat(mainmaple);

        QString user_platform = USERPLATFORM;

        QString user_file = getfilename_isoclines();
        QByteArray ba_user_file = maplepathformat(user_file);
        removeFile(user_file);

        out << "restart;\n";
        out << "read( \"" << ba_mainmaple << "\" );\n";
        out << "user_file := \"" << user_file << "\":\n";
        out << "user_numpoints := " << numpoints << ":\n";
        out << "Digits := " << precision << ":\n";
        out << "user_x1 := 0.0:\n";
        out << "user_x2 := 1.0:\n";
        out << "user_y1 := 0.0:\n";
        out << "user_y2 := " << TWOPI << ":\n";
        out << "u := x*cos(y):\n";
        out << "v := x*sin(y):\n";
        out << "user_f := ";
        for (i = 0; f != nullptr; i++) {
            out << printterm2(buf, f, (i == 0) ? true : false, "U", "V");
            f = f->next_term2;
        }
        if (i == 0)
            out << "0:\n";
        else
            out << ":\n";

        out << "try FindSingularities() finally: if returnvalue=0 then "
            << "`quit`(0); else `quit(1)` end if: end try:\n";

        out.flush();
        file.close();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
//              COMMON SETTINGS ROUTINES
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
//          P4InputVF::hasCommonString
// -----------------------------------------------------------------------
bool P4InputVF::hasCommonString(std::vector<QString> lst)
{
    for (int i = 1; i < numSelected_; i++) {
        if (lst[selected_[0]].compare(lst[selected_[i]]))
            return false;
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::hasCommonInt
// -----------------------------------------------------------------------
bool P4InputVF::hasCommonInt(std::vector<int> lst)
{
    for (int i = 1; i < numSelected_; i++) {
        if (lst[selected_[i]] != lst[selected_[0]])
            return false;
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::hasCommonBool
// -----------------------------------------------------------------------
bool P4InputVF::hasCommonBool(std::vector<bool> lst)
{
    for (int i = 1; i < numSelected_; i++) {
        if (lst[selected_[i]] != lst[selected_[0]])
            return false;
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::hasCommonParvalue
// -----------------------------------------------------------------------
bool P4InputVF::hasCommonParvalue(int index)
{
    for (int i = 1; i < numSelected_; i++) {
        if (parvalue_[selected_[0]][index].compare(
                parvalue_[selected_[i]][index]))
            return false;
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::commonString
// -----------------------------------------------------------------------
QString P4InputVF::commonString(std::vector<QString> lst)
{
    return lst[selected_[0]];
}

// -----------------------------------------------------------------------
//          P4InputVF::commonInt
// -----------------------------------------------------------------------
int P4InputVF::commonInt(std::vector<int> lst) { return lst[selected_[0]]; }

// -----------------------------------------------------------------------
//          P4InputVF::commonBoolean
// -----------------------------------------------------------------------
bool P4InputVF::commonBoolean(std::vector<bool> lst)
{
    return lst[selected_[0]];
}

// -----------------------------------------------------------------------
//          P4InputVF::commonParvalue
// -----------------------------------------------------------------------
QString P4InputVF::commonParvalue(int index)
{
    return parvalue_[selected_[0]][index];
}

// -----------------------------------------------------------------------
//          P4InputVF::setCommonString
// -----------------------------------------------------------------------
void P4InputVF::setCommonString(std::vector<QString> lst, QString val)
{
    for (int i = 0; i < numSelected_; i++)
        lst[selected_[i]] = val;
}

// -----------------------------------------------------------------------
//          P4InputVF::setCommonInt
// -----------------------------------------------------------------------
void P4InputVF::setCommonInt(std::vector<int> lst, int val)
{
    for (int i = 0; i < numSelected_; i++)
        lst[selected_[i]] = val;
}

// -----------------------------------------------------------------------
//          P4InputVF::setCommonBool
// -----------------------------------------------------------------------
void P4InputVF::setCommonBool(std::vector<bool> lst, bool val)
{
    for (int i = 0; i < numSelected_; i++) {
        lst[selected_[i]] = val;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::setCommonParvalue
// -----------------------------------------------------------------------
void P4InputVF::setCommonParvalue(int index, QString val)
{
    for (int i = 0; i < numSelected_; i++) {
        j = selected_[i];
        parvalue_[selected_[i]][index] = val;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::addVectorField
// -----------------------------------------------------------------------
void P4InputVF::addVectorField()
{
    int i;

    if (!g_VFResults.vf_.empty()) {
        g_VFResults.clearVFs();
        g_VFResults.vfK_ = nullptr;
        g_VFResults.K_ = 0;
    }

    xdot_.push_back(QString(DEFAULTXDOT));
    ydot_.push_back(QString(DEFAULTYDOT));
    gcf_.push_back(QString(DEFAULTGCF));
    epsilon_.push_back(QString(DEFAULTEPSILON));
    numeric_.push_back(DEFAULTNUMERIC);
    testsep_.push_back(DEFAULTTESTSEP);
    precision_.push_back(DEFAULTPRECISION);
    taylorlevel_.push_back(DEFAULTLEVEL);
    numericlevel_.push_back(DEFAULTNUMLEVEL);
    maxlevel_.push_back(DEFAULTMAXLEVEL);
    weakness_.push_back(DEFAULTWEAKNESS);
    parvalue_.push_back(std::vector<QString>());
    for (i = 0; i < MAXNUMPARAMS; i++)
        parvalue_.back().push_back(QString());

    if (hasCommonString(xdot_))
        xdot_[numVF_] = commonString(xdot_);
    if (hasCommonString(ydot_))
        ydot_[numVF_] = commonString(ydot_);
    if (hasCommonString(gcf_))
        gcf_[numVF_] = commonString(gcf_);
    if (hasCommonString(epsilon_))
        epsilon_[numVF_] = commonString(epsilon_);
    if (hasCommonBool(numeric_))
        numeric_[numVF_] = commonBool(numeric_);
    if (hasCommonBool(testsep_))
        testsep_[numVF_] = commonBool(testsep_);
    if (hasCommonInt(precision_))
        precision_[numVF_] = commonInt(precision_);
    if (hasCommonInt(taylorlevel_))
        taylorlevel_[numVF_] = commonInt(taylorlevel_);
    if (hasCommonInt(numericlevel_))
        numericlevel_[numVF_] = commonInt(numericlevel_);
    if (hasCommonInt(maxlevel_))
        maxlevel_[numVF_] = commonInt(maxlevel_);
    if (hasCommonInt(weakness_))
        weakness_[numVF_] = commonInt(weakness_);

    for (i = 0; i < MAXNUMPARAMS; i++)
        if (hasCommonParvalue(i))
            parvalue_[numVF_][i] = commonParvalue(i);

    numVF_++;
}

// -----------------------------------------------------------------------
//          P4InputVF::deleteVectorField
// -----------------------------------------------------------------------
void P4InputVF::deleteVectorField(int index)  // FIXME
{
    // first disconnect from other structures

    int k, i;

    if (!g_VFResults.vf_.empty()) {
        g_VFResults.clearVFs();
        g_VFResults.vfK_ = nullptr;
        g_VFResults.K_ = 0;
    }

    for (auto it = vfRegions_.begin(); it != vfRegions_.end(); ++it) {
        if (it->vfIndex == index) {
            it->signs.clear();
            vfRegions_.erase(it);
            numVFRegions_--;
        } else if (it->vfIndex > index)
            it->vfIndex--;
    }

    for (auto it = selected_.begin(); it != selected_.end(); ++it) {
        if (*it == index) {
            selected_.erase(it);
            numSelected_--;
            if (numSelected_ == 0) {
                numSelected_ = 1;
                selected_[0] = index;
                if (selected_[0] >= numVF_ - 1)
                    selected_[0] = numVF_ - 2;
                if (selected_[0] < 0)
                    selected_[0] = 0;
                break;
            }
        } else if (*it > index)
            *it--;
    }

    if (numVF_ == 1) {
        // delete last one: replace by default startup values
        xdot_[0] = DEFAULTXDOT;
        ydot_[0] = DEFAULTYDOT;
        gcf_[0] = DEFAULTGCF;
        epsilon_[0] = DEFAULTEPSILON;
        numeric_[0] = DEFAULTNUMERIC;
        precision_[0] = DEFAULTPRECISION;
        testsep_[0] = DEFAULTTESTSEP;
        taylorlevel_[0] = DEFAULTLEVEL;
        numericlevel_[0] = DEFAULTNUMLEVEL;
        maxlevel_[0] = DEFAULTMAXLEVEL;
        weakness_[0] = DEFAULTWEAKNESS;
        for (k = 0; k < MAXNUMPARAMS; k++) {
            parvalue_[0][k] = QString();
            parlabel_[k] = QString();
        }

        numparams_ = 0;
        p_ = DEFAULTP;
        q_ = DEFAULTQ;
        // symbolicpackage = PACKAGE_MAPLE;
        typeofstudy_ = DEFAULTTYPE;
        x0_ = DEFAULTX0;
        y0_ = DEFAULTY0;
        /*
                defaulttypeofview = TYPEOFVIEW_SPHERE;
                defaultxmin = X_MIN;
                defaultxmax = X_MAX;
                defaultymin = Y_MIN;
                defaultymax = Y_MAX;
        */
    } else {
        xdot_.erase(xdot_.begin() + index);
        ydot_.erase(ydot_.begin() + index);
        gcf_.erase(gcf_.begin() + index);
        epsilon_.erase(epsilon_.begin() + index);
        parvalue_.erase(parvalue_.begin() + index);  // TODO: funcioinarà?

        numVF_--;
    }
    // TODO: segurament innecessari
    /*if (numVF_ == 1 && numVFRegions_ == 0 && numCurves_ == 0) {
        numVFRegions_ = 1;
        vfregions = (VFREGION *)malloc(sizeof(VFREGION));
        vfregions->vfindex = 0;
        vfregions->signs = NULL;
    }*/
}

// -----------------------------------------------------------------------
//          P4InputVF::addSeparatingCurve
// -----------------------------------------------------------------------
void P4InputVF::addSeparatingCurve()
{
    int i, n;

    if (!g_VFResults.curves_result_.empty()) {
        for (i = 0; i < numCurves_; i++)
            g_VFResults.resetCurveInfo(i);
        g_VFResults.curves_result_.clear();
    }

    n = numCurves_;
    numCurves_++;

    curves_.push_back(QString());
    numPointsCurve_.push_back(DEFAULT_CURVEPOINTS);

    for (i = 0; i < numVFRegions_; i++)
        vfregions[i].signs.push_back(1);

    for (i = 0; i < numCurveRegions_; i++)
        curveregions[i].signs.push_back(1);
}

// -----------------------------------------------------------------------
//          P4InputVF::deleteSeparatingCurve
// -----------------------------------------------------------------------
void P4InputVF::deleteSeparatingCurve(int index)
{
    int i;

    if (index < 0 || index >= numCurves_)
        return;

    if (!g_VFResults.curves_result_.empty()) {
        for (i = 0; i < numCurves_; i++)
            g_VFResults.resetCurveInfo(i);
        g_VFResults.curves_result_.clear();
    }

    if (numCurves_ == 1) {
        // special case

        numCurves_ = 0;
        curves_.clear();
        numPointsCurve_.clear();

        for (i = 0; i < numVFRegions_; i++)
            vfRegions_[i].signs.clear());
        for (i = 0; i < numCurveRegions_; i++)
            curveRegions_[i].signs.clear());
        if (numVFRegions_ == 0 && numVF_ == 1) {
            numVFRegions_ = 1;
            vfRegions_.clear();
            vfRegions_.push_back(
                p4InputVFRegions::vfRegion(0, std::vector<int>()));
        }
        return;
    }

    curves_.erase(curves_.begin() + index);
    numCurves_--;
}

// -----------------------------------------------------------------------
//          P4InputVF::prepareCurves (P5 version)
// -----------------------------------------------------------------------
void P4InputVF::prepareCurves()
{
    QFile file(QFile::encodeName(getmaplefilename()));
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream fp(&file);
        prepareFile(fp, true);
        fp.flush();
        file.close();
    }
    // TODO: what if P4 cannot open the file?
}

// -----------------------------------------------------------------------
//          P4InputVF::evaluateCurves (P5 version)
// -----------------------------------------------------------------------

bool P4InputVF::evaluateCurves()
{
    QString filedotmpl;
    QString s, e;
    QProcess *proc;

    evaluatingGcf_ = false;
    evaluatingPiecewiseConfig_ = true;

    // possible clean up after last GCF evaluation
    if (evalProcess_ != nullptr) {
        delete evalProcess_;
        evalProcess_ = nullptr;
    }

    prepareCurves();

    QString filedotmpl;
    filedotmpl = getmaplefilename();

    s = getMapleExe();
    if (s.isEmpty())
        s = "";
    else
        s = s.append(" \"").append(filedotmpl).append("\"");

    if (processText_ == nullptr)
        createProcessWindow();
    else {
        processText_->append(
            "\n\n------------------------------------------"
            "-------------------------------------\n\n");
        terminateProcessButton_->setEnabled(true);
        outputWindow_->show();
        outputWindow_->raise();
    }

    QProcess proc = new QProcess(this);

    proc->setWorkingDirectory(QDir::currentPath());

    connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
            g_p4app, &P4Application::signalCurvesEvaluated);
#ifdef QT_QPROCESS_OLD
    connect(proc,
            static_cast<void (QProcess::*)(QProcess::ProcessError)>(
                &QProcess::error),
            g_p4app, &P4Application::catchProcessError);
#else
    connect(proc, &QProcess::errorOccurred, g_p4app,
            &P4Application::catchProcessError);
#endif
    connect(proc, &QProcess::readyReadStandardOutput, this,
            &P4InputVF::readProcessStdout);

    processfailed_ = false;
    QString pa = "External Command: " + getMapleExe() + " " + filedotmpl;
    processText_->append(pa);
    proc->start(getMapleExe(), QStringList(filedotmpl), QIODevice::ReadWrite);

    if (proc->state() != QProcess::Running &&
        proc->state() != QProcess::Starting) {
        processfailed_ = true;
        delete proc;
        proc = nullptr;
        evalProcess_ = nullptr;
        evalFile_ = "";
        evalFile2_ = "";
        g_p4app->signalEvaluated(-1);
        terminateProcessButton_->setEnabled(false);
        return false;
    } else {
        evalProcess_ = proc;
        evalFile = filedotmpl;
        EvalFile2 = "";
        evaluating_ = true;
        evaluatingPiecewiseConfig_ = true;
        evaluatingGcf_ = false;
    }
}
}

// -----------------------------------------------------------------------
//          P4InputVF::finishCurvesEvaluation
// -----------------------------------------------------------------------
void P4InputVF::finishCurvesEvaluation(void)
{
    evaluatingPiecewiseConfig_ = false;
    P4Event *e =
        new P4Event((QEvent::Type)TYPE_SIGNAL_CURVESEVALUATED, nullptr);
    g_p4app->postEvent(g_p4stardlg, e);
}

// -----------------------------------------------------------------------
//          P4InputVF::markVFRegion
// -----------------------------------------------------------------------
// FIXME cal q sigui un punter la p?
void P4InputVF::markVFRegion(int index, const double *p)
{
    int i, k;

    if (g_VFResults.curves_result_.empty())
        return;

    for (i = numVFRegions_ - 1; i >= 0; i--) {
        // FIXME: math_regions
        if (isInsideRegion_sphere(vfRegions_[i].signs, p))
            break;
    }
    if (i < 0) {
        std::vector<int> sgns;
        if (numCurves_ > 0)
            sgns.reserve(sizeof(int) * numCurves_);
        for (auto curveResult : g_VFResults.curves_result_) {
            if (eval_curve(curveResult, p) < 0)
                sgns.push_back(-1);
            else
                sgns.push_back(+1);
        }
        vfRegions_.push_back(p4InputVFRegions::vfRegion(-1, sgns));
        numVFRegions_++;
        i = numVFRegions_ - 1;
    }
    vfRegions_[i].vfIndex = index;
}

// -----------------------------------------------------------------------
//          P4InputVF::unmarkVFRegion
// -----------------------------------------------------------------------
void P4InputVF::unmarkVFRegion(int index, const double *p)
{
    int i;

    if (g_VFResults.curves_result_.empty())
        return;

    for (i = numVFRegions_ - 1; i >= 0; i--) {
        // FIXME math_regions
        if (isInsideRegion_sphere(vfRegions_[i].signs, p))
            break;
    }
    if (i < 0)
        return;  // region cannot be found???

    if (vfRegions_[i].vfIndex != index)
        return;  // region does not have the corresponding index

    vfRegions_.erase(vfRegions_.begin() + i);
    if (numVFRegions_ == 1) {
        vfRegions_.clear();
    }
    numVFRegions_--;
}

// -----------------------------------------------------------------------
//          P4InputVF::markCurveRegion
// -----------------------------------------------------------------------
void P4InputVF::markCurveRegion(int index, const double *p)
{
    int i;
    std::vector<int> signs;

    if (numCurves_ == 0)
        return;
    if (g_VFResults.curves_result_.empty())
        return;

    signs.reserve(sizeof(int) * numCurves_);
    // TODO: veure si realment cal anar marxa enrera
    for (i = 0; i < numCurves_; i++) {
        if (i == index)
            signs[i] = 0;
        // FIXME eval_curve
        else if (eval_curve(g_VFResults.curves_result_[i], p) < 0)
            signs[i] = -1;
        else
            signs[i] = 1;
    }
    for (auto curve : curveRegions_) {
        if (signs == curve.signs && index == curve.index) {
            // curve mark already exists
            return;
        }
    }

    curveRegions_.push_back(index, signs);
    numCurveRegions_++;
    resampleCurve(index);
}

// -----------------------------------------------------------------------
//          P4InputVF::unmarkCurveRegion
// -----------------------------------------------------------------------
void P4InputVF::unmarkCurveRegion(int index, const double *p)
{
    std::vector<int> signs;

    if (numCurves_ == 0 || g_VFResults.curves_result_.empty())
        return;

    signs.reserve(sizeof(int) * numCurves_);
    for (int i = 0; i < numCurves_; i++) {
        if (i == index)
            signs.push_back(0);
        else if (eval_curve(g_VFResults.curves_result_[i], p) < 0)
            signs.push_back(-1);
        else
            signs.push_back(1);
    }
    for (auto k = curveRegions_.begin(); k != curveRegions_.end(); ++k) {
        if (signs == k->signs && index == k->curveIndex) {
            // curve mark exists
            curveRegions_.erase(k);
            numCurveRegions_--;
            resampleCurve(index);
        }
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::clearVFMarks
// -----------------------------------------------------------------------
void P4InputVF::clearVFMarks()
{
    vfRegions_.clear();
    numVFRegions_ = 0;
}

// -----------------------------------------------------------------------
//          P4InputVF::clearCurveMarks
// -----------------------------------------------------------------------
void P4InputVF::clearCurveMarks(void)
{
    curveRegions_.clear();
    numCurveRegions_ = 0;
}

// -----------------------------------------------------------------------
//          P4InputVF::isCurvePointDrawn
// -----------------------------------------------------------------------
bool P4InputVF::isCurvePointDrawn(int index, const double *pcoord)
{
    int k, j;

    if (numCurves_ == 0)
        return true;
    if (g_VFResults.curves_result_.empty())
        return false;

    for (auto curve : curveRegions_) {
        if (curve.curveIndex != index)
            continue;
        for (k = numCurves_ - 1; k >= 0; k--) {
            if (k == index)
                continue;

            if (eval_curve(g_VFResults.curves_result_[k], pcoord) < 0) {
                if (curve.signs[k] > 0)
                    break;
            } else {
                if (curve.signs[k] < 0)
                    break;
            }
        }
        if (k < 0) {
            // matching region found --> disable curve draw.
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//          P4InputVF::resampleCurve
// -----------------------------------------------------------------------
void P4InputVF::resampleCurve(int i)
{
    if (g_VFResults.curves_result_.empty())
        return;

    for (auto &sep : g_VFResults.curves_result_[i].sep_points) {
        if (isCurvePointDrawn(i, sep))
            sep.color = CSEPCURVE;
        else
            sep.color = CSHADEDCURVE;
    }
}

// -----------------------------------------------------------------------
//          P4InputVF::resampleGcf
// -----------------------------------------------------------------------
void P4InputVF::resampleGcf(int i)
{
    if (g_VFResults.curves_result_.empty() || g_VFResults.vf_.empty())
        return;

    for (auto it = g_VFResults.vf_[i]->gcf_points_.begin();
         it != g_VFResults.vf_[i]->gcf_points.end(); ++it) {
        if (getVFIndex_sphere(it->pcoord) != i) {
            g_VFResults.vf_[i]->gcf_points_.erase(it);
            if (it + 1 != g_VFResults.vf_[i]->gcf_points_.end())
                (it + 1)->dashes = 0;
        }
    }
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
//          GET VF INDEX FUNCTIONS //TODO posar totes
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

/*

    In this file, routines are developped to check if a point belongs
    to a given region, and to find the associated vector field.


    In order to avoid calculations with large numbers, when a point
    is close to infinity, the calculations are not done in usual
    coordinates, but instead in one of the charts near infinity.

    Therefore, a lot of variants have been developed with care.

*/

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_R2
// ---------------------------------------------------------------------
//
// Given a coordinate ucoord=(x0,y0), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// The point is given in standard coordinates (in the finite region)
// and is assumed to be in a ball with small radius.
int P4InputVF::getVFIndex_R2(const double *ucoord)
{
    if (g_VFResults.curves_result_.empty())
        return -1;
    for (int i = numVFRegions_ - 1; i >= 0; i--) {
        if (isInsideRegion_R2(vfRegions_[i].signs, ucoord))
            return vfRegions_[i].vfIndex;
    }
    return -1;
}

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_sphere
// ---------------------------------------------------------------------
//
// Given a coordinate pcoord=(X,Y,Z), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// The point is given in Poincare-sphere coordinates (X,Y,Z), or
// Poincare-Lyapunov coordinates (X,Y,Z) depending on the setting of the
// system
int P4InputVF::getVFIndex_sphere(const double *pcoord)
{
    double ucoord[2], theta;
    if (!g_VFResults.plweights_) {
        return getVFIndex_psphere(pcoord);
    } else {
        return getVFIndex_plsphere(pcoord);
    }
}

// ---------------------------------------------------------------------
//                      GETVFINDEX_PSPHERE
// ---------------------------------------------------------------------
//
// Given a coordinate pcoord=(X,Y,Z), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// The point is given in Poincare-sphere coordinates (X,Y,Z), where
// (X,Y) lies on the unit circle and Z is (the inverse of) a radial coordinate.
//
// Since we do not know if the point is close to infinity or not, and
// if it is, in which chart it lies, we need to first find that out.
int P4InputVF::getVFIndex_psphere(const double *pcoord)
{
    double ucoord[2], theta;
    if (pcoord[2] > ZCOORD) {
        psphere_to_R2(pcoord[0], pcoord[1], pcoord[2], ucoord);
        return getVFIndex_R2(ucoord);
    } else {
        theta = atan2(fabs(pcoord[1]), fabs(pcoord[0]));
        if (theta < PI_DIV4 && theta > -PI_DIV4) {
            if (pcoord[0] > 0) {
                psphere_to_U1(pcoord[0], pcoord[1], pcoord[2], ucoord);
                return getVFIndex_U1(ucoord);
            } else {
                psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], ucoord);
                return getVFIndex_V1(ucoord);
            }
        } else {
            if (pcoord[1] > 0) {
                psphere_to_U2(pcoord[0], pcoord[1], pcoord[2], ucoord);
                return getVFIndex_U2(ucoord);
            } else {
                psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], ucoord);
                return getVFIndex_V2(ucoord);
            }
        }
    }
}

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_plsphere
// ---------------------------------------------------------------------
//
// Given a coordinate pcoord=(X,Y,Z), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// The point is given in Poincare-sphere coordinates (X,Y,Z).  Such coordinates
// can be of two forms:
//
//      (X,Y,Z) = (0,u,v)  --> (x,y) = (u,v)
//      (X,Y,Z) = (1,u,v)  --> (x,y) = (cos(v)/u^p, sin(v)/u^q)
//
// The first form is used for points inside the unit circle; the second
// for points near infinity.  In the above form, the relation with the
// normal (x,y)-coordinates in the plane are shown.
int P4InputVF::getVFIndex_plsphere(const double *pcoord)
{
    double ucoord[2];

    ucoord[0] = pcoord[1];
    ucoord[1] = pcoord[2];

    if (pcoord[0])
        return getVFIndex_R2(ucoord);
    else
        return getVFIndex_cyl(ucoord);
}

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_cyl
// ---------------------------------------------------------------------
//
// Given a coordinate y=(r,theta), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// This is only used in the Poincare-Lyapunov setting, in a study
// near infinity.
//
// The point is given in cylindrical coordinates (r,theta).  The r-coordinate
// is a (inverse) radial coordinate, the theta is an angular coordinate.
int P4InputVF::getVFIndex_cyl(const double *y)
{
    if (g_VFResults.curves_result_.empty())
        return -1;
    for (int i = numVFRegions_ - 1; i >= 0; i--) {
        if (isInsideRegion_cyl(vfRegions_[i].signs, y))
            return vfRegions_[i].vfIndex;
    }
    return -1;
}

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_U1
// ---------------------------------------------------------------------
//
// Given a coordinate y=(z1,z2), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// The relation with original coordinates is:
//
//          (x,y) = (1/z2,z1/z2),           z2 > 0.
//
// If z2<0, we are inside the chart VV1.
int P4InputVF::getVFIndex_U1(const double *y)
{
    if (g_VFResults.curves_result_.empty())
        return -1;
    for (int i = numVFRegions_ - 1; i >= 0; i--) {
        if (isInsideRegion_U1(vfRegions_[i].signs, y))
            return vfRegions_.vfIndex;
    }
    return -1;
}

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_V1
// ---------------------------------------------------------------------
//
// Given a coordinate y=(z1,z2), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// The relation with original coordinates is:
//
//          (x,y) = (-1/z2,z1/z2),           z2 > 0.
//
// If z2<0, we are inside the chart UU1.
int P4InputVF::getVFIndex_V1(const double *y)
{
    if (g_VFResults.curves_result_.empty())
        return -1;
    for (int i = numVFRegions_ - 1; i >= 0; i--) {
        if (isInsideRegion_V1(vfRegions_[i].signs, y))
            return vfRegions_.vfIndex;
    }
    return -1;
}

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_U2
// ---------------------------------------------------------------------
//
// Given a coordinate y=(z1,z2), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// The relation with original coordinates is:
//
//          (x,y) = (z1/z2,1/z2),           z2 > 0.
//
// If z2<0, we are inside the chart VV2.
int P4InputVF::getVFIndex_U2(const double *y)
{
    if (g_VFResults.curves_result_.empty())
        return -1;
    for (int i = numVFRegions_ - 1; i >= 0; i--) {
        if (isInsideRegion_U2(vfRegions_[i].signs, y))
            return vfRegions_.vfIndex;
    }
    return -1;
}

// ---------------------------------------------------------------------
//          P4InputVF::getVFIndex_V2
// ---------------------------------------------------------------------
//
// Given a coordinate y=(z1,z2), we find the index of the vector
// field that is assigned to the region where the point belongs to.
//
// The relation with original coordinates is:
//
//          (x,y) = (z1/z2,-1/z2),           z2 > 0.
//
// If z2<0, we are inside the chart UU2.
int P4InputVF::getVFIndex_V2(const double *y)
{
    if (g_VFResults.curves_result_.empty())
        return -1;
    for (int i = numVFRegions_ - 1; i >= 0; i--) {
        if (isInsideRegion_V2(vfRegions_[i].signs, y))
            return vfRegions_.vfIndex;
    }
    return -1;
}

// ---------------------------------------------------------------------
//  getVFIndex_UU1, getVFIndex_UU2, getVFIndex_VV1, getVFIndex_VV2
// ---------------------------------------------------------------------
//
// When integration starts, for example in a U1 chart in a point (z1,z2)
// very close to infinity (close to z2=0, but z2>0), it may sometimes happen
// that the line of infinity is crossed (to a point z2<0).  The resulting
// point lies in the U1 chart, but in a wrong part of it.  We say that
// it lies in the VV1-part: this point will be mapped to the symmetric point
// in the V1-chart.
//
// So:
//
//   * a point in the UU1 chart lies in the wrong part of the V1-chart, and is
//   in fact a U1-point
//   * a point in the UU2 chart lies in the wrong part of the V2-chart, and is
//   in fact a U2-point
//   * a point in the VV1 chart lies in the wrong part of the U1-chart, and is
//   in fact a V1-point
//   * a point in the VV2 chart lies in the wrong part of the U2-chart, and is
//   in fact a V2-point
//
// The relation between the symmetric charts is simple:
//              (z1,z2) ---> (-z1,-z2).
//
// Note: these charts are only used when p=q=1.
int P4InputVF::getVFIndex_UU1(const double *yy)
{
    double y[2];
    y[0] = -yy[0];
    y[1] = -yy[1];
    return getVFIndex_U1(y);
}

int P4InputVF::getVFIndex_UU2(const double *yy)
{
    double y[2];
    y[0] = -yy[0];
    y[1] = -yy[1];
    return getVFIndex_U2(y);
}

int P4InputVF::getVFIndex_VV1(const double *yy)
{
    double y[2];
    y[0] = -yy[0];
    y[1] = -yy[1];
    return getVFIndex_V1(y);
}

int P4InputVF::getVFIndex_VV2(const double *yy)
{
    double y[2];
    y[0] = -yy[0];
    y[1] = -yy[1];
    return getVFIndex_V2(y);
}

// ---------------------------------------------------------------------
//  isInsideRegion_R2, isInsideRegion_R2_epsilon
// ---------------------------------------------------------------------
//
// Checks if a points lies in a region determined by a set of signs.
// The point lies in the finite region.
//
// The _epsilon variant increases the region by an epsilon amount before
// checking
bool isInsideRegion_R2(const int *signs, const double *ucoord)
{
    int k;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        if (eval_term2(g_VFResults.curves_result_[k].sep, ucoord) < 0) {
            if (signs[k] > 0)
                return false;
        } else {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

bool isInsideRegion_R2_epsilon(const int *signs, const double *ucoord,
                               double epsilon)
{
    int k;
    double v;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        v = eval_term2(g_VFResults.curves_result_[k].sep, ucoord);
        if (v < -epsilon) {
            if (signs[k] > 0)
                return false;
        }
        if (v > epsilon) {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------
//  isInsideRegion_U1, isInsideRegion_U1_epsilon
// ---------------------------------------------------------------------
//
// Checks if a points lies in a region determined by a set of signs.
// The point lies in the U1 chart.
// This is only used when p=q=1
//
// The _epsilon variant increases the region by an epsilon amount before
// checking
bool isInsideRegion_U1(const int *signs, const double *z1z2)
{
    int k;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        if (eval_term2(g_VFResults.curves_result_[k].sep_U1, z1z2) < 0) {
            if (signs[k] > 0)
                return false;
        } else {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

bool isInsideRegion_U1_epsilon(const int *signs, const double *z1z2,
                               double epsilon)
{
    int k;
    double v;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        v = eval_term2(g_VFResults.curves_result_[k].sep_U1, z1z2);
        if (v < -epsilon) {
            if (signs[k] > 0)
                return false;
        }
        if (v > epsilon) {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------
//  isInsideRegion_V1, isInsideRegion_V1_epsilon
// ---------------------------------------------------------------------
//
// Checks if a points lies in a region determined by a set of signs.
// The point lies in the V1 chart.
// This is only used when p=q=1
//
// The _epsilon variant increases the region by an epsilon amount before
// checking
bool isInsideRegion_V1(const int *signs, const double *z1z2)
{
    int k;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        if (eval_term2(g_VFResults.curves_result_[k].sep_V1, z1z2) < 0) {
            if (signs[k] > 0)
                return false;
        } else {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

bool isInsideRegion_V1_epsilon(const int *signs, const double *z1z2,
                               double epsilon)
{
    int k;
    double v;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        v = eval_term2(g_VFResults.curves_result_[k].sep_V1, z1z2);
        if (v < -epsilon) {
            if (signs[k] > 0)
                return false;
        }
        if (v > epsilon) {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------
//  isInsideRegion_U2, isInsideRegion_U2_epsilon
// ---------------------------------------------------------------------
//
// Checks if a points lies in a region determined by a set of signs.
// The point lies in the U2 chart.
// This is only used when p=q=1
//
// The _epsilon variant increases the region by an epsilon amount before
// checking
//

bool isInsideRegion_U2(const int *signs, const double *z1z2)
{
    int k;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        if (eval_term2(g_VFResults.curves_result_[k].sep_U2, z1z2) < 0) {
            if (signs[k] > 0)
                return false;
        } else {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}
bool isInsideRegion_U2_epsilon(const int *signs, const double *z1z2,
                               double epsilon)
{
    int k;
    double v;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        v = eval_term2(g_VFResults.curves_result_[k].sep_U2, z1z2);
        if (v < -epsilon) {
            if (signs[k] > 0)
                return false;
        }
        if (v > epsilon) {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------
//                      isInsideRegion_V2
//                      isInsideRegion_V2_epsilon
// ---------------------------------------------------------------------
//
// Checks if a points lies in a region determined by a set of signs.
// The point lies in the V2 chart.
// This is only used when p=q=1
//
// The _epsilon variant increases the region by an epsilon amount before
// checking
bool isInsideRegion_V2(const int *signs, const double *z1z2)
{
    int k;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        if (eval_term2(g_VFResults.curves_result_[k].sep_V2, z1z2) < 0) {
            if (signs[k] > 0)
                return false;
        } else {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

bool isInsideRegion_V2_epsilon(const int *signs, const double *z1z2,
                               double epsilon)
{
    int k;
    double v;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        v = eval_term2(g_VFResults.curves_result_[k].sep_V2, z1z2);
        if (v < -epsilon) {
            if (signs[k] > 0)
                return false;
        }
        if (v > epsilon) {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------
//                      isInsideRegion_cyl
//                      isInsideRegion_cyl_epsilon
// ---------------------------------------------------------------------
//
// Checks if a points lies in a region determined by a set of signs.
// The point lies in the part near infinity, in the case of Poincare-
// Lyapunov compactification ((p,q) is not (1,1)).
//
// The _epsilon variant increases the region by an epsilon amount before
// checking
bool isInsideRegion_cyl(const int *signs, const double *rtheta)
{
    int k;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        if (eval_term3(g_VFResults.curves_result_[k].sep_C, rtheta) < 0) {
            if (signs[k] > 0)
                return false;
        } else {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

bool isInsideRegion_cyl_epsilon(const int *signs, const double *rtheta,
                                double epsilon)
{
    int k;
    double v;

    if (g_VFResults.curves_result_.empty())
        return false;

    for (k = g_ThisVF->numCurves_ - 1; k >= 0; k--) {
        v = eval_term3(g_VFResults.curves_result_[k].sep_C, rtheta);
        if (v < -epsilon) {
            if (signs[k] > 0)
                return false;
        }
        if (v > epsilon) {
            if (signs[k] < 0)
                return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------
//                      isInsideRegion_sphere
//                      isInsideRegion_sphere_epsilon
// ---------------------------------------------------------------------
//
// Checks if a points lies in a region determined by a set of signs.
// We don't know where the point lies, so we have to determine this first.
//
// The _epsilon variant increases the region by an epsilon amount before
// checking
//
bool isInsideRegion_sphere(const int *signs, const double *pcoord)
{
    double ucoord[2], theta;

    if (!g_VFResults.plweights_) {
        if (pcoord[2] > ZCOORD) {
            psphere_to_R2(pcoord[0], pcoord[1], pcoord[2], ucoord);
            return isInsideRegion_R2(signs, ucoord);
        } else {
            theta = atan2(fabs(pcoord[1]), fabs(pcoord[0]));
            if (theta < PI_DIV4 && theta > -PI_DIV4) {
                if (pcoord[0] > 0) {
                    psphere_to_U1(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return isInsideRegion_U1(signs, ucoord);
                } else {
                    psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return isInsideRegion_V1(signs, ucoord);
                }
            } else {
                if (pcoord[1] > 0) {
                    psphere_to_U2(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return isInsideRegion_U2(signs, ucoord);
                } else {
                    psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return isInsideRegion_V2(signs, ucoord);
                }
            }
        }
    } else {
        ucoord[0] = pcoord[1];
        ucoord[1] = pcoord[2];

        if (pcoord[0])
            return isInsideRegion_R2(signs, ucoord);
        else
            return isInsideRegion_cyl(signs, ucoord);
    }
}

bool isInsideRegion_sphere_epsilon(const int *signs, const double *pcoord,
                                   double epsilon)
{
    double ucoord[2], theta;

    if (!g_VFResults.plweights_) {
        if (pcoord[2] > ZCOORD) {
            psphere_to_R2(pcoord[0], pcoord[1], pcoord[2], ucoord);
            return isInsideRegion_R2_epsilon(signs, ucoord, epsilon);
        } else {
            theta = atan2(fabs(pcoord[1]), fabs(pcoord[0]));
            if (theta < PI_DIV4 && theta > -PI_DIV4) {
                if (pcoord[0] > 0) {
                    psphere_to_U1(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return isInsideRegion_U1_epsilon(signs, ucoord, epsilon);
                } else {
                    psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return isInsideRegion_V1_epsilon(signs, ucoord, epsilon);
                }
            } else {
                if (pcoord[1] > 0) {
                    psphere_to_U2(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return isInsideRegion_U2_epsilon(signs, ucoord, epsilon);
                } else {
                    psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], ucoord);
                    return isInsideRegion_V2_epsilon(signs, ucoord, epsilon);
                }
            }
        }
    } else {
        ucoord[0] = pcoord[1];
        ucoord[1] = pcoord[2];

        if (pcoord[0])
            return isInsideRegion_R2_epsilon(signs, ucoord, epsilon);
        else
            return isInsideRegion_cyl_epsilon(signs, ucoord, epsilon);
    }
}