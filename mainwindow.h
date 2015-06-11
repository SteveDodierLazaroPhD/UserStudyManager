#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "step.h"
#include "webmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void hideAll();
    void showWebUI();
    void showQuickLinkUI();
    void showProgressCalcUI();

private:
    Ui::MainWindow *ui;
    WebManager *nm;
    bool ongoingUpload;

protected slots:
    void launchProcess(const QString &name) const;
    void launchActivityJournal() const;
    void launchActivityLogManager() const;

    /* Events that dictate which UI is being shown */
    void onPageLoadStarted();
    void onPageLoaded(const bool);
    void onProgressReportQueried(const QString &);
    void onStepQueried(Part part, Step step);

    void onUploadStart();
    void onUploadFinished();

    /* Local UI events */
    void setToolbarEnabled();
    void onLoadWebsiteButtonClicked();
};

#endif // MAINWINDOW_H
