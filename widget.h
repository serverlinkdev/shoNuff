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
    void on_pbModifyRegistry_clicked();
    void on_pbFind2k4Exe_clicked();

private:
    Ui::Widget *ui;
    void setCssStyleSheet();
    QString ut2004ExePath;
    QString fullCommand;
    bool exists(const QString & someFile);
    QString convertToNativeSeparators(const QString & someFile);
    bool isFileAnExecutable(QString & someFile);
    bool confirmWriteChanges(QString & someFile);
    bool confirmChangesMade(const QString & fullCommand);
    void writeCommandKey();
    bool writeProtocolkey();
    void reportFail();
    void inspect();
    void report();
};
#endif // WIDGET_H
