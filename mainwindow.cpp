#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createproject.h"
#include "createfile.h"
#include "gitaddwidget.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    for (auto btn : this->findChildren<QPushButton*>()) {
        btn->setFlat(true);
    }
    this->setWindowTitle("PrimalIDE");
    delete ui->statusbar;
    this->setStyleSheet(R"(
    QPushButton {
        background-color: transparent;
        border: none;
        color: #c5b597;
        padding: 6px 12px;
        font-size: 11px;
        text-align: left;
    }
    QPushButton:flat {
        background-color: #5b524c;
    }
    QPushButton:hover {
        background-color: #5b524c;
        color: #c5b597;
    }

    QPushButton:pressed {
        background-color: #5b524c;
    }

)");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_new_file_clicked()
{
    CreateFile new_file(this);
    new_file.setWindowTitle("Create file");
    new_file.exec();
}


void MainWindow::on_pb_open_project_clicked()
{
    QString folder_path = QFileDialog::getExistingDirectory(this,
                                                            QString("Open project"),
                                                            QDir::currentPath(),
                                                            QFileDialog::ShowDirsOnly
                                                                | QFileDialog::DontResolveSymlinks);
    if(folder_path.isEmpty()){
        return;
    }
}


void MainWindow::on_pb_create_project_clicked()
{
    CreateProject create(this);
    create.setWindowTitle("Create project");
    create.exec();
}


void MainWindow::on_pb_clone_repo_clicked()
{
    GitAddWidget dialog(this);
    dialog.setWindowTitle("Clone repository");
    dialog.exec();
}

