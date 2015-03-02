#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "study.h"
#include <iostream>

using namespace std;

void MainWindow::onCurrentProgress()
{
    ui->webView->setUrl(QString(URL_BASE) + "running");
}

void MainWindow::onUpload()
{
    ui->webView->setUrl(QString(URL_BASE) + "upload");
}

void MainWindow::onInformation()
{
    ui->webView->setUrl(QString(URL_BASE) + "information");
}

void MainWindow::onContact()
{
    ui->webView->setUrl(QString(URL_BASE) + "contact");
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionCurrentProgress, SIGNAL(triggered()), this, SLOT(onCurrentProgress()));
    connect(ui->actionUploadData, SIGNAL(triggered()), this, SLOT(onUpload()));
    connect(ui->actionInformationSheet, SIGNAL(triggered()), this, SLOT(onInformation()));
    connect(ui->actionContact, SIGNAL(triggered()), this, SLOT(onContact()));

}

MainWindow::~MainWindow()
{
    delete ui;
}
