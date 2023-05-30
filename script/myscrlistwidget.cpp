#include "myscrlistwidget.h"

MyScrListWidget::MyScrListWidget()
{
        this->setContextMenuPolicy( Qt::CustomContextMenu);
        QListWidgetItem *item;

        m_menu = new QMenu(this);
        p_menu = new QMenu(this);

        m_actionOne = new QAction(this);
        p_actionOne = new QAction (this);
        p_actionTwo = new QAction (this);

        m_actionOne->setText("保存");
        p_actionOne->setText("新建脚本");
        p_actionTwo->setText("载入脚本");

        m_menu->addAction(m_actionOne);

        p_menu->addAction(p_actionOne);
        p_menu->addAction(p_actionTwo);
//设置menu和action
        connect(m_actionOne, SIGNAL(triggered()), this, SLOT(actionSave()));
        connect(p_actionOne,SIGNAL(triggered()),this,SLOT(actionNew()));
        connect(p_actionTwo,SIGNAL(triggered()),this,SLOT(actionLoad()));
//关联action和槽
        connect(this, SIGNAL(customContextMenuRequested(const QPoint&))
            , this, SLOT(showMenu(const QPoint&)));//关联右键菜单
//关联menu的触发器和槽
}

void MyScrListWidget::showMenu(const QPoint& point)
{
    if(this->itemAt(point)==NULL)
    {
        p_menu->exec(mapToGlobal(point));
    }
    //第一种方法，使用保存的item指针获取对象内存
    else
    {
        myItem =NULL;//初始化,使每次通过指针获取的都是最新一次调用menu得到的对象
        m_menu->exec(mapToGlobal(point));
        myItem = this->itemAt(point);
            //通过itemAt方法获取点击的对象
        //qDebug()<<myItem->text();//可以正常输出，说明对象指针得到了
    }

}

void MyScrListWidget::actionSave()
{
    qDebug() << "actionSave"<<this->currentItem()->text();//第二种方法，通过currentItem方法获取选中的对象
    //，也可以正常输出，说明这种方法也可以获取对象内存指针
    emit save(this->currentItem());
}

void MyScrListWidget::actionNew()
{
    qDebug() << "actionNew";
    emit newScr();

}
void MyScrListWidget::actionLoad()
{
    qDebug() << "actionNew";
    emit loadScr();
}
