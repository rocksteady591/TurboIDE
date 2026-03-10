#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createproject.h"
#include "createfile.h"
#include "gitaddwidget.h"
#include "editor.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("TurboIDE");
    ui->statusbar->hide();
    this->setStyleSheet(R"(
        QMainWindow {
            background-color: #3b3330;
        }
        QWidget {
            background-color: #3b3330;
        }
        QLabel{
            color: #7a6e65;
        }
        QPushButton {
            background-color: #3b3330;
            border: none;
            color: #7a6e65;
            padding: 6px 12px;
            font-size: 11px;
            text-align: left;
        }
        QPushButton:hover {
            color: #f0dfc0;
        }
        QPushButton:pressed {
            color: #c5b597;
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
    Editor* editor = new Editor(folder_path);
    editor->setAttribute(Qt::WA_DeleteOnClose);
    editor->show();
    this->close();
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

