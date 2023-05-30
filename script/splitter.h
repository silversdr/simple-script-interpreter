#ifndef SPLITTER_H
#define SPLITTER_H
#include<QObject>
#include<QWidget>
#include <QApplication>
#include <QTextCodec>
#include <QSplitter>
#include <QDirModel>
#include <QTreeView>
#include <QTextEdit>
#include<QGridLayout>
#include<QMainWindow>
#include<QStandardItem>
#include"codeeditor.h"
class MySplitter :public QObject
{
private:
    QSplitter * mainSplitter;
    QSplitter * rightSplitter;
    CodeEditor * edit;
    QTreeView *tree;
    QStandardItemModel* model;
    QGridLayout * layout_1;
public:
    MySplitter();

    ~MySplitter();

};

#endif







