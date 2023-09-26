#include "mapwidget.h"
#include "box.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QRect>
#include <QPainter>
#include <QPoint>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QKeyEvent>
#include <QWheelEvent>

MapWidget::MapWidget(QWidget *parent) : QWidget(parent)
{
    initImages();
    //.test
    connect(this,&MapWidget::dataChanged,this,&MapWidget::dataChangedSlot);
//    connect(&testTimer,&QTimer::timeout,this,&MapWidget::statePrinter);
//    testTimer.start(10);
}

MapWidget::~MapWidget()
{
    deleteData();
    deleteBoxes();
}

void MapWidget::initImages()
{
    images.insert(-1,QPixmap());
    images.insert(-2,QPixmap(":/images/barrier.png"));
    //initBoxList
    QFile reader;
    reader.setFileName(QDir::currentPath() + "/boxlist.json");
    reader.open(QIODevice::ReadOnly);
    QJsonArray jArray = QJsonDocument::fromJson(reader.readAll()).array();
    reader.close();
    int jSize = jArray.size();
    for(int i = 0;i <= jSize;++i)
    {
        images.insert(i,QPixmap(QDir::currentPath() + "/images/" + jArray.at(i).toString() + ".png"));
    }
}

const QPixmap MapWidget::imageAt(int id) const
{
    if(images.contains(id))
    {
        return images[id];
    }
    else
    {
        return images[-2];
    }
}

void MapWidget::paintEvent(QPaintEvent *event)
{
    if(wScale == -1 || hScale == -1)
    {
        return ;
    }
    if(data == nullptr)
    {
        return ;
    }
//    qDebug() << "paintEvent.";
    qDebug() << "shift:" << isShiftPressed;
    QPainter painter(this);

    QRect paintRect = this->rect();
    paintRect.setSize(paintRect.size() * 0.9);
    paintRect.moveCenter(rect().center());

    QPoint corner;
    int boxSize;
    if(paintRect.width() / wScale >= paintRect.height() / hScale)
    {
        boxSize = paintRect.height() / hScale;
        corner.setY(paintRect.top());
        corner.setX(paintRect.center().x()
                    - wScale * boxSize / 2);
    }
    else
    {
        boxSize = paintRect.width() / wScale;
        corner.setX(paintRect.left());
        corner.setY(paintRect.center().y()
                    - hScale * boxSize / 2);
    }

    //draw boxMap
    painter.setPen(QColor(150,150,150,180));
    painter.setBrush(Qt::NoBrush);
    for(int i = 0;i <= hScale - 1;++i)
    {
        for(int j = 0;j <= wScale - 1;++j)
        {
            QRect boxRect(corner.x() + j * boxSize,
                          corner.y() + i * boxSize,
                          boxSize,
                          boxSize);
            painter.drawRect(boxRect);
            boxes[i][j]->setGeometry(boxRect);
            painter.drawPixmap(boxRect,imageAt(data[i][j]));
        }
    }
}

void MapWidget::mousePressEvent(QMouseEvent *event)
{
    this->setFocus();
}

void MapWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat()){//ignore autorepeat
        return ;
    }
    int key = event->key();
    setIsShiftPressed(event->modifiers() & Qt::ShiftModifier);
    if(key == Qt::Key_F11)
    {
        qDebug() << "mainWindow" << mainWindow;
        if(mainWindow->isFullScreen())
        {
            mainWindow->showNormal();
        }
        else
        {
            mainWindow->showFullScreen();
        }
    }
    else if(isdigit(key))
    {
        qDebug() << "mainWindow: " << mainWindow;
        if(key - '0' < mainWindow->getUi()->boxList->count() - 1)
        {
            mainWindow->getUi()->boxList->setCurrentRow(key - '0');
        }
    }
}

void MapWidget::keyReleaseEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat()){//ignore autorepeat
        return ;
    }
    int key = event->key();
    setIsShiftPressed(event->modifiers() & Qt::ShiftModifier);
}

void MapWidget::wheelEvent(QWheelEvent *event)
{
    int currentRow = mainWindow->getUi()->boxList->currentRow();
    int count = mainWindow->getUi()->boxList->count();
    if(currentRow == -1)
    {
        mainWindow->getUi()->boxList->setCurrentRow(0);
        return ;
    }
    if(event->angleDelta().y() > 0)
    {
        mainWindow->getUi()->boxList->setCurrentRow((currentRow + count - 1) % count);
    }
    else
    {
        mainWindow->getUi()->boxList->setCurrentRow((currentRow + 1) % count);
    }
}

void MapWidget::setIsShiftPressed(bool flag)
{
    isShiftPressed = flag;
    if(!flag){
        fillVector.clear();
    }
}

void MapWidget::setWScale(int w)
{
    qDebug() << "set wScale to " << w << ".";
    wScale = w;
}

void MapWidget::setHScale(int h)
{
    qDebug() << "set hScale to " << h << ".";
    hScale = h;
}

void MapWidget::setMainWindow(MainWindow *mw)
{
    mainWindow = mw;
}

int MapWidget::getWScale() const
{
    return wScale;
}

int MapWidget::getHScale() const
{
    return hScale;
}

int &MapWidget::dataAt(int r, int c)
{
    return data[r][c];
}

int &MapWidget::dataAt(const QPoint &pt)
{
    return data[pt.x()][pt.y()];
}


int &MapWidget::getDataAt(int r, int c)
{
    emit dataChanged(r,c);
    return data[r][c];
}

int &MapWidget::getDataAt(const QPoint &pt)
{
    emit dataChanged(pt.x(),pt.y());
    return data[pt.x()][pt.y()];
}

const int& MapWidget::getConstDataAt(int r, int c) const
{
    return data[r][c];
}

const int& MapWidget::getConstDataAt(const QPoint &pt) const
{
    return data[pt.x()][pt.y()];
}

void MapWidget::deleteData(int hScale)
{
    if(data != nullptr)
    {
        qDebug() << "MapWidget::deleteData.";
        for(int i = 0;i <= hScale - 1;++i)
        {
            delete data[i];
        }
        delete data;
        this->wScale = -1;
        this->hScale = -1;
        data = nullptr;
    }
}

void MapWidget::deleteData()
{
    deleteData(hScale);
}

void MapWidget::newData()
{
    if(wScale == -1 || hScale == -1)
    {
        return ;
    }
    qDebug() << "MapWidget::newData.";
    data = new int*[hScale];
    for(int i = 0;i <= hScale - 1;++i)
    {
        data[i] = new int[wScale];
        for(int j = 0;j <= wScale - 1;++j)
        {
            //init data to -1
            data[i][j] = -1;
        }
    }
}

void MapWidget::deleteBoxes()
{
    if(boxes.empty())
    {
        return ;
    }

    qDebug() << "MapWidget::deleteBoxes.";
    for(auto& boxLine:boxes)
    {
        for(auto& box:boxLine)
        {
            delete box;
        }
    }
    boxes.clear();
}

void MapWidget::newBoxes()
{
    if(wScale == -1 || hScale == -1)
    {
        return ;
    }
    qDebug() << "MapWidget::newBoxes.";
    Box* handle;
    for(int i = 0;i <= hScale - 1;++i)
    {
        boxes.insert(i,QMap<int,Box*>());
        for(int j = 0;j <= wScale - 1;++j)
        {
            handle = new Box(this);
            handle->setLCpos(QPoint(i,j));
            connect(handle,&Box::clicked,this,&MapWidget::boxClickedSlot);
            connect(handle,&Box::hover,this,&MapWidget::boxHoverSlot);
            handle->show();
            boxes[i].insert(j,handle);
        }
    }
}

void MapWidget::clearData()
{
    if(data == nullptr)
    {
        return ;
    }
    for(int i = 0;i <= hScale - 1;++i)
    {
        for(int j = 0;j <= wScale - 1;++j)
        {
            getDataAt(i,j) = -1;
        }
    }
    update();
}

void MapWidget::fill(int box, const QPoint &p1, const QPoint &p2)
{
    int minR = qMin(p1.x(),p2.x()),
        maxR = qMax(p1.x(),p2.x()),
        minC = qMin(p1.y(),p2.y()),
        maxC = qMax(p1.y(),p2.y());
    for(int i = minR;i <= maxR;++i){
        for(int j = minC;j <= maxC;++j){
            getDataAt(i,j) = box;
        }
    }
}

void MapWidget::boxClickedSlot(QMouseEvent *event)
{
    this->setFocus();
    Box* box = dynamic_cast<Box*>(sender());
    QListWidget* boxList = getMainWindow()->getUi()->boxList;

    if(event->button() == Qt::LeftButton)
    {
        if(isShiftPressed){
            if(fillVector.size() == 0){
                fillVector.push_back(box->getLCpos());
            }
            else{//to fill a rect between two points
                qDebug() << "fill air.";
                fill(-1,fillVector[0],box->getLCpos());
                fillVector.clear();
                update();
                return ;
            }
        }
        if(getConstDataAt(box->getLCpos()) != -1){
            getDataAt(box->getLCpos()) = -1;
        }
    }
    else if(event->button() == Qt::RightButton)
    {
        if(boxList->selectedItems().empty()){
            return ;
        }
        if(isShiftPressed){
            if(fillVector.size() == 0){
                fillVector.push_back(box->getLCpos());
            }
            else{//to fill a rect between two points
                qDebug() << "fill.";
                fill(boxList->currentRow(),fillVector[0],box->getLCpos());
                fillVector.clear();
                update();
                return ;
            }
        }
        getDataAt(box->getLCpos()) = boxList->currentRow();
    }
    else if(event->button() == Qt::MidButton)
    {
        boxList->setCurrentRow(data[box->getLCpos().x()][box->getLCpos().y()]);
    }
    this->update();
}

void MapWidget::boxHoverSlot(QMouseEvent *event)
{
    QPoint LCpos = dynamic_cast<Box*>(sender())->getLCpos();
    QString msg = "(" + QString::number(LCpos.x()) + "," + QString::number(LCpos.y()) + ")";
    mainWindow->getUi()->statusbar->showMessage(msg);
}

void MapWidget::dataChangedSlot(int r, int c)
{
    qDebug() << "data changed at " << r << "," << c;
    if(mainWindow->getFileState() == open_save)
    {
        mainWindow->setFileState(open_not_save);
    }
}

void MapWidget::statePrinter()
{
    qDebug() << isShiftPressed;
}

