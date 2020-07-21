#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pbFind2k4Exe_clicked();
    void on_pbModifyRegistry_clicked();

private:
    Ui::Widget *ui;
    QString ut2004ExePath;

    bool exists(const QString & someFile);
    bool confirmChangesMade(const QString & fullCommand);
    bool confirmWriteChanges(QString & someFile);
    QString convertToNativeSeparators(const QString & someFile);
    QString fullCommand;
    bool isOkToProceed();
    bool isFileAnExecutable(QString & someFile);
    void report();
    void reportFail();
    void setCssStyleSheet();
    void writeCommandKey();
    bool writeProtocolkey();
};
#endif // WIDGET_H
