#ifndef MYFUNCLISTWIDGET_H
#define MYFUNCLISTWIDGET_H

#include<QListWidget>
#include<QMenu>
#include<QAction>
#include<QDebug>
#include<QListWidgetItem>
class MyFuncListWidget:public QListWidget
{
    Q_OBJECT
public:
    MyFuncListWidget();

public slots:
protected slots:
    void showMenu(const QPoint &point);
    void actionSave();
    void actionUnload();
    void actionNew();
    void actionLoad();
signals:
    void save(QListWidgetItem *);
    void loadFunc();
    void newFunc();
    void runFunc();
    void unloadFunc();
public:
   QMenu *m_menu;
   QMenu *p_menu;

   QAction *m_actionOne;
   QAction *m_actionTwo;

   QAction *p_actionOne;
   QAction *p_actionTwo;
   QAction *p_actionThree;


   QListWidgetItem *myItem;

};



#endif // MYFUNCLISTWIDGET_H
