#ifndef MYSCRLISTWIDGET_H
#define MYSCRLISTWIDGET_H

#include<QListWidget>
#include<QMenu>
#include<QAction>
#include<QDebug>
#include<QListWidgetItem>
#include<QFile>
class MyScrListWidget:public QListWidget
{
    Q_OBJECT
public:
    MyScrListWidget();

public slots:
protected slots:
    void showMenu(const QPoint &point);
    void actionSave();

    void actionNew();
    void actionLoad();
signals:
    void save(QListWidgetItem *);
    void loadScr();
    void newScr();

public:
   QMenu *m_menu;
   QMenu *p_menu;

   QAction *m_actionOne;

   QAction *p_actionOne;
   QAction *p_actionTwo;

   QListWidgetItem *myItem;
};



#endif // MYFUNCLISTWIDGET_H
