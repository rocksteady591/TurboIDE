#ifndef EDITOR_H
#define EDITOR_H

#include <QFileSystemModel>
#include <QMainWindow>
#include <QPlainTextEdit>

namespace Ui {
class Editor;
}
class CodeEditor;

class MyQFileSystemModel : public QFileSystemModel {
    Q_OBJECT
public:
    explicit MyQFileSystemModel(QObject* parent = nullptr)
        : QFileSystemModel(parent) {}

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
};

// полоса с номерами — прикрепляется слева от редактора
class LineNumberArea : public QWidget {
public:
    LineNumberArea(CodeEditor* editor) : QWidget(reinterpret_cast<QWidget*>(editor)), editor_(editor) {}
    int width_hint = 40;
    QSize sizeHint() const override { return QSize(width_hint, 0); }
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    CodeEditor* editor_; // теперь CodeEditor а не QPlainTextEdit
};

// редактор с номерами строк
class CodeEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit CodeEditor(QWidget* parent = nullptr);
    void lineNumberAreaPaintEvent(QPaintEvent* event);
protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void updateLineNumberAreaWidth();
    void updateLineNumberArea(const QRect& rect, int dy);

private:
    LineNumberArea* lineNumberArea_;
};

class Editor : public QMainWindow {
    Q_OBJECT
public:
    explicit Editor(const QString& path, QWidget* parent = nullptr);
    ~Editor();
private:
    Ui::Editor* ui;
    QString root_path_;
};

#endif // EDITOR_H
