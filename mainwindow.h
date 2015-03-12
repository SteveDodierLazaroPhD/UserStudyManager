#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "uclwebmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    UCLWebManager *nm;

protected slots:
    void launchProcess(const QString &name) const;
    void launchActivityJournal() const;
    void launchActivityLogManager() const;
    void onPageLoadStarted();
    void onPageLoaded(const bool);
};

#endif // MAINWINDOW_H
