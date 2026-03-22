#include "findword.h"
#include "ui_findword.h"
#include <QScreen>

FindWord::FindWord(QPlainTextEdit* file,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FindWord)
{
    ui->setupUi(this);
    file_ = file;
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 4;
    this->move(x, y);
    this->move(x, y);
    ui->le_text->setPlaceholderText("Find");
    // Убираем внешние отступы лейаута, чтобы кнопки не прижимались
    ui->horizontalLayout->setContentsMargins(5, 2, 5, 2);
    ui->horizontalLayout->setSpacing(5); // Расстояние между кнопками
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

FindWord::~FindWord()
{
    delete ui;
}



void FindWord::on_pb_top_clicked()
{
    if(ui->le_text->text().size() > 0){
        file_->find(ui->le_text->text(), QTextDocument::FindBackward);
    }
}


void FindWord::on_pb_bot_clicked()
{
    if(ui->le_text->text().size() > 0){
        file_->find(ui->le_text->text());
    }
}


void FindWord::on_pb_close_clicked()
{
    this->close();
}


