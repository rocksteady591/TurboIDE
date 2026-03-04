#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pb_new_file_clicked();

    void on_pb_open_project_clicked();

    void on_pb_create_project_clicked();

    void on_pb_clone_repo_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
