#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<string>
#include<sstream>
#include<iostream>
#include<fstream>
#include<QDebug>
#include<QFileInfoList>
#include<QFileInfo>
#include<QListWidgetItem>
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include<QPlainTextEdit>
#include<codeeditor.h>
#include<QSplitter>
#include<QTextBrowser>
#include<QListWidget>
#include<QLabel>
#include <QFileDevice>
#include <QTextStream>
#include <QtEvents>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include<QMap>
#include<QTextCodec>
#include"myscrlistwidget.h"
#include"myfunclistwidget.h"
#include"Interpreter/Constant.h"
#include"Interpreter/DLexer.h"
#include"Interpreter/Interpreter.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initSplitter();//分割
    void setSplitter();//设置分割比例
    void setMenu();
    CodeEditor *textEdit;//

private:
    Ui::MainWindow *ui;
    Interpreter interpreter;
    MyFuncListWidget *funcListWidget;
    MyScrListWidget *scrListWidget;

    QTextBrowser * browser1;
    QTextBrowser * browser2;

    QLabel *funcLab, *scriptLab;
    QTabWidget * tabWidget;

    //menu

    QMenu *fileMenu;
    QAction *scrLoad;
    QAction *funcLoad;
    QAction *funcNewact;
    QAction *scrNewact;
    QAction *fileSaveact;
    QAction *fileSaveasact;

    QMenu *runMenu;
    QAction *runScript;


    //splitter
    QSplitter * mainSplitter;
    QSplitter * rightSplitter;
    QSplitter * leftSplitter;

    QString currentSelectedFilePath;

    //current item

    QPoint delPos;

    std::streambuf *buffer;

    QString selectedFunctionPath,selectedScriptPath;


//    QMap<QString,QString> QfilesContent;
    QMap<QString,QString> QfilesContentChanged;
//    QMap<QString,bool>QfileChangedSign;


private:
    QString readFile(QString filePath);
    bool saveToFile(QString filePath,QString data);

    QString readCurrentEditor();
    void clearAndWriteToEditor(QString data);


public slots:

    void onFunctionListItemClicked(QListWidgetItem*);
    void onScriptListItemClicked(QListWidgetItem*);

    void Open_Scr();
    void Open_Func();
    void Create_NewFunc();
    void Create_NewScr();

    //需要重写
    void Save_File(QListWidgetItem *);
    void Save_AllFile();



    void Run_Script();
    void closeEvent(QCloseEvent *event);

    void unload_Func();


    void saveChangedContent(QString fileName);

};

#endif // MAINWINDOW_H
