#include "createfile.h"
#include "editor.h"
#include "ui_editor.h"

#include <QSplitter>
#include <QFileSystemModel>
#include <QTreeView>
#include <QPlainTextEdit>
#include <QPainter>
#include <QTextBlock>
#include <QMessageBox>
#include <QShortcut>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QProcess>


CppHighlighter::CppHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent){
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor("#c96c6b"));
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList keywords = {
        "\\bint\\b", "\\bvoid\\b", "\\bclass\\b", "\\bstruct\\b",
        "\\bif\\b", "\\belse\\b", "\\bfor\\b", "\\bwhile\\b",
        "\\breturn\\b", "\\bconst\\b", "\\bauto\\b", "\\bbool\\b"
    };

    for(const auto& keyword : keywords){
        rules_.append({QRegularExpression(keyword), keywordFormat});
    }

    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor("#6a9955"));
    rules_.append({QRegularExpression("//[^\n]*"), commentFormat});

    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor("#ce9178"));
    rules_.append({QRegularExpression("\".*\""), stringFormat});

    QTextCharFormat funcFormat;
    funcFormat.setForeground(QColor("#65a2de")); // жёлтый
    rules_.append({ QRegularExpression("\\b[A-Za-z_][A-Za-z0-9_]*(?=\\()"), funcFormat });

    QTextCharFormat methodFormat;
    methodFormat.setForeground(QColor("#65a2de"));
    rules_.append({ QRegularExpression("(?<=\\.)[A-Za-z_][A-Za-z0-9_]*(?=\\()"), methodFormat });

    QTextCharFormat classFormat;
    classFormat.setForeground(QColor("#7864a0"));
    rules_.append({QRegularExpression("\\b[A-Za-z_][A-Za-z0-9_]*(?=\\::)"), classFormat});
}

void CppHighlighter::highlightBlock(const QString& text) {
    for(const Rule& rule : rules_){
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while(it.hasNext()){
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

void CodeEditor::resizeEvent(QResizeEvent* event) {
    QPlainTextEdit::resizeEvent(event);

    // берём область содержимого редактора
    QRect cr = contentsRect();
    int width = lineNumberArea_->width_hint;
    // позиционируем полосу по левому краю редактора
    lineNumberArea_->setGeometry(QRect(cr.left(), cr.top(), width, cr.height()));
}

void CodeEditor::updateLineNumberAreaWidth() {
    int digits = QString::number(qMax(1, blockCount())).length();
    int width = 16 + fontMetrics().horizontalAdvance('9') * digits;
    lineNumberArea_->width_hint = width;
    setViewportMargins(width, 0, 0, 0);

    // пересчитываем позицию
    QRect cr = contentsRect();
    lineNumberArea_->setGeometry(QRect(cr.left(), cr.top(), width, cr.height()));
}

// ── LineNumberArea ──────────────────────

void LineNumberArea::paintEvent(QPaintEvent* event) {
    editor_->lineNumberAreaPaintEvent(event); // делегируем в CodeEditor
}

// ── CodeEditor ──────────────────────────

CodeEditor::CodeEditor(QWidget* parent) : QPlainTextEdit(parent) {
    lineNumberArea_ = new LineNumberArea(this);

    connect(this, &QPlainTextEdit::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest,
            this, &CodeEditor::updateLineNumberArea);

    updateLineNumberAreaWidth();
}

// этот метод внутри CodeEditor — имеет доступ к protected методам QPlainTextEdit
void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event) {
    QPainter painter(lineNumberArea_);
    painter.fillRect(event->rect(), QColor("#1a1a1a"));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block)
                         .translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            painter.setPen(QColor("#555566"));
            painter.setFont(QFont("Courier New", 9));
            painter.drawText(0, top, lineNumberArea_->width_hint - 6,
                             fontMetrics().height(),
                             Qt::AlignRight,
                             QString::number(blockNumber + 1));
        }
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::updateLineNumberArea(const QRect& rect, int dy) {
    if (dy) lineNumberArea_->scroll(0, dy);
    else     lineNumberArea_->update(0, rect.y(), lineNumberArea_->width(), rect.height());
    updateLineNumberAreaWidth();
}



Editor::Editor(const QString& path, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Editor)
    , root_path_(path)
{
    ui->setupUi(this);
    this->setWindowTitle("TurboIDE");
    if (ui->statusbar) {
        ui->statusbar->hide();
    }
    this->setStyleSheet(R"(
        QMainWindow {
            background-color: #3b3330;
        }
        QWidget {
            background-color: #3b3330;
        }
        QLabel{
            color: #7a6e65;
        }
        QAction{
            border: #062d44;
            color: #7a6e65;
            background-color: #000a11;
        }
        QMenu{
            color: #7a6e65;
            border: #062d44;
            background-color: #000a11;
        }
        QTreeView{
            color: #7a6e65;
        }
        QPlainTextEdit{
            color: #ffffff;
            background-color: #262624;
        }
        QPushButton {
            background-color: #3b3330;
            border: none;
            color: #7a6e65;
            padding: 6px 12px;
            font-size: 11px;
            text-align: left;
        }
        QPushButton:hover {
            color: #f0dfc0;
        }
        QPushButton:pressed {
            color: #c5b597;
        }
)");
    //шорткат на сохранение файла из QPlainTextEdit
    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);
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

    //создание контекстного меню
    tree->setContextMenuPolicy(Qt::CustomContextMenu);


    //вертикальный сплиттер
    QSplitter* vertical_splitter = new QSplitter(Qt::Vertical, splitter);

    //редактор
    CodeEditor* text_edit = new CodeEditor(vertical_splitter);
    //панель терминала
    QWidget* terminalPanel = new QWidget(vertical_splitter);
    QVBoxLayout* termLayout = new QVBoxLayout(terminalPanel);
    termLayout->setContentsMargins(0, 0, 0, 0);
    QPlainTextEdit* terminalOutput = new QPlainTextEdit(terminalPanel);
    terminalOutput->setReadOnly(true);
    QLineEdit* inputTerminal = new QLineEdit(terminalPanel);
    inputTerminal->setPlaceholderText("$ введите команду...");
    termLayout->addWidget(terminalOutput);
    termLayout->addWidget(inputTerminal);
    terminalPanel->hide();
    //bash процесс
    QProcess* bash_process = new QProcess(this);
    bash_process->setWorkingDirectory(root_path_);
    bash_process->start("bash");
    connect(bash_process, &QProcess::readyReadStandardOutput, this, [=](){
        terminalOutput->appendPlainText(bash_process->readAllStandardOutput());
    });
    connect(bash_process, &QProcess::readyReadStandardError, this, [=](){
        terminalOutput->appendPlainText(bash_process->readAllStandardError());
    });
    connect(inputTerminal, &QLineEdit::returnPressed, this, [=](){
        QString cmd = inputTerminal->text();
        terminalOutput->appendPlainText("$ " + cmd);
        bash_process->write((cmd + "\n").toUtf8());
        inputTerminal->clear();
    });
    connect(ui->actionnew_terminal, &QAction::triggered, this, [=](){
        if(!terminalPanel->isVisible()){
            terminalPanel->show();
            terminalPanel->setFocus();
        }
    });
    connect(ui->actionclose_terminal, &QAction::triggered, this, [=](){
        if(terminalPanel->isVisible()){
            terminalPanel->hide();
        }
    });


    connect(shortcut, &QShortcut::activated, this, [text_edit, this](){
        QFile file(open_file_);
        if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QString text_with_window(text_edit->toPlainText());
            file.write(text_with_window.toUtf8());
            file.close();
        }else{
            QMessageBox::information(this, "TurboIDE", "Невозможно сохранить файл");
        }
    });
    new CppHighlighter(text_edit->document());
    setCentralWidget(splitter);
    splitter->setSizes({250, 750});


    connect(tree, &QTreeView::clicked, this, [model, text_edit, tree, this](const QModelIndex& index){
        QString path = model->filePath(index);
        open_file_ = path;
        QFile file_in_textEdit(path);
        if(file_in_textEdit.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream file_is_string(&file_in_textEdit);
            text_edit->setPlainText(file_is_string.readAll());
            text_edit->setWindowFilePath(open_file_);
            file_in_textEdit.close();
        }else{
            bool isExpanded = tree->isExpanded(index);
            tree->setExpanded(index, !isExpanded);

        }
    });

    connect(tree, &QTreeView::customContextMenuRequested, this, [this, tree, model](const QPoint& pos){
        this->showContextMenu(pos, tree, model);
    });
}

void Editor::showContextMenu(const QPoint& pos, QTreeView* tree, MyQFileSystemModel* model){
    QModelIndex index = tree->indexAt(pos);
    if(index.isValid()){
        QMenu contextMenu(this);
        QAction* actionDelete = contextMenu.addAction("Delete");
        QAction* actionNewFile = contextMenu.addAction("New file");
        QAction* actionNewFolder = contextMenu.addAction("New folder");

        connect(actionNewFile, &QAction::triggered, this, [index, model](){
            if(model->isDir(index)){
                QString folderPath = model->filePath(index);
                CreateFile* createNewFile = new CreateFile(folderPath, FolderOrFile::FILE);
                createNewFile->setAttribute(Qt::WA_DeleteOnClose);
                createNewFile->setWindowTitle("Create file");
                createNewFile->show();
            }
        });

        connect(actionDelete, &QAction::triggered, this, [model, this, index](){
            if(model->isDir(index)){
                QDir current_dir = model->filePath(index);
                if(current_dir.exists()){
                    if(current_dir.removeRecursively()){
                        QMessageBox::information(this, "TurboIDE", "Папка успешно удалена");
                    }else{
                        QMessageBox::information(this, "TurboIDE", "Ошибка, не удалось удалить папку");
                    }
                }
            }else{
                if(QFile::remove(model->filePath(index))){
                    QMessageBox::information(this, "TurboIDE", "Файл успешно удален");
                }else{
                    QMessageBox::information(this, "TurboIDE", "Ошибка, не удалось удалить файл");
                }
            }
        });

        connect(actionNewFolder, &QAction::triggered, this, [model, index](){
            if(model->isDir(index)){
                QDir current_dir = model->filePath(index);
                if(current_dir.exists()){
                    CreateFile* createNewFolder = new CreateFile(model->filePath(index), FolderOrFile::FOLDER);
                    createNewFolder->setAttribute(Qt::WA_DeleteOnClose);
                    createNewFolder->setWindowTitle("Create folder");
                    createNewFolder->show();
                }
            }
        });

        contextMenu.exec(tree->viewport()->mapToGlobal(pos));
    }
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

