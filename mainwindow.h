#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "step.h"
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

    void showWebUI();
    void showQuickLinkUI();

private:
    Ui::MainWindow *ui;
    UCLWebManager *nm;
    bool ongoingUpload;

protected slots:
    void launchProcess(const QString &name) const;
    void launchActivityJournal() const;
    void launchActivityLogManager() const;
    void onPageLoadStarted();
    void onPageLoaded(const bool);
    void onStepQueried(Part part, Step step);

    void onUploadStart();
    void onUploadFinished();
    void setToolbarEnabled();

    void onLoadWebsiteButtonClicked();
};

#endif // MAINWINDOW_H
