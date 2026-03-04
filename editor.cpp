#include "editor.h"
#include "ui_editor.h"

#include <QSplitter>
#include <QFileSystemModel>
#include <QTreeView>
#include <QPlainTextEdit>

Editor::Editor(const QString& path, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Editor)
    , root_path_(path)
{
    ui->setupUi(this);
    delete ui->statusbar;
    //сплиттер горизонтальный
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    //модель файловой системы
    QFileSystemModel *model = new QFileSystemModel(this);
    model->setRootPath(root_path_);
    //дерево файлов
    QTreeView* tree = new QTreeView(splitter);
    tree->setModel(model);
    tree->setRootIndex(model->index(root_path_));
    //tree->setColumnHidden(1, true);
    tree->setHeaderHidden(true);
    //редактор
    QPlainTextEdit* text_edit = new QPlainTextEdit(splitter);
    text_edit->setAttribute(Qt::WA_DeleteOnClose);
    setCentralWidget(splitter);
    splitter->setSizes({250, 750});
}

Editor::~Editor()
{
    delete ui;
}
