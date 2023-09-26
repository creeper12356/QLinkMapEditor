#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QMap>
#include <QPoint>
#include <QTimer>
class Box;
class MainWindow;
//展示地图的部件
class MapWidget : public QWidget
{
    Q_OBJECT
private:
    int wScale = -1;
    int hScale = -1;
    int** data = nullptr;

    MainWindow* mainWindow = nullptr;//主窗口
    QMap<int,QMap<int,Box*>> boxes;//存储坐标到箱子的映射
    QMap<int,QPixmap> images;//箱子id与图片的映射

    //工作状态
    bool isShiftPressed = false;
    QVector<QPoint> fillVector;//填充区间
    QVector<QPoint> copyVector;//复制范围的容器
    //test
    QTimer testTimer;
public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget();
private:
    //init funcs
    void initImages();
private:
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    void setIsShiftPressed(bool flag);
public slots:
    //setters
    void setWScale(int w);
    void setHScale(int h);
    void setMainWindow(MainWindow* mw);

public:
    //getters
    int getWScale() const;
    int getHScale() const;
    //可修改，不发信号
    int& dataAt(int r,int c);
    int& dataAt(const QPoint& pt);
    //修改且发信号
    int& getDataAt(int r,int c);
    int& getDataAt(const QPoint& pt);
    //不提供修改，不发信号
    const int &getConstDataAt(int r,int c) const;
    const int &getConstDataAt(const QPoint& pt) const;
    //返回id对应的图片，如果id无效，返回无效图片（-2）
    const QPixmap imageAt(int id) const;
    MainWindow* getMainWindow(){return mainWindow;}
public:
    void deleteData(int hScale);
    void deleteData();
    void newData();

    void deleteBoxes();
    void newBoxes();

    void clearData();//清空所有方块
    void fill(int box,const QPoint& p1,const QPoint& p2);
private:
//    void randomSwapOnce();//随机交换方块
signals:
    void dataChanged(int r,int c);//位于r行c列的方块被更改
private slots:
    //方块被选中的槽
    void boxClickedSlot(QMouseEvent* event);
    void boxHoverSlot(QMouseEvent* event);
    void dataChangedSlot(int r,int c);
    void statePrinter();
};

#endif // MAPWIDGET_H
