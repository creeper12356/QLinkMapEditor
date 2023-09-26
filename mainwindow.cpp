#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setMainWindow(this);
    ui->centralwidget->setFocus();//默认焦点

    initBoxList();
    connect(this,&MainWindow::fileStateChanged,this,&MainWindow::fileStateChangedSlot);
    ui->boxList->setFocusPolicy(Qt::NoFocus);
}

MainWindow::~MainWindow()
{
    delete ui;
    int itemCount = ui->boxList->count();
    for(int i = 0;i <= itemCount - 1;++i)
    {
        delete ui->boxList->item(i);
    }
}

void MainWindow::setFileState(FileState fs)
{
    if(fState == fs)
    {
        return ;
    }
    emit fileStateChanged(fs);
    fState = fs;
    if(fs == not_open)
    {
        ui->actionSave->setEnabled(false);
        ui->actionClear->setEnabled(false);
    }
    else if(fs == open_not_save)
    {
        ui->actionSave->setEnabled(true);
        ui->actionClear->setEnabled(true);
    }
    else if(fs == open_save)
    {
        ui->actionSave->setEnabled(false);
        ui->actionClear->setEnabled(true);
    }
}

bool MainWindow::initMapSize()
{
    //basic settings
    QInputDialog dialog(this);
//    dialog.
    dialog.setInputMode(QInputDialog::IntInput);
    dialog.setIntRange(1,50);

    //input wscale
    connect(&dialog,
            &QInputDialog::intValueSelected,
            ui->centralwidget,
            &MapWidget::setWScale);
    dialog.setLabelText("输入宽度规模");
    dialog.exec();

    //disconnect
    disconnect(&dialog,
                &QInputDialog::intValueSelected,
                ui->centralwidget,
                &MapWidget::setWScale);

    //inputhscale
    connect(&dialog,
            &QInputDialog::intValueSelected,
            ui->centralwidget,
            &MapWidget::setHScale);
    dialog.setLabelText("输入高度规模");
    dialog.exec();
    if(ui->centralwidget->getWScale() == -1
            || ui->centralwidget->getHScale() == -1)
    {
        return false;
    }
    return true;
}

void MainWindow::initBoxList()
{
    QFile reader;
    reader.setFileName(QDir::currentPath() + "/boxlist.json");
    reader.open(QIODevice::ReadOnly);
    QJsonArray jArray = QJsonDocument::fromJson(reader.readAll()).array();
    reader.close();
    int jSize = jArray.size();
    QListWidgetItem* handle;
    for(int i = 0;i <= jSize - 1;++i)
    {
        handle = new QListWidgetItem;
        handle->setText(jArray.at(i).toString());
        handle->setIcon(ui->centralwidget->imageAt(i));
        ui->boxList->addItem(handle);
    }
}

void MainWindow::on_actionNew_triggered()
{
    ui->centralwidget->deleteData();
    ui->centralwidget->deleteBoxes();
    this->path = "";
    if(!initMapSize())
    {
        setFileState(not_open);
        return ;
    }
    setFileState(open_not_save);
    ui->centralwidget->newData();
    ui->centralwidget->newBoxes();
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

void MainWindow::on_actionOpen_triggered()
{
    ui->centralwidget->deleteData();
    ui->centralwidget->deleteBoxes();
    QString fileName = QFileDialog::getOpenFileName(this,"打开文件",QDir::homePath(),"map.json");
    if(fileName.isNull())
    {
        setFileState(not_open);
        return ;
    }
    QDir parentDir(fileName);
    parentDir.cdUp();

    QFile reader;
    //read basic settings
    reader.setFileName(parentDir.path() + "/basic.json");
    if(!reader.exists())
    {
        QMessageBox::warning(this,"警告","无法在当前目录找到basic.json文件。");
        setFileState(not_open);
        return ;
    }

    //打开成功
    setFileState(open_save);
    reader.open(QIODevice::ReadOnly);
    QJsonObject object = QJsonDocument::fromJson(reader.readAll()).object();
    reader.close();
    ui->centralwidget->setWScale(object.value("wScale").toInt());
    ui->centralwidget->setHScale(object.value("hScale").toInt());

    ui->centralwidget->newData();
    ui->centralwidget->newBoxes();

    //read map array

    reader.setFileName(fileName);
    reader.open(QIODevice::ReadOnly);
    QJsonArray jArray = QJsonDocument::fromJson(reader.readAll()).array();
    reader.close();
    int count = 0;
    for(int i = 0;i <= ui->centralwidget->getHScale() - 1;++i)
    {
        for(int j = 0;j <= ui->centralwidget->getWScale() - 1;++j)
        {
            ui->centralwidget->dataAt(i,j) = jArray.at(count).toInt();
            ++count;
        }
    }

    this->path = fileName;
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName;
    if(QFile::exists(this->path))
    {
        fileName = this->path;
    }
    else
    {
        fileName = QFileDialog::getSaveFileName(this,"保存文件",QDir::homePath(),"*.json");
        if(fileName.isNull())
        {
            return ;
        }
    }
    setFileState(open_save);
    this->path = fileName;
    QFile writer;
    writer.setFileName(fileName);
    writer.open(QIODevice::WriteOnly);
    QJsonArray jArray;
    for(int i = 0;i <= ui->centralwidget->getHScale() - 1;++i)
    {
        for(int j = 0;j <= ui->centralwidget->getWScale() - 1;++j)
        {
            jArray.append(ui->centralwidget->getConstDataAt(i,j));
        }
    }
    QJsonDocument docu(jArray);
    writer.write(docu.toJson());
    writer.close();
}

void MainWindow::fileStateChangedSlot(FileState state)
{
    qDebug() << "fileState changed to " << FILESTATE_TO_STR(state);
}
void MainWindow::on_actionClear_triggered()
{
    ui->centralwidget->clearData();
}
