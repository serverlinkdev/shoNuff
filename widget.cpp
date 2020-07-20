#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
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
    if (!exists(ut2004ExePath)) return;
    // make user reselect file if they answered No
    ui->pbModifyRegistry->setEnabled(false);
    if (!confirmWriteChanges(ut2004ExePath)) return;

// Write protocolKey to the registry.  We wont fail if their is existing,
// so as to allow them to modify the existing.
    if (!writeProtocolkey())
    {
        reportFail();
        return;
    }

// Write the users path to the registry:
    QSettings commandKey(
        "HKEY_CLASSES_ROOT\\ut2004\\shell\\open\\command",
        QSettings::NativeFormat);

    auto fullCommand = convertToNativeSeparators(ut2004ExePath) + " %1";
    commandKey.setValue(".", fullCommand);

// Test that we really did it or not:
    if (confirmChangesMade(fullCommand))
    {
        QMessageBox::information(
                    nullptr,
                    "Success",
                    "All done! Your changes were made successfully!");
    }
    else
    {
        reportFail();
    }
}

void Widget::on_pbFind2k4Exe_clicked()
{
    ut2004ExePath = QFileDialog::getOpenFileName(
                this, "Find ut2004.exe", "/", "*.exe");

    if (ut2004ExePath.length() == 0) return; // user cancelled
    if (!isFileAnExecutable(ut2004ExePath)) return; // wrong file type by user

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

bool Widget::isFileAnExecutable(QString &someFile)
{
    QFileInfo x(someFile);
    return x.suffix()=="exe";
}

bool Widget::confirmWriteChanges(QString &someFile)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Modify the Windows Registry?");
    msgBox.setText("Your chose the following as your UT2004 launcher:");
    msgBox.setInformativeText(convertToNativeSeparators(someFile) +
                              "\n\nAdd this info to the registry?\n\n"
                              "This cannot be undone.");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    // This is a *trick* so we can get the msgBox to resize so as to display
    // the user selected path on one row only and does not word wrap which
    // will confuse user's otherwise.
    QSpacerItem* horizontalSpacer =
        new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    QGridLayout* layout = (QGridLayout*)msgBox.layout();
    layout->addItem(
        horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    int answer = msgBox.exec();

    switch (answer)
    {
    case QMessageBox::Yes:
        return true;
    case QMessageBox::No:
        return false;
    }
    return false;
}

bool Widget::confirmChangesMade(const QString &fullCommand)
{
    // Tests: 1st check for HKEY_CLASSES_ROOT\\ut2004
    QSettings settings(
        "HKEY_CLASSES_ROOT\\ut2004\\shell\\open", QSettings::NativeFormat);

    if (settings.childGroups().contains("command", Qt::CaseSensitive))
    {
        // Tests: now check to see if the users path is really stored!

        QSettings testBed(
                    "HKEY_CLASSES_ROOT\\ut2004\\shell\\open\\command",
                    QSettings::NativeFormat);

        if (!testBed.contains("Default")) return false;

        auto var = testBed.value("Default").toString(); // path to UT2004.exe
        if (var != fullCommand)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    return false;
}

bool Widget::writeProtocolkey()
{
// Create the keys first:
    QSettings protocolKey("HKEY_CLASSES_ROOT\\ut2004", QSettings::NativeFormat);

    QString data = "URL:ut2004 Protocol";
    protocolKey.setValue(".", data);

    auto name = "URL Protocol";
    protocolKey.setValue(name, "");

// Tests: 1st check for HKEY_CLASSES_ROOT\\ut2004
    QSettings settings("HKEY_CLASSES_ROOT\\", QSettings::NativeFormat);

    if (settings.childGroups().contains("ut2004", Qt::CaseSensitive))
    {
// Tests: now check for each of the two string values we set above:

        QSettings testBed("HKEY_CLASSES_ROOT\\ut2004", QSettings::NativeFormat);

        if (!testBed.contains("Default")) return false; // URL:ut2004 Protocol

        auto var = testBed.value("Default").toString(); // URL:ut2004 Protocol
        if (var != data) return false;

        if (!testBed.contains("URL Protocol")) return false;
        qDebug().noquote() << "success! " << Q_FUNC_INFO;
        return true;
    }
    else
    {
        qDebug().noquote() << "The HKEY_CLASSES_ROOT\\ut2004 key was not created!";
        return false;
    }
    return false;
}

void Widget::reportFail()
{
    QMessageBox::critical(
                nullptr,
                "Fail",
                "Changes to registry not successful!");
}

