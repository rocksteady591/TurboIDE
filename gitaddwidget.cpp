#include "gitaddwidget.h"
#include "ui_gitaddwidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QRegularExpression>

GitAddWidget::GitAddWidget(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GitAddWidget)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    ui->progressBar->hide();
}

GitAddWidget::~GitAddWidget()
{
    delete ui;
}

void GitAddWidget::onGitOutput(const QString& text) {
    // выводим текст в plainTextEdit
    ui->plainTextEdit->appendPlainText(text);

    // git пишет прогресс вот так: "Receiving objects:  45% (90/200)"
    QRegularExpression re(R"((\d+)%)");
    QRegularExpressionMatch match = re.match(text);
    if (match.hasMatch()) {
        int percent = match.captured(1).toInt();
        ui->progressBar->setValue(percent);
    }
}

void GitAddWidget::CloneRepository(const QString& ssh_path){
    ui->progressBar->show();
    ui->progressBar->setValue(0);
    ui->plainTextEdit->clear();

    QProcess* process = new QProcess(this);
    // git пишет прогресс в stderr — это нормально
    connect(process, &QProcess::readyReadStandardError, [=]() {
        onGitOutput(process->readAllStandardError());
    });

    connect(process, &QProcess::readyReadStandardOutput, [=]() {
        onGitOutput(process->readAllStandardOutput());
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus) {
                if (exitCode == 0) {
                    ui->progressBar->setValue(100);
                    QMessageBox::information(this, "TurboIDE", "Клонирование завершено");
                    ui->pb_clone->setText("Open");
                } else {
                    QMessageBox::critical(this, "TurboIDE", "Ошибка при клонировании");
                }
                process->deleteLater();
            });

    process->start("git", {"clone", "--progress", ssh_path, path_});
}

void GitAddWidget::on_pb_path_clicked()
{
    QString folder_path = QFileDialog::getExistingDirectory(this,
                                                            QString("Open folder"),
                                                            QDir::currentPath(),
                                                            QFileDialog::ShowDirsOnly
                                                                | QFileDialog::DontResolveSymlinks);
    if(folder_path.isEmpty()){
        return;
    }
    ui->le_path->setInputMask("");
    ui->le_path->setStyleSheet("color: rgb(197, 181, 151);");
    ui->le_path->setText(folder_path);
    path_ = folder_path;
}


void GitAddWidget::on_pb_clone_clicked()
{
    if(ui->pb_clone->text().toStdString() == "Clone"){
        if(ui->le_path->text().isEmpty()){
            ui->le_path->setInputMask("Выберите путь");
            ui->le_path->setStyleSheet("color: red;");
            return;
        }
        if(ui->le_github_ssh->text().isEmpty()){
            ui->le_github_ssh->setInputMask("Вставьте ssh ключ");
            ui->le_github_ssh->setStyleSheet("color: red;");
            return;
        }
        CloneRepository(ui->le_github_ssh->text());
    }else if(ui->pb_clone->text().toStdString() == "Open"){

    }

}


void GitAddWidget::on_le_path_textEdited(const QString&)
{
    ui->le_path->setInputMask("");
    ui->le_path->setStyleSheet("color: rgb(197, 181, 151);");
}


void GitAddWidget::on_le_github_ssh_textEdited(const QString&)
{
    ui->le_github_ssh->setInputMask("");
    ui->le_github_ssh->setStyleSheet("color: rgb(197, 181, 151);");
}



