#include "editor.h"
#include "gitaddwidget.h"
#include "mainwindow.h"
#include "ui_gitaddwidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QRegularExpression>

GitAddWidget::GitAddWidget(class QWidget *parent)
    : QWidget(parent, Qt::Window)
    , ui(new Ui::GitAddWidget)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    ui->progressBar->hide();

    this->setStyleSheet(R"(
        QMainWindow {
            background-color: #011627;
        }
        QWidget {
            background-color: #011627;
        }
        QLineEdit{
            color: #105a7e;
        }
        QLabel{
            color: #105a7e;
        }
        QPlainTextEdit{
            color: white;
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
    if(parent){
        QPoint center = parent->geometry().center();
        this->adjustSize(); // пересчитываем размер до показа
        this->move(center.x() - this->width() / 2,
                   center.y() - this->height() / 2);
    }
}

GitAddWidget::~GitAddWidget()
{
    delete ui;
}

void GitAddWidget::onGitOutput(const QString& text) {
    // выводим текст в plainTextEdit
    ui->plainTextEdit->appendPlainText(text);

    // прогресс
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
    // git пишет прогресс в stderr
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

    QString repo_name = ssh_path.section('/', -1).section('.', 0, 0);
    QString full_path = QDir(path_).filePath(repo_name);
    path_ = full_path;
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
    if(ui->pb_clone->text() == "Clone"){
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
    }else if(ui->pb_clone->text() == "Open"){
        auto* parent_ptr = qobject_cast<MainWindow*>(parent());
        if(parent_ptr != nullptr){
            parent_ptr->updateHistory(path_);
        }

        Editor* editor = new Editor(path_, nullptr);
        editor->setAttribute(Qt::WA_DeleteOnClose);
        editor->show();

        this->close();
        if(parent_ptr != nullptr){
            parent_ptr->close();
        }
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



