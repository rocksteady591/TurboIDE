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
    this->setWindowTitle("PrimalIDE");
    delete ui->statusbar;
    //сплиттер горизонтальный
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    //модель файловой системы
    MyQFileSystemModel *model = new MyQFileSystemModel(this);
    model->setRootPath(root_path_);
    //дерево файлов
    QTreeView* tree = new QTreeView(splitter);
    tree->setModel(model);
    tree->setRootIndex(model->index(root_path_));
    tree->setColumnHidden(1, true);
    tree->setColumnHidden(2, true);
    tree->setColumnHidden(3, true);

    //tree->setHeaderHidden(true);
    //редактор
    QPlainTextEdit* text_edit = new QPlainTextEdit(splitter);
    text_edit->setAttribute(Qt::WA_DeleteOnClose);
    setCentralWidget(splitter);
    splitter->setSizes({250, 750});
}


QVariant MyQFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal){
        if(section == 0){
            return "Project";
        }
    }
    return QVariant();
}

Editor::~Editor()
{
    delete ui;
}
