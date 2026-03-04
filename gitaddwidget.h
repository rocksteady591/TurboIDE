#ifndef GITADDWIDGET_H
#define GITADDWIDGET_H

#include "qdialog.h"
#include <QWidget>

namespace Ui {
class GitAddWidget;
}

class GitAddWidget : public QDialog
{
    Q_OBJECT

public:
    explicit GitAddWidget(QWidget *parent = nullptr);
    ~GitAddWidget();

private slots:
    void on_pb_path_clicked();

    void on_pb_clone_clicked();

    void on_le_path_textEdited(const QString &arg1);

    void on_le_github_ssh_textEdited(const QString &arg1);

    void CloneRepository(const QString& ssh_path);

    void onGitOutput(const QString& text); // обработка вывода

private:
    Ui::GitAddWidget *ui;
    QString path_;
    QWidget *QDialog;
};

#endif // GITADDWIDGET_H
