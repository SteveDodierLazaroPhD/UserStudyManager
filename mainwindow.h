#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include "step.h"
#include "webviewservice.h"

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
    WebViewService *nm; // OWNED BY STUDYUTILS!
    bool ongoingUpload;

    void saveSettings();
    void loadSettings();

protected slots:
    void launchProcess(const QString &name) const;
    void launchActivityJournal() const;
    void launchActivityLogManager() const;
    void launchUrl(const QUrl &url) const;

    /* Events that dictate which UI is being shown */
    void onPageLoadStarted();
    void onPageLoaded(const bool);
    void onUnsupportedStepQueried(Part part, Step step);

    void onProgressReportQueried(const QString &);
    void onReportStarting(const Part &part, const Step &step);
    void onReportTargetElicited(const qint64 &target);
    void onReportStep(const qint64 &daysChecked, const qint64 &tmpResult);
    void onReportSuccess(const Part &part, const Step &step, const qint64 &result);
    void onReportRequestFailure(const QString &message);

    void onPackagingStarting(const Part &part, const Step &step);
    void onPackagingTargetElicited(const qint64 &target, const QString &nextFile);
    void onPackagingStep(const qint64 &daysChecked, const QString &nextFile, const qint64 &tmpFileSize);
    void onPackagingSuccess(const Part &part, const Step &step, const QString &filename, const qint64 &fileSize);
    void onPackagingFailure(const QString &message);

    void onUploadStart();
    void onUploadFinished();

    /* Local UI events */
    void setToolbarEnabled();
    void onLoadWebsiteButtonClicked();
};

#endif // MAINWINDOW_H
