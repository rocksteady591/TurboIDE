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
    //редактор
    CodeEditor* text_edit = new CodeEditor(splitter);
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
    //text_edit->setAttribute(Qt::WA_DeleteOnClose);
    setCentralWidget(splitter);
    splitter->setSizes({250, 750});


    connect(tree, &QTreeView::clicked, this, [model, text_edit, this](const QModelIndex& index){
        QString path = model->filePath(index);
        open_file_ = path;
        QFile file_in_textEdit(path);
        if(file_in_textEdit.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream file_is_string(&file_in_textEdit);
            text_edit->setPlainText(file_is_string.readAll());
            text_edit->setWindowFilePath(open_file_);
            file_in_textEdit.close();
        }else{
            QMessageBox::information(this, "TurboIDE", "Невозможно открыть файл");
        }
    });
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

