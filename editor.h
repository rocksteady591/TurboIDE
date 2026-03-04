#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>

namespace Ui {
class Editor;
}

class Editor : public QMainWindow
{
    Q_OBJECT

public:
    explicit Editor(const QString& path_, QWidget *parent = nullptr);
    ~Editor();

private:
    Ui::Editor *ui;
    QString root_path_;
};

#endif // EDITOR_H
