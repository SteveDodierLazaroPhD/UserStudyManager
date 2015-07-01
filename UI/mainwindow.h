/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Model/part.h"
#include "Model/step.h"
#include "Services/webviewservice.h"
#include <QMainWindow>
#include <QSettings>

namespace Ui {
class MainWindow;
class UploadingToolbarForm;
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
    void showUploadUI();

private:
    Ui::MainWindow *ui;
    Ui::UploadingToolbarForm *form;
    WebViewService *nm; // OWNED BY STUDYUTILS!
    bool ongoingUpload;
    Part uploadPart;
    Step uploadStep;

    void saveSettings();
    void loadSettings();

    void finishUpload();

protected slots:
    void launchProcess(const QString &name) const;
    void launchActivityJournal() const;
    void launchActivityLogManager() const;
    void launchUrl(const QUrl &url) const;

    /* Events that dictate which UI is being shown */
    void onPageLoadStarted();
    void onPageLoaded(const bool);
    void onUnsupportedStepQueried(Part part, Step step);

    /* Progress reporting */
    void onProgressReportQueried(const QString &);
    void onReportStarting(const Part &part, const Step &step);
    void onReportTargetElicited(const qint64 &target);
    void onReportStep(const qint64 &daysChecked, const qint64 &tmpResult);
    void onReportSuccess(const Part &part, const Step &step, const qint64 &result);
    void onReportRequestFailure(const QString &message);

    /* Archive packaging */
    void onPackagingStarting(const Part &part, const Step &step);
    void onPackagingTargetElicited(const qint64 &target, const QString &nextFile);
    void onPackagingStep(const qint64 &daysChecked, const QString &nextFile, const qint64 &tmpFileSize);
    void onPackagingSuccess(const Part &part, const Step &step, const QString &filename, const qint64 &fileSize, const QString &checksum);
    void onPackagingFailure(const QString &message);

    /* Data uploading */
    void onUploadJobActionRequested(const QString &);
    void onUploadStarted(const Part &part, const Step &step);
    void onUploadTargetSet(const Part &part, const Step &step, const qint64 &expectedSize);
    void onUploadStep(const Part &part, const Step &step, const qint64 &sentSize);
    void onUploadSucceeded(const Part &part, const Step &step);
    void onUploadFailed(const Part &part, const Step &step, const QString &errMsg);

    void onShowUploadButtonClicked();
    void onLoadWebsiteButtonClicked();
};

#endif // MAINWINDOW_H
