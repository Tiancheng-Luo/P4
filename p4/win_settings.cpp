#include <qlayout.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include "custom.h"
#include "table.h"
#include "win_p4.h"
#include "math_p4.h"
#include "win_settings.h"
#include "p4application.h"

/*
    The Settings Window is a modal dialog box!
*/

extern QString GetDefaultP4Path( void );
extern QString GetDefaultP4MaplePath( void );
extern QString GetDefaultP4BinPath( void );
extern QString GetDefaultP4Path( void );
extern QString GetDefaultP4ReducePath( void );
extern QString GetDefaultP4TempPath( void );
extern QString GetDefaultP4HelpPath( void );
extern QString GetDefaultP4SumTablePath( void );
extern QString GetDefaultMapleInstallation( void );
extern QString GetDefaultReduceInstallation( void );

extern QString GetP4Path( void );
extern QString GetP4TempPath( void );
extern QString GetP4SumTablePath( void );
extern QString GetMapleExe( void );
extern QString GetReduceExe( void );

extern void SetP4Path( QString );
extern void SetP4TempPath( QString );
extern void SetP4SumTablePath( QString );
extern void SetMapleExe( QString );
extern void SetReduceExe( QString );

extern void SetP4WindowTitle( QWidget *, QString );

static QString StripSlash( QString p )
{
    if( p.isNull() )
        return p;
    if( p.length() > 0 )
    {
        int i;
        i = p.length()-1;
        if( p[i] == '/' || p[i] == '\\' )
             p = p.mid(0,i);
    }
    return p;
}

static QString StripQuotes( QString p )
{
    if( p.isNull() )
        return p;
    if( p[0] == '\"' )
        p = p.mid(1);

    if( p.length() != 0 )
    {
        int i;
        i = p.length()-1;
        if( p[i] == '\"' )
        {
            p = p.mid(0,i);
        }
    }

    return p;
}

static QString AddQuotes( QString p )
{
    if( p.isNull() )
        return p;

    if( p.contains( ' ' ) )
    {
        p = "\"" + p + "\"";
    }

    return p;
}

QSettingsDlg::QSettingsDlg( QWidget * parent, Qt::WindowFlags f )
    : QDialog(  parent, f )
{
//  setFont( QFont( FONTSTYLE, FONTSIZE ) );

    edt_base = new QLineEdit( StripQuotes(GetP4Path()), this );
    lbl_base = new QLabel( "&Base Installation Path", this );
    btn_base = new QPushButton( "Browse...", this );
    lbl_base->setBuddy( edt_base );

    edt_sum = new QLineEdit( StripQuotes(GetP4SumTablePath()), this );
    lbl_sum = new QLabel( "&Sumtable Path", this );
    btn_sum = new QPushButton( "Browse...", this );
    lbl_sum->setBuddy( edt_sum );

    edt_temp = new QLineEdit( StripQuotes(GetP4TempPath()), this );
    lbl_temp = new QLabel( "&Temporary Files Path", this );
    btn_temp = new QPushButton( "Browse...", this );
    lbl_temp->setBuddy( edt_temp );

    edt_maple = new QLineEdit( StripQuotes(GetMapleExe()), this );
    lbl_maple = new QLabel( "&Maple Executable", this );
    btn_maple = new QPushButton( "Browse...", this );
    lbl_maple->setBuddy( edt_maple );

    edt_red = new QLineEdit( StripQuotes(GetReduceExe()), this );
    lbl_red = new QLabel(  "R&educe Executable", this );
    btn_red = new QPushButton( "Browse...", this );
    lbl_red->setBuddy( edt_red );

    btn_ok = new QPushButton( "&Ok", this );
    btn_reset = new QPushButton( "&Reset", this );
    btn_cancel = new QPushButton( "&Cancel", this );

    btn_ok->setDefault(true);

#ifdef TOOLTIPS
    btn_base->setToolTip(  "Search for this path on your computer" );
    btn_sum->setToolTip(   "Search for this path on your computer" );
    btn_temp->setToolTip(  "Search for this path on your computer" );
    btn_maple->setToolTip( "Search for this file on your computer.\nYou need a command-line version of Maple.\nThe name would be something like cmaple9.5.exe in Windows." );
    btn_red->setToolTip(   "Search for this file on your computer.\n" );

    edt_base->setToolTip(  "The base installation path.  In here, one finds bin/, help/ and reduce/ subdirectories." );
    edt_sum->setToolTip(   "The path where sumtables are stored when calculating Lyapunov constants." );
    edt_temp->setToolTip(  "Temporary path name.\nLeave blank when you want temporary files to be stored in the current working directory." );
    edt_maple->setToolTip( "The name of the Maple executable (command-line version)" );
    edt_red->setToolTip(   "The name of the reduce executable" );

    btn_ok->setToolTip(    "Store changes, and go back to program" );
    btn_cancel->setToolTip( "Undo any changes" );
    btn_reset->setToolTip(  "Reset to defaults, try to locate Maple/Reduce executable automatically" );
#endif

    mainLayout = new QBoxLayout( QBoxLayout::TopToBottom, this );

    mainLayout->addSpacing(4);
    mainLayout->addWidget( new QLabel(
        "These settings are application-wide and will be saved upon exit.\n"
        "An improper value may lead to failure of evaluating vector fields.\n"
#ifdef Q_OS_WIN
        "\n"
        "Note: you are allowed to used spaces in the path names."
#endif
        , this ) );
    mainLayout->addSpacing(4);

    QHBoxLayout * buttons = new QHBoxLayout();
    buttons->addStretch(0);
    buttons->addWidget( btn_ok );
    buttons->addWidget( btn_reset );
    buttons->addWidget( btn_cancel );
    buttons->addStretch(0);

    QGridLayout * lay00 = new QGridLayout();

    lay00->addWidget( lbl_base, 0, 0 );
    lay00->addWidget( edt_base, 0, 1 );
    lay00->addWidget( btn_base, 0, 2 );
    lay00->addWidget( lbl_sum, 1, 0 );
    lay00->addWidget( edt_sum, 1, 1 );
    lay00->addWidget( btn_sum, 1, 2 );
    lay00->addWidget( lbl_temp, 2, 0 );
    lay00->addWidget( edt_temp, 2, 1 );
    lay00->addWidget( btn_temp, 2, 2 );
    lay00->addWidget( lbl_maple, 3, 0 );
    lay00->addWidget( edt_maple, 3, 1 );
    lay00->addWidget( btn_maple, 3, 2 );
    lay00->addWidget( lbl_red, 4, 0 );
    lay00->addWidget( edt_red, 4, 1 );
    lay00->addWidget( btn_red, 4, 2 );

    mainLayout->addLayout( lay00 );
    mainLayout->addSpacing( 4 );
    mainLayout->addLayout( buttons );

    setLayout( mainLayout );

    QObject::connect( btn_base, SIGNAL(clicked()), this, SLOT(OnBrowseBase()) );
    QObject::connect( btn_sum, SIGNAL(clicked()), this, SLOT(OnBrowseSum()) );
    QObject::connect( btn_temp, SIGNAL(clicked()), this, SLOT(OnBrowseTemp()) );
    QObject::connect( btn_maple, SIGNAL(clicked()), this, SLOT(OnBrowseMaple()) );
    QObject::connect( btn_red, SIGNAL(clicked()), this, SLOT(OnBrowseReduce()) );
    QObject::connect( btn_ok, SIGNAL(clicked()), this, SLOT(OnOk()) );
    QObject::connect( btn_reset, SIGNAL(clicked()), this, SLOT(OnReset()) );
    QObject::connect( btn_cancel, SIGNAL(clicked()), this, SLOT(OnCancel()) );

#ifdef Q_OS_WIN
    edt_red->setEnabled(false);
    btn_red->setEnabled(false);
#endif

    SetP4WindowTitle( this, "Main Program Settings" );
}

void QSettingsDlg::OnCancel( void )
{
    done( 0 );
}

void QSettingsDlg::OnOk( void )
{
    QString s;

    // Strip outer Quotes and white space, in any order, and possible also trailing slash
    
    s = StripSlash( StripQuotes( StripQuotes( edt_base->text() ).trimmed() ) );
    SetP4Path( s );
    s = StripSlash( StripQuotes( StripQuotes( edt_sum->text() ).trimmed() ) );
    SetP4SumTablePath( s );
    s = StripSlash( StripQuotes( StripQuotes( edt_temp->text() ).trimmed() ) );
    SetP4TempPath( s );

    // for exe's, trailing slash is irrelevant

    s = StripQuotes( StripQuotes( edt_maple->text() ).trimmed() );
    
    
#ifdef Q_OS_WIN
    // what to do when the user enters a value for a maple executable that does not exist?

    QFileInfo maplepat;
    int vb;

    maplepat.setFile( s );
    if( !maplepat.exists() )
    {
        vb = false;
        
        QMessageBox msgBox;
        
        msgBox.setText( "The maple executable was not found on disc.\n\n"
                        "P4 will not be able to evaluate/plot vector fields.\n\n" );
        msgBox.addButton( "&Go back to settings window", QMessageBox::AcceptRole );
        msgBox.addButton( "&Don't care", QMessageBox::RejectRole );
        switch (msgBox.exec()) {
        case QMessageBox::AcceptRole:
            // yes was clicked
            vb=false;
            break;
        case QMessageBox::RejectRole:
            // no was clicked
            vb=true;
            break;
        default:
            // should never be reached
            break;
        }
        if( !vb )
            return;
    }
    
#endif
    
    SetMapleExe( AddQuotes(s) );
    s = StripQuotes( StripQuotes( edt_red->text() ).trimmed() );
    SetReduceExe( AddQuotes(s) );

    done( 1 );
}

void QSettingsDlg::OnReset( void )
{
    edt_base->setText( StripQuotes(GetDefaultP4Path()) );
    edt_sum->setText( StripQuotes(GetDefaultP4SumTablePath()) );
    edt_temp->setText( StripQuotes(GetDefaultP4TempPath()) );
    edt_maple->setText( StripQuotes(GetDefaultMapleInstallation()) );
    edt_red->setText( StripQuotes(GetDefaultReduceInstallation()) );
}

void QSettingsDlg::OnBrowseReduce( void )
{
     BrowseForExistingPathOrFile( edt_red, QString("Select reduce executable:"), true );
}

void QSettingsDlg::OnBrowseMaple( void )
{
     BrowseForExistingPathOrFile( edt_maple, "Select maple executable:", true );
}

void QSettingsDlg::OnBrowseTemp( void )
{
     BrowseForExistingPathOrFile( edt_temp, "Select temporary path:", false );
}

void QSettingsDlg::OnBrowseSum( void )
{
     BrowseForExistingPathOrFile( edt_sum, "Select sumtable path:", false );
}

void QSettingsDlg::OnBrowseBase( void )
{
     BrowseForExistingPathOrFile( edt_base, "Select P4 base installation path:", false );
}

void QSettingsDlg::BrowseForExistingPathOrFile( QLineEdit * edt, QString caption, bool isfile )
{
     // browse for an existing path if isfile = false, or for an existing file if isfile = true.

     QString oldstr, newstr;
     
     oldstr = edt->text();
     if( !isfile )
         newstr = QFileDialog::getExistingDirectory ( this, caption, oldstr, QFileDialog::ShowDirsOnly );
     else
         newstr = QFileDialog::getOpenFileName( this, caption, oldstr );

     if( !newstr.isNull() )
     {
         newstr = QDir::toNativeSeparators(newstr);
         edt->setText( newstr );
     }
}
 
