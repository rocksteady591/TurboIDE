#include "createproject.h"
#include "editor.h"
#include "mainwindow.h"
#include "ui_createproject.h"
#include <fstream>
#include <QFileDialog>
#include <QMessageBox>

CreateProject::CreateProject(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateProject)
{
    ui->setupUi(this);
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
        QLineEdit{
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

CreateProject::~CreateProject()
{
    delete ui;
}

void CreateProject::on_pb_create_2_clicked()
{
    QString folder_path = QFileDialog::getExistingDirectory(this,
                                                            QString("Open folder"),
                                                            QDir::currentPath(),
                                                            QFileDialog::ShowDirsOnly
                                                                | QFileDialog::DontResolveSymlinks);
    if(folder_path.isEmpty()){
        return;
    }
    ui->le_project_path->setText(folder_path + "/" + project_name_);
    path_ = folder_path;

}


void CreateProject::on_le_project_name_textEdited(const QString &arg1)
{
    project_name_ = arg1;
    ui->le_project_path->setText(path_ + "/" + project_name_);
}


void CreateProject::on_pb_create_clicked()
{
    std::filesystem::path new_dir = path_.toStdString() + "/" + project_name_.toStdString();
    if(std::filesystem::create_directory(new_dir)){
        std::ofstream out_cpp;
        out_cpp.open(new_dir / "main.cpp");
        std::ofstream out_cmake;
        out_cmake.open(new_dir / "CMakeLists.txt");
        if(out_cpp.is_open()){
            if(out_cmake.is_open()){
                out_cmake << "cmake_minimum_required(VERSION 3.15)\n";
                out_cmake << "project(" << project_name_.toStdString() << ")\n";
                out_cmake << "set(CMAKE_CXX_STANDARD 20)\n";
                out_cmake << "add_executable(" << project_name_.toStdString() << " main.cpp)\n";
            }
            out_cpp << "#include <iostream>\n";
            out_cpp << "int main(){\n";
            out_cpp << "\tstd::cout << \"Hello, world!\";\n";
            out_cpp << "}\n";
            this->close();
            Editor* editor = new Editor(path_ + "/" + project_name_);
            auto* parent_ptr = qobject_cast<MainWindow*>(parent());
            if(parent_ptr){
                parent_ptr->updateHistory(path_ + "/" + project_name_);
            }
            editor->setAttribute(Qt::WA_DeleteOnClose);
            editor->show();
        }else {
            return;
        }
        out_cmake.close();
        out_cpp.close();
    }else {
        QMessageBox::information(this, "TurboIDE", "Директория с таким именем существует!");
        return;
    }
}

