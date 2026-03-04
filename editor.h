#ifndef EDITOR_H
#define EDITOR_H

#include <QFileSystemModel>
#include <QMainWindow>

namespace Ui {
class Editor;
}

class MyQFileSystemModel : public QFileSystemModel {
    Q_OBJECT
public:
    explicit MyQFileSystemModel(QObject* parent = nullptr)
        : QFileSystemModel(parent) {}

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
};

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
