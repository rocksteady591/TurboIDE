#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createproject.h"
#include "createfile.h"
#include "gitaddwidget.h"
#include "editor.h"
#include <QFileDialog>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("TurboIDE");
    ui->statusbar->hide();
    loadHistory();
    this->setStyleSheet(R"(
        QMainWindow {
            background-color: #011627;
        }
        QWidget {
            background-color: #011627;
        }
        QLabel{
            color: #105a7e;
        }
        QGroupBox{
            color: #105a7e;
        }
        QPushButton {
            background-color: #011627;
            border: none;
            color: #105a7e;
            padding: 6px 12px;
            font-size: 15px;
            text-align: left;
        }
        QPushButton:hover {
            color: #f5f9fe;
            background-color: #0b2942;
        }
        QPushButton:pressed {
            color: #93d0ea;
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
    updateHistory(folder_path);
    openProject(folder_path);
}

void MainWindow::updateHistory(const QString& path){
    QSettings settings("History", "TurboIDE");
    //читаю список
    QStringList all_paths = settings.value("recentProjects").toStringList();
    //удаляю дубликаты
    all_paths.removeAll(path);
    //вставляю в начало
    all_paths.prepend(path);
    while(all_paths.size() > count_path_history_){
        all_paths.removeLast();
    }
    //сохраняю обратно
    settings.setValue("recentProjects", all_paths);
}

void MainWindow::openProject(const QString& path){
    Editor* editor = new Editor(path);
    editor->setAttribute(Qt::WA_DeleteOnClose);
    editor->show();
    this->close();
}

void MainWindow::loadHistory(){
    QLayout* layout = ui->gb_history->layout();
    QLayoutItem* item;

    while((item = layout->takeAt(0)) != nullptr){
        if(item->widget()){
            delete item->widget();
        }
        delete item;
    }
    QSettings settings("History", "TurboIDE");
    QStringList all_paths = settings.value("recentProjects").toStringList();

    for(int i = 0; i < all_paths.size(); ++i){
        QPushButton* button = new QPushButton(QFileInfo(all_paths[i]).fileName(), this);
        button->setToolTip(all_paths[i]);
        layout->addWidget(button);
        connect(button, &QPushButton::clicked, this, [all_paths, i, this](){
            openProject(all_paths[i]);
        });
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
    GitAddWidget* dialog = new GitAddWidget(this);
    //dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle("Clone repository");
    dialog->show();
}

