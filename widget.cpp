#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , ut2004ExePath("")
{
    ui->setupUi(this);
    ui->pbModifyRegistry->setDisabled(true);
}

Widget::~Widget()
{
    delete ui;
}


// Sample data users would encounter:
// ut2004://66.150.121.29:7777?spectatorOnly=1
void Widget::on_pbModifyRegistry_clicked()
{
    //#TODO disable this button if user has not selected a file yet

    //#TODO raise error
    if (!exists(ut2004ExePath)) return;

    auto fullCommand = convertToNativeSeparators(ut2004ExePath) + " %1";

    QSettings protcolKey(
        "HKEY_CLASSES_ROOT\\ut2004", QSettings::NativeFormat);

    protcolKey.setValue(".", "URL:ut2004 Protocol");
    protcolKey.setValue("URL Protocol", "");


    QSettings commandKey(
        "HKEY_CLASSES_ROOT\\ut2004\\shell\\open\\command", QSettings::NativeFormat);

    commandKey.setValue(".", fullCommand);
    // #TODO verify written to reg
    // #TODO show pop up that it's done
}

void Widget::on_pbFind2k4Exe_clicked()
{
    ut2004ExePath = QFileDialog::getOpenFileName(this, "Find ut2004.exe", "/");
    if (ut2004ExePath.length() == 0) return; // user cancelled
    ui->pbModifyRegistry->setDisabled(false);
}

bool Widget::exists(const QString &someFile)
{
    QFileInfo fileInfo(someFile);
    return ((fileInfo.exists() && fileInfo.isFile()));
}

QString Widget::convertToNativeSeparators(const QString &someFile)
{
    auto x = QDir::toNativeSeparators(someFile);
    return ut2004ExePath = QString(x); // deep copy
}

