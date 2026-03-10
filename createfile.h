#ifndef CREATEFILE_H
#define CREATEFILE_H

#include <QDialog>
#include <QWidget>

namespace Ui {
class CreateFile;
}

class CreateFile : public QDialog
{
    Q_OBJECT

public:
    explicit CreateFile(QWidget *parent = nullptr);
    explicit CreateFile(const QString& path, QWidget *parent = nullptr);
    ~CreateFile();

private slots:
    void on_pb_path_clicked();

    void on_le_file_name_textEdited(const QString &arg1);

    void on_pb_create_clicked();

private:
    Ui::CreateFile *ui;
    bool create_file_in_current_folder_ = false;
    QString path_;
    QString file_name_ = "test_file.cpp";
};

#endif // CREATEFILE_H
