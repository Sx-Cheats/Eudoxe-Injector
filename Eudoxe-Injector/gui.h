#pragma once

#ifndef UI_SLNINJECTOR_H
#define UI_SLNINJECTOR_H

#include "Includes.h"

QT_BEGIN_NAMESPACE

class QAppMovable : public QWidget
{
    QMainWindow* MainWindow;
    QPoint Base;

public:

    QAppMovable(QMainWindow* _MainWindow, QWidget* parent, QRect Geometry) : QWidget(parent), MainWindow(_MainWindow) {
        Base = pos(); setFocusPolicy(Qt::ClickFocus); setGeometry(Geometry); installEventFilter(this);};

protected :

    void mousePressEvent(QMouseEvent* event)
    {
        if (event->buttons() == Qt::MouseButton::LeftButton)
            Base = event->globalPos();
    }

    void mouseMoveEvent(QMouseEvent* event)
    {
        if (event->buttons() == Qt::MouseButton::LeftButton)
        {
            MainWindow->move((MainWindow->pos() + event->globalPos()) - Base);
            Base = event->globalPos();

        }
    }
}; 

class Ui_SlnInjectorClass
{
public:

    QSettings * settings;
    QButton   * BPInject;

    struct __options {

        QString  DllPath          = "";
        QString  Process          = "";
        WORD     InjectMethod     = 0;
        double   Timeout          = 1.000;
        // ------------------
        WORD      Loader          = 0;
        bool      AutoInject      = false;
        bool      LoadDllCopy     = false;
        bool      HijackHandle    = false;
        bool      RandomFileName  = false;
        bool      CloakThread     = false;
        bool      RemoveFromPEB   = false;
        WORD      Arch            = 0;

    } Options;

    Ui_SlnInjectorClass() 
    {
        settings  = new QSettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    };

    bool IsRunningAsAdmin()
    {
        HANDLE tk;
        if (OpenProcessToken(NtCurrentProcess(), TOKEN_QUERY, &tk) == 0)
            return false;

        TOKEN_ELEVATION Elevation;
        if (!GetTokenInformation(tk, TokenElevation, &Elevation, sizeof(Elevation), (new DWORD{ sizeof(TOKEN_ELEVATION) })))
            return false;

        NtClose(tk);
        return (bool)Elevation.TokenIsElevated;

    }

    enum Settings
    {
         AUTO_INJECT,
         LOAD_DLL_COPY,
         HIJACK_HANDLE,
         RANDOM_FILE_NAME,
         CLOAK_THREAD,
         REMOVE_FROM_PEB,
         ARCH,
         LOADER,
         DLL_PATH,
         PROCESS,
         INJECT_METHOD,
         TIMEOUT,
    };

    template<typename T>
    void SaveSettings(Settings  type, T value)
    {
        settings->setValue(QString::number((int)type),value);
    };

    QVariant GetSettings(Settings type)
    {
      return settings->value(QString::number((int)type));
    }

    QString ToCorrectFormat(QString str)
    {
        if (str.length() > 1)
            return str;
        else
            return "";
    }

    void setupUi(QMainWindow* MainWindow)
    {
        
        bool IsAdmin = IsRunningAsAdmin();
       
        MainWindow->resize(370, 510);
        MainWindow->setWindowFlag(Qt::FramelessWindowHint);

        QWidget* CentralWidget = QtComponents::AddWidget(MainWindow);
        MainWindow->setCentralWidget(CentralWidget);

        CentralWidget->setObjectName("centralwidget");

        QAppMovable * titlebar = new QAppMovable (MainWindow,CentralWidget, { 2,2,366,32 });
        titlebar->setObjectName("TitleBar");

        if(!IsAdmin)
          QtComponents::AddText(titlebar, "Eudoxe Injector", 15, true, false, { 10 ,6,170,21 })->setObjectName("Title");
        else if (IsAdmin)
        {
            QtComponents::Icon(titlebar,  qApp->style()->standardIcon(QStyle::SP_VistaShield), {17,17}, {10,6});
            QtComponents::AddText(titlebar, "Eudoxe Injector", 15, true, false, { 40 ,6,170,21 })->setObjectName("Title");
        }
        
        QtComponents::AddButton(titlebar, "×", 20, true, { 330,0,40,32 })->click([&] {qApp->exit(); })->Button->setObjectName("BPClose");
        QtComponents::AddButton(titlebar, "―", 20, true, { 290,0,40,32 })->click([&, MainWindow] {; MainWindow->showMinimized(); })->Button->setObjectName("BPMinimize");


        QTabWidget* tabwidget = QtComponents::AddTabWidget(CentralWidget, { 0,32,370,480 });
        tabwidget->setObjectName("tabWidget");


        QWidget* InjectorSection = QtComponents::AddWidget(tabwidget);
        InjectorSection->setObjectName("injector");
        tabwidget->addTab(InjectorSection, QIcon("./Logos/syringe.png"), "Injector");



        QWidget* OptionsSection = QtComponents::AddWidget(tabwidget);
        tabwidget->addTab(OptionsSection, QIcon("./Logos/settings.png"), "Options");

        // Injection Section

        QtComponents::AddText(InjectorSection, "dll path", 12, false, true, { 35,30,49,16 });
        auto DllPath = QtComponents::AddLineEdit(InjectorSection, false, { 40, 50, 250, 25 });
        DllPath->Input->setText(ToCorrectFormat(GetSettings(Settings::DLL_PATH).toString()));

        DllPath->WhenEditFinish([&, DllPath, MainWindow] {

            QString dllpath = DllPath->Input->text();
            QFile dll(dllpath);   


            if (!dll.exists() || strcmp(PathFindExtensionA(dllpath.toStdString().c_str()), ".dll"))
            {
                QMessageBox(QMessageBox::Information, "Info", "Invalid dll path !", QMessageBox::Ok, MainWindow, Qt::Drawer).exec();
                return;
            };

            Options.DllPath = dllpath;

            SaveSettings(Settings::DLL_PATH, dllpath);

            });
        
        QtComponents::BpIcon(InjectorSection, "./Logos/folder.png", { 35,35 }, { 300,40 })->click([&,DllPath] {
            QString dllpath = QFileDialog::getOpenFileName((QWidget*)DllPath->Input, QObject::tr("Open File"),QDir::currentPath(),QObject::tr("Library (*.dll)"));

            if (!QFile(dllpath).exists())
                return;

            Options.DllPath = dllpath;

            SaveSettings(Settings::DLL_PATH, dllpath);

            DllPath->Input->setText(dllpath);

        })->Button->setObjectName("BPSearchDll");


        QtComponents::AddText(InjectorSection, "process", 12, false, true, { 35,110,49,16 });
        auto Process = QtComponents::AddLineEdit(InjectorSection, true, { 40, 130, 250, 25 });
        
        Process->Input->setText(ToCorrectFormat(GetSettings(Settings::PROCESS).toString()));
        Process->WhenEditFinish([&, Process] {
            QString process = Process->Input->text();
            Options.Process = process;
            SaveSettings(Settings::PROCESS, process);
            })->Input->setFont(QtComponents::CurrentFont(11, false, false, "Bahnschrift SemiCondensed"));

       
        
        QtComponents::BpIcon(InjectorSection, "./Logos/feature.png", { 35,35 }, { 300,120 })->Button->setObjectName("BPSearchProcess");
       
        QtComponents::AddText(InjectorSection, "Inject Method", 12, false, true, { 35,195,80,16 });
        QtComponents::AddComboBox(InjectorSection, { 45,220,250,30 })->AddItems({"CreateRemoteThread","NtCreateThreadEx","QueueUserAPC","ThreadHiJacking","RtlCreateUserThread"})->WhenIndexChanged([&](int index) {
            Options.InjectMethod = index;
            SaveSettings(Settings::INJECT_METHOD, index);
            })->ComboBox->setCurrentIndex(GetSettings(Settings::INJECT_METHOD).toInt());

        QtComponents::AddText(InjectorSection, "Timeout (second)", 12, false, true, { 35,275,100,16 });

        QtComponents::AddDoubleSpinBox(InjectorSection, 1, 30, 1, 1, { 45,305,120,24 })->WhenValueChanged([&](double value) {
            Options.Timeout = value;
            SaveSettings(Settings::TIMEOUT, value);
            })->SpinBox->setValue(GetSettings(Settings::TIMEOUT).toDouble());

        BPInject = QtComponents::AddButton(InjectorSection, "  Inject", 20, true, { 110,360,150,45 });
        BPInject->Button->setIcon(QIcon("./Logos/syringe.png"));
        BPInject->Button->setIconSize({ 25, 25 });
        BPInject->Button->setObjectName("BPInject");

        // Options Section

        QtComponents::AddText(OptionsSection, "Loader", 12, false, true, { 55,25,100,16 });

        QtComponents::AddComboBox(OptionsSection, { 60,50,250,30 })->AddItems({ "LoadLibrary","LoadLibraryEx","LoadPackagedLibrary","LdrpLoadDll","MapViewOfFile" })->WhenIndexChanged([&](int index) {
            Options.Loader = index;
            SaveSettings(Settings::LOADER, index);
            })->ComboBox->setCurrentIndex(GetSettings(Settings::LOADER).toInt());


        QtComponents::AddCheckBox(OptionsSection, "Auto Inject", false, { 40, 125, 100, 20 })->WhenStateChanged([&](int state) {
            Options.AutoInject = state;
            SaveSettings(Settings::AUTO_INJECT, state);
            })->CheckBox->setChecked(GetSettings(Settings::AUTO_INJECT).toBool());

        QtComponents::AddCheckBox(OptionsSection, "Load Dll Copy", false,    { 190, 125, 125, 20 })->WhenStateChanged([&](int state) {
            Options.LoadDllCopy = state;
            SaveSettings(Settings::LOAD_DLL_COPY, state);
            })->CheckBox->setChecked(GetSettings(Settings::LOAD_DLL_COPY).toBool());

        QtComponents::AddCheckBox(OptionsSection, "Hijack Handle", false,    { 40, 165, 118, 20 })->WhenStateChanged([&](int state) {
            Options.HijackHandle = state;
            SaveSettings(Settings::HIJACK_HANDLE, state);
            })->CheckBox->setChecked(GetSettings(Settings::HIJACK_HANDLE).toBool());

        QtComponents::AddCheckBox(OptionsSection, "Random file name", false, { 190, 165, 150, 20 })->WhenStateChanged([&](int state) {
            Options.RandomFileName = state;
            SaveSettings(Settings::RANDOM_FILE_NAME, state);
            })->CheckBox->setChecked(GetSettings(Settings::RANDOM_FILE_NAME).toBool());

        QtComponents::AddCheckBox(OptionsSection, "Cloak thread", false,     { 40, 205, 118, 20 })->WhenStateChanged([&](int state) {
            Options.CloakThread = state;
            SaveSettings(Settings::CLOAK_THREAD, state);
            })->CheckBox->setChecked(GetSettings(Settings::CLOAK_THREAD).toBool());

        QtComponents::AddCheckBox(OptionsSection, "Remove from PEB", false, { 190, 205, 140, 20 })->WhenStateChanged([&](int state) {
            Options.RemoveFromPEB = state;
            SaveSettings(Settings::REMOVE_FROM_PEB, state);
            })->CheckBox->setChecked(GetSettings(Settings::REMOVE_FROM_PEB).toBool());

        QtComponents::AddText(OptionsSection, "Arch", 12, false, true, { 40,250,50,16 });
        QtComponents::AddComboBox(OptionsSection, { 45,275,55,30 })->AddItems({ "x64","x32" })->WhenIndexChanged([&](int index) {
            qDebug() << " INDEXXXXXX = " << index;
            Options.Arch = index;
            SaveSettings(Settings::ARCH, index);
        })->ComboBox->setCurrentIndex(GetSettings(Settings::ARCH).toInt());

        if (!IsAdmin)
        {
            QButton* DBRestartAsAdmin = QtComponents::AddButton(OptionsSection, " Restart as Admin", 12, false, { 190,275,130,30 });
            DBRestartAsAdmin->Button->setIcon(qApp->style()->standardIcon(QStyle::SP_VistaShield));
            DBRestartAsAdmin->Button->setObjectName("BPRestartAdmin");
            DBRestartAsAdmin->click([&] {
                char szPath[MAX_PATH];
                if (GetModuleFileNameA(NULL, szPath, ARRAYSIZE(szPath)))
                {
                    SHELLEXECUTEINFOA  sei = { sizeof(sei) };

                    sei.lpVerb = "runas";
                    sei.lpFile = szPath;
                    sei.hwnd = NULL;
                    sei.nShow = SW_NORMAL;

                    if(ShellExecuteExA(&sei))
                         qApp->exit();
                }
                else
                    QMessageBox(QMessageBox::Critical, "Error", "Can't run as admin !", QMessageBox::Ok, MainWindow, Qt::Drawer).exec();

                });
        }

        QtComponents::AddButton(OptionsSection, "Clear  options", 12, false, { 280,415,75,25 })->click([&] {
            settings->clear();
            })->Button->setObjectName("BPSaveOption");

        tabwidget->setStyleSheet(QMENU_STYESHEET);
        CentralWidget->setStyleSheet(INJECTOR_STYLESHEET);
    }
};


QT_END_NAMESPACE

#endif // UI_SLNINJECTOR_H

