#include "mainwindow.h"
#include "ui_mainwindow.h"
//.s
//.func

//保存单个文件 QfileChangedSign 的bool 是否为true ,QfilesContentChanged -> QfilesContent和磁盘文件，标记设置为false,QfilesContentChanged的元素erase
//保存所有文件 遍历QfileChangedSign 依此保存单个文件

//继承QListWidget 重载鼠标点击事件

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    this->setMinimumSize(800,800);

    buffer = std::cout.rdbuf();
    initSplitter();//分割界面

    //左半侧树
    funcListWidget = new MyFuncListWidget();
    funcListWidget->setWindowTitle("function");
    scrListWidget = new MyScrListWidget();
    scrListWidget->setWindowTitle("scriprt");

    funcLab = new QLabel("函数文件");
    scriptLab = new QLabel("脚本文件");

    leftSplitter->addWidget(funcLab);
    leftSplitter->addWidget(funcListWidget);
    leftSplitter->addWidget(scriptLab);
    leftSplitter->addWidget(scrListWidget);

    //右半侧 编辑 结果（tab选项卡）

    tabWidget = new QTabWidget();
    //输出界面
    browser1 =new QTextBrowser(this) ;
    browser2 =new QTextBrowser(this) ;
    QString str ="数据";

    tabWidget->addTab(browser1,"输出结果");
    tabWidget->setCurrentIndex(0);
    tabWidget->addTab(browser2,"错误提示");
    textEdit = new CodeEditor(this);
    rightSplitter->addWidget(textEdit);
    rightSplitter->addWidget(tabWidget);

    setSplitter();

    setCentralWidget(mainSplitter);

    //创建菜单项
    fileMenu = new QMenu(this);
    runMenu = new QMenu(this);
    scrLoad = new QAction(tr("打开脚本"),this);
    funcLoad = new QAction(tr("打开函数"),this);
    funcNewact = new QAction(tr("新建函数"),this);
    scrNewact = new QAction(tr("新建脚本"),this);
    fileSaveact = new QAction(tr("保存"),this);
    runScript = new QAction(tr("在当前函数环境下运行脚本"),this);
    //添加菜单
    fileMenu = menuBar()->addMenu(tr("文件"));
    runMenu = menuBar()->addMenu(tr("运行"));
    //添加菜单项

    fileMenu->addSeparator();
    fileMenu->addAction(funcLoad);
    fileMenu->addSeparator();
    fileMenu->addAction(scrLoad);
    fileMenu->addSeparator();
    fileMenu->addAction(funcNewact);
    fileMenu->addSeparator();
    fileMenu->addAction(scrNewact);
    fileMenu->addSeparator();
    fileMenu->addAction(fileSaveact);

    runMenu->addSeparator();
    runMenu->addAction(runScript);

    funcListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    scrListWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(funcListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onFunctionListItemClicked(QListWidgetItem*)));
    connect(scrListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onScriptListItemClicked(QListWidgetItem*)));

    //右键界面
    //卸载
    connect(funcListWidget, SIGNAL(unloadFunc ()), this, SLOT(unload_Func()));

    connect(scrListWidget,SIGNAL(save(QListWidgetItem *)),this,SLOT(Save_File(QListWidgetItem *)) );
    connect(funcListWidget,SIGNAL(save(QListWidgetItem*)),this,SLOT(Save_File(QListWidgetItem *)) );

    connect(scrListWidget,SIGNAL(loadScr()),this,SLOT(Open_Scr()) );
    connect(funcListWidget,SIGNAL(loadFunc()),this,SLOT(Open_Func()) );
    connect(scrListWidget,SIGNAL(newScr()),this,SLOT(Create_NewScr()) );
    connect(funcListWidget,SIGNAL(newFunc()),this,SLOT(Create_NewFunc()) );
    connect(funcListWidget,SIGNAL(runFunc()),this,SLOT(Run_Script()) );

    //添加触发函数
    connect(scrLoad,SIGNAL(triggered()),this,SLOT(Open_Scr()) );
    connect(funcLoad,SIGNAL(triggered()),this,SLOT(Open_Func()) );
    connect(funcNewact,SIGNAL(triggered()),this,SLOT(Create_NewFunc()) );
    connect(scrNewact,SIGNAL(triggered()),this,SLOT(Create_NewScr()) );
    connect(fileSaveact,SIGNAL(triggered()),this,SLOT(Save_AllFile()));
    connect(runScript,SIGNAL(triggered()),this,SLOT(Run_Script()) );


}

MainWindow::~MainWindow()
{
    delete ui;
}


QString MainWindow::readFile(QString filePath){
    QFile aFile(filePath);
    if(!aFile.exists())//如果文件不存在
        return "";
    if(!aFile.open(QIODevice::ReadOnly |QIODevice::Text))//如果不是以只读和文本方式打开文件返回false
        return "";
    QTextStream aStream(&aFile);//文本流读取文件
    aStream.setCodec("GBK");
    aStream.setAutoDetectUnicode(true);//自动检测unicode便于显示汉字
    return aStream.readAll();

}
bool MainWindow::saveToFile(QString filePath,QString data){
    QFile myfile(filePath);
    myfile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream out(&myfile);
    out.setCodec("GBK");
    out << data;
    myfile.close();
    return true;
}


QString MainWindow::readCurrentEditor(){
    return this->textEdit->toPlainText();
}

void MainWindow::clearAndWriteToEditor(QString data){
    textEdit->clear();
    textEdit->highlightCurrentLine();
    textEdit->appendPlainText(data);
}



void MainWindow::Open_Scr()
{

    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,tr("Open File"),tr(""),tr("Text File (*.s)"));
    if(fileName == "")                  //如果用户直接关闭了文件浏览对话框，那么文件名就为空值，直接返回
    {
        return;
    }
    else
    {
        currentSelectedFilePath = fileName;
        QString QfileContent = readFile(fileName);

        QfilesContentChanged[fileName] = QfileContent;

        QListWidgetItem *item =new QListWidgetItem(fileName);
        this->clearAndWriteToEditor(QfileContent);

        scrListWidget->addItem(item);
        for(int i=0;i<this->scrListWidget->count();i++){
            this->scrListWidget->item(i)->setForeground(QColor(0,0,0));
            this->scrListWidget->item(i)->setBackground(QColor(255,255,255));
        }
        item->setBackground(QColor(100,100,100));
        item->setForeground(QColor(255,255,255));
        this->selectedScriptPath = item->text();
    }
}


void MainWindow::Open_Func()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,tr("Open File"),tr(""),tr("Text File (*.func)"));
    if(fileName == "")
     {
        return;
    }
    else
    {
        currentSelectedFilePath = fileName;
        QString QfileContent = this->readFile(fileName);

        QfilesContentChanged[fileName] = QfileContent;

        QListWidgetItem *item =new QListWidgetItem(fileName);
        this->clearAndWriteToEditor(QfileContent);
        funcListWidget->addItem(item);

        for(int i=0;i<this->funcListWidget->count();i++){
            this->funcListWidget->item(i)->setForeground(QColor(0,0,0));
            this->funcListWidget->item(i)->setBackground(QColor(255,255,255));
        }
        item->setBackground(QColor(100,100,100));
        item->setForeground(QColor(255,255,255));
        this->selectedFunctionPath = fileName;

        try {
            //载入函数
            interpreter.loadFunctionFile(fileName.toStdString());
        } catch (std::string e) {
            browser2->append(QString::fromStdString(e));
        }
        catch(Error e)
        {
            QString error =QString::number(e.pc)+"行："+QString::fromStdString(e.errorMsg);
            browser2->append(error);
        }
    }

}

void MainWindow::Create_NewFunc()   //创建新文件
{
    QFileDialog fileDialog;
    QString filePath = fileDialog.getSaveFileName(this,tr("creat File"),"/home",tr("Text File(*.func)"));
    if(filePath == ""){
        return;
    }

    currentSelectedFilePath = filePath;

    QListWidgetItem * listWidgetItem =new QListWidgetItem(filePath,funcListWidget);
    this->clearAndWriteToEditor("");

    //显示文本框
    QFile qFile(filePath);
    qFile.open(QIODevice::ReadWrite | QIODevice::Truncate);
    qFile.close();

    QfilesContentChanged[filePath] = "";

    for(int i=0;i<this->funcListWidget->count();i++){
        this->funcListWidget->item(i)->setForeground(QColor(0,0,0));
        this->funcListWidget->item(i)->setBackground(QColor(255,255,255));
    }
    listWidgetItem->setBackground(QColor(100,100,100));
    listWidgetItem->setForeground(QColor(255,255,255));


    this->selectedFunctionPath = listWidgetItem->text();

}
void MainWindow::Create_NewScr()   //创建新文件
{
    QFileDialog fileDialog;
    QString filePath = fileDialog.getSaveFileName(this,tr("creat File"),"/home",tr("Text File(*.s)"));
    if(filePath == ""){
        return;
    }

    currentSelectedFilePath = filePath;

    QListWidgetItem * listWidgetItem =new QListWidgetItem(filePath,scrListWidget);
    this->clearAndWriteToEditor("");

    //显示文本框

    QString QfileContent;
    QFile qFile(filePath);
    qFile.open(QIODevice::ReadWrite | QIODevice::Truncate);
    qFile.close();

    QfilesContentChanged[filePath] = "";


    for(int i=0;i<this->scrListWidget->count();i++){
        this->scrListWidget->item(i)->setForeground(QColor(0,0,0));
        this->scrListWidget->item(i)->setBackground(QColor(255,255,255));
    }
    listWidgetItem->setBackground(QColor(100,100,100));
    listWidgetItem->setForeground(QColor(255,255,255));

    this->selectedScriptPath = listWidgetItem->text();
}

void MainWindow::Save_AllFile()
{
    for(int i=0;i<scrListWidget->count();i++)
    {
        QString fileName=scrListWidget->item(i)->text();
        this->saveToFile(fileName,QfilesContentChanged[fileName]);
//        QfileChangedSign[fileName]=false;
    }
    for(int i=0;i<funcListWidget->count();i++)
    {
        QString fileName=funcListWidget->item(i)->text();
        this->saveToFile(fileName,QfilesContentChanged[fileName]);
//        QfileChangedSign[fileName]=false;
    }
}

//TODO 有问题
void MainWindow::Save_File(QListWidgetItem *item)
{
    if(QfilesContentChanged[currentSelectedFilePath] != this->readFile(currentSelectedFilePath))
    {

        QString fileName=item->text();
        this->saveToFile(fileName,QfilesContentChanged[fileName]);
        QMessageBox::warning(this,tr("提示"),tr("保存文件成功"));
    }
    else
    {
       return;
    }
}


void MainWindow:: Run_Script()
{
    qDebug()<<"run";
    //先保存
    if(selectedScriptPath==NULL)
    {
        return;
    }
    else
    {
    QString fileName=this->selectedScriptPath;
    this->saveToFile(fileName,QfilesContentChanged[selectedScriptPath]);

    std::stringstream ss;
    try{

    std::cout.rdbuf(ss.rdbuf());
    DLexer lexer;
    lexer.inputFile(fileName.toStdString());
    //重定向
    browser2->clear();
    browser1->clear();
    interpreter.inputScript(lexer.getTokenStream(fileName.toStdString()));
    browser1->append(QString::fromStdString(ss.str()));

    std::cout.rdbuf(buffer);
    std::cout<<std::endl;

    //结束重定向

    std::cout<<std::endl;
    qDebug()<<"jieshu";
    tabWidget->setCurrentIndex(0);

 }
    catch(std::string e)
    {
        tabWidget->setCurrentIndex(1);

        browser2->append(QString::fromStdString(e));
        QMessageBox::critical(this, tr("出错了"), tr("请检查错误提示输出窗口并修改脚本！"));
    }
    catch(Error e)
    {
        tabWidget->setCurrentIndex(1);

        QString error =QString::number(e.pc)+"行："+QString::fromStdString(e.errorMsg);
        browser2->append(error);
        QMessageBox::critical(this, tr("出错了"), tr("请检查错误提示输出窗口并修改脚本！"));

    }
    }
}
void MainWindow::closeEvent(QCloseEvent *event)
{

    QMessageBox::StandardButton button;
        button=QMessageBox::question(this,tr("退出程序"),QString(tr("是否保存并退出")),QMessageBox::Yes|QMessageBox::No);
        if(button==QMessageBox::No)
        {
            event->ignore(); // 忽略退出信号，程序继续进行
        }
        else if(button==QMessageBox::Yes)
        {
            for(int i=0;i<scrListWidget->count();i++)
            {
                QString fileName=scrListWidget->item(i)->text();
                QFile file(fileName);
                file.open(QIODevice::WriteOnly | QIODevice::Truncate);
                QTextStream out(&file);
                QString fileContent = QfilesContentChanged[fileName];
                out<<fileContent;
                file.close();
//                QfileChangedSign[fileName]=false;
            }
            for(int i=0;i<funcListWidget->count();i++)
            {
                QString fileName=funcListWidget->item(i)->text();
                QFile file(fileName);
                file.open(QIODevice::WriteOnly | QIODevice::Truncate);
                QTextStream out(&file);
                QString fileContent = QfilesContentChanged[fileName];
                out<<fileContent;
                file.close();
//                QfileChangedSign[fileName]=false;
            }
            event->accept(); // 接受退出信号，程序退出
        }

}
//onScriptListItemClicked
void MainWindow::onScriptListItemClicked(QListWidgetItem* item){
    //点击事件  保存editor的内容到QfilesContentChanged，设置文件改变标记为true，再打开新文件
    for(int i=0;i<this->scrListWidget->count();i++){
        this->scrListWidget->item(i)->setForeground(QColor(0,0,0));
        this->scrListWidget->item(i)->setBackground(QColor(255,255,255));
    }
    item->setBackground(QColor(100,100,100));
    item->setForeground(QColor(255,255,255));

    QString str;
    if(currentSelectedFilePath != item->text()){
        //保存文件
        QFile file(currentSelectedFilePath);
        qDebug()<<currentSelectedFilePath;
        qDebug()<<"save";
        this->saveToFile(currentSelectedFilePath,this->readCurrentEditor());
    }


    QString currentContent = this->readCurrentEditor();
    QfilesContentChanged[currentSelectedFilePath]=currentContent;
//    QfileChangedSign[currentSelectedFilePath] =true;

    //打开新文件
    QString fileName = item->text();
    if(!QfilesContentChanged.count(fileName))
    {
       //填充错误信息
        qDebug()<<"error";
        return;
    }

    QString fileContent = QfilesContentChanged[fileName];
    this->clearAndWriteToEditor(fileContent);

    currentSelectedFilePath = item->text();
    selectedScriptPath = item->text();

}



void MainWindow:: onFunctionListItemClicked(QListWidgetItem* item)
{
    //点击事件  保存editor的内容到QfilesContentChanged，设置文件改变标记为true，再打开新文件
    //

    for(int i=0;i<this->funcListWidget->count();i++){
        this->funcListWidget->item(i)->setForeground(QColor(0,0,0));
        this->funcListWidget->item(i)->setBackground(QColor(255,255,255));
        qDebug("有效");
    }
    item->setBackground(QColor(100,100,100));
    item->setForeground(QColor(255,255,255));
    qDebug()<<"on clicked";

    if(currentSelectedFilePath != item->text()){
        //保存文件
        QFile file(currentSelectedFilePath);
        qDebug()<<currentSelectedFilePath;
        qDebug()<<"save";
        this->saveToFile(currentSelectedFilePath,this->readCurrentEditor());
    }

    QString currentContent = this->readCurrentEditor();
    QfilesContentChanged[currentSelectedFilePath]=currentContent;
//    QfileChangedSign[currentSelectedFilePath] =true;

    //打开新文件

    QString fileName = item->text();
    if(!QfilesContentChanged.count(fileName))
    {
       //填充错误信息
        qDebug()<<"error";
        return;
    }

   QString fileContent = QfilesContentChanged[fileName];
   this->clearAndWriteToEditor(fileContent);

   currentSelectedFilePath = item->text();
   selectedFunctionPath = item->text();


}
void MainWindow::initSplitter()
{
    funcListWidget = new MyFuncListWidget;//
    scrListWidget = new MyScrListWidget;
    mainSplitter =new QSplitter (Qt::Horizontal);

    rightSplitter =new QSplitter (Qt::Vertical);
    leftSplitter = new QSplitter(Qt::Vertical);
}
void MainWindow::setSplitter()
{
    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(rightSplitter);
    mainSplitter->setStretchFactor(0,2);
    mainSplitter->setStretchFactor(1,8);
    leftSplitter->setStretchFactor(0,1);
    leftSplitter->setStretchFactor(1,2);
    rightSplitter->setStretchFactor(0,8);
    rightSplitter->setStretchFactor(1,2);
}

void MainWindow::saveChangedContent(QString fileName)
{

}

void  MainWindow::unload_Func()
{

    QListWidgetItem *item = funcListWidget->currentItem();
  QString fileName;
       if(item!=NULL)
       {
          fileName=funcListWidget->currentItem()->text();
           //funcListWidget->removeItemWidget(item);
           delete item;
       }
    qDebug()<<"jn";

        try {
                interpreter.unloadFunctionFile(fileName.toStdString());
            }
            catch(std::string e)
            {
                browser2->append(QString::fromStdString(e));
            }
            catch(Error e)
            {
                QString error =QString::number(e.pc)+"行："+QString::fromStdString(e.errorMsg);
                browser2->append(error);
            }
}
