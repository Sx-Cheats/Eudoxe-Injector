#pragma once

#include "gui.h"

class EudoxeInjector : public QMainWindow, public Ui_SlnInjectorClass
{
    Q_OBJECT

public:
    EudoxeInjector(QWidget* parent = nullptr)
        : QMainWindow(parent), Ui_SlnInjectorClass()
    {
        setupUi(this);
        BPInject->click([&] 
        {
                qDebug() << "Options.Arch           = "     <<  Options.Arch;
                qDebug() << "Options.AutoInject     = "     <<  Options.AutoInject;
                qDebug() << "Options.CloakThread    = "     <<  Options.CloakThread;
                qDebug() << "Options.DllPath        = "     <<  Options.DllPath;
                qDebug() << "Options.HijackHandle   = "     <<  Options.HijackHandle;
                qDebug() << "Options.InjectMethod   = "     <<  Options.InjectMethod;
                qDebug() << "Options.LoadDllCopy    = "     <<  Options.LoadDllCopy;
                qDebug() << "Options.Loader         = "     <<  Options.Loader;
                qDebug() << "Options.Process        = "     <<  Options.Process;
                qDebug() << "Options.RandomFileName = "     <<  Options.RandomFileName;
                qDebug() << "Options.RemoveFromPEB  = "     <<  Options.RemoveFromPEB;
                qDebug() << "Options.Timeout        = "     <<  Options.Timeout;
                qDebug() << "--------------------------";
        
        });


    }

    ~EudoxeInjector() {};

private:
};
