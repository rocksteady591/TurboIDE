#include "editor.h"
#include "ui_editor.h"

#include <QSplitter>
#include <QFileSystemModel>
#include <QTreeView>
#include <QPlainTextEdit>
#include <QPainter>
#include <QTextBlock>

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

    QTextBlock block = firstVisibleBlock(); // теперь без editor_->
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

    //редактор
    CodeEditor* text_edit = new CodeEditor(splitter);
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
