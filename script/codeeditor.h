#ifndef CODEEDITOR_H
#define CODEEDITOR_H
#include <QPlainTextEdit>
#include<QSplitter>
#include<QLayout>
#include<QMainWindow>
#include <QPainter>
#include <QTextBlock>
#include<QTableWidgetItem>
#include<QHeaderView>
#include<QVBoxLayout>
#include<QGroupBox>
#include<QStringList>
#include<QPoint>
QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE
class LineNumberArea;
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
protected:
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
    QWidget *widget;
    QTableWidget *tableWidget;
    QStringList header;



};
class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};



#endif
/*
std::vector<std::string> reserved_keywords = {

    "IF",
    "ENDIF",
    "ELSE",


    "WHILE",
    "ENDWHILE",

    "FOR",
    "TO",
    "STEP",
    "ENDFOR",

    "ASIGN",//赋值语句

    "PRINT",
    "LOAD",
    "SAVE",

    "ENDL",
    };
    */

