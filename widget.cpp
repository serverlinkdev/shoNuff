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
    setCssStyleSheet();
    auto icon = ":/images/shoNuff.png";
    setWindowIcon(QIcon(icon));
    ui->pbModifyRegistry->setDisabled(true);
}

Widget::~Widget()
{
    delete ui;
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
        return (var != fullCommand) ? false : true;
    }
    return false;
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

QString Widget::convertToNativeSeparators(const QString &someFile)
{
    auto x = QDir::toNativeSeparators(someFile);
    return ut2004ExePath = QString(x); // deep copy
}

bool Widget::exists(const QString &someFile)
{
    QFileInfo fileInfo(someFile);
    return ((fileInfo.exists() && fileInfo.isFile()));
}

bool Widget::isOkToProceed()
{
    if (!exists(ut2004ExePath)) return false;
    // make user reselect file if they answered No
    ui->pbModifyRegistry->setEnabled(false);
    return (confirmWriteChanges(ut2004ExePath));
}

bool Widget::isFileAnExecutable(QString &someFile)
{
    QFileInfo x(someFile);
    return x.suffix()=="exe";
}

void Widget::on_pbFind2k4Exe_clicked()
{
    // strip last two param's of the method call to use
    // native windows file selector dialog box
    ut2004ExePath =
        QFileDialog::getOpenFileName(
            this,
            "Find UT2004.exe",
            "/",
            "*.exe",
            0,
            QFileDialog::DontUseNativeDialog);

    if (ut2004ExePath.length() == 0) return; // user cancelled
    if (!isFileAnExecutable(ut2004ExePath)) return; // wrong file type by user

    ui->pbModifyRegistry->setDisabled(false); // allow user to press modify
}

// Sample data users would encounter:
// ut2004://66.150.121.29:7777?spectatorOnly=1
void Widget::on_pbModifyRegistry_clicked()
{
    // NOTE: if a 'normal' user runs this, and the values
    // they choose is the same as existing in registry
    // this App says changes made successfully.

    // perform some tests to be sure we have sane environment
    if (!isOkToProceed()) return;

    // Write protocolKey to the registry.  We wont fail if there is existing,
    // so as to allow them to modify the existing.
    if (!writeProtocolkey())
    {
        reportFail();
        return;
    }

    // Write the users path to the registry:
    fullCommand = convertToNativeSeparators(ut2004ExePath) + " %1";
    writeCommandKey();

    // Test that we really did it or not and report it to user:
    report();
}

void Widget::report()
{
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

void Widget::reportFail()
{
    QMessageBox::critical(
                nullptr,
                "Fail",
                "Changes to registry not successful!");
}

void Widget::setCssStyleSheet()
{
    QFile file(":/qss/Dark.qss");
    file.open(QFile::ReadOnly);
    auto sheet = QString::fromLatin1(file.readAll());

    qApp->setStyleSheet(sheet);
}

// Beware of false positives, Qt will not fail/crash and reports the default
// so you MUST test any changes
void Widget::writeCommandKey()
{
    QSettings commandKey(
        "HKEY_CLASSES_ROOT\\ut2004\\shell\\open\\command",
        QSettings::NativeFormat);

    commandKey.setValue(".", fullCommand);
}

// Beware of false positives, Qt will not fail/crash and reports the default
// so you MUST test any changes
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
        return true;
    }
    return false;
}
