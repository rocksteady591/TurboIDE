#ifndef CREATEPROJECT_H
#define CREATEPROJECT_H

#include <QDialog>
#include <QWidget>

namespace Ui {
class CreateProject;
}

class CreateProject : public QDialog
{
    Q_OBJECT

public:
    explicit CreateProject(QWidget *parent = nullptr);
    ~CreateProject();

private slots:
    void on_pb_create_2_clicked();

    void on_le_project_name_textEdited(const QString &arg1);

    void on_pb_create_clicked();

private:
    Ui::CreateProject *ui;
    QString path_;
    QString project_name_ = "test_project";
};

#endif // CREATEPROJECT_H
