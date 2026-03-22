#ifndef FINDWORD_H
#define FINDWORD_H

#include <QPlainTextEdit>
#include <QWidget>

namespace Ui {
class FindWord;
}

class FindWord : public QWidget
{
    Q_OBJECT

public:
    explicit FindWord(QPlainTextEdit* file,QWidget *parent = nullptr);
    ~FindWord();

private slots:

    void on_pb_top_clicked();

    void on_pb_bot_clicked();

    void on_pb_close_clicked();

private:
    Ui::FindWord *ui;
    QPlainTextEdit* file_;
};

#endif // FINDWORD_H
