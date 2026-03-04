#include "createfile.h"
#include "ui_createfile.h"
#include <fstream>
#include <QFileDialog>
#include <QMessageBox>

CreateFile::CreateFile(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateFile)
{
    ui->setupUi(this);
    ui->le_file_name->setText(file_name_);
}

CreateFile::~CreateFile()
{
    delete ui;
}

void CreateFile::on_pb_path_clicked()
{
    QString folder_path = QFileDialog::getExistingDirectory(this,
                                                            QString("Open folder"),
                                                            QDir::currentPath(),
                                                            QFileDialog::ShowDirsOnly
                                                                | QFileDialog::DontResolveSymlinks);
    if(folder_path.isEmpty()){
        return;
    }
    path_ = folder_path;
    ui->le_file_path->setText(path_ + "/" + file_name_);
}


void CreateFile::on_le_file_name_textEdited(const QString &arg1)
{
    file_name_ = arg1;
    ui->le_file_path->setText(path_ + "/" + file_name_);
}


void CreateFile::on_pb_create_clicked()
{
    std::ofstream out(path_.toStdString() + "/" + file_name_.toStdString(), std::ios::app);
    if(out.is_open()){
        out << "#include <iostream>\n";
        out << "int main(){\n";
        out << "\tstd::cout << \"Hello, world!\";\n";
        out << "}\n";
        out.close();
        this->close();
    }else{
        QMessageBox::information(this, "TurboIDE", "Файл не удалось открыть!");
        return;
    }
}

