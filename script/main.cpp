#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>  //添加头文件

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));//设置编码
    MainWindow w;
    w.show();
    return a.exec();
}
