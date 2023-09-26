#ifndef BOX_H
#define BOX_H

#include <QObject>
#include <QLabel>
#include <QMouseEvent>

class Box :public QLabel
{
    Q_OBJECT
private:
    QPoint LCpos;//行列坐标
public:
    explicit Box(QWidget *parent = nullptr);
public:
    //setters
    void setLCpos(const QPoint& p);
    //getters
    const QPoint& getLCpos() const;
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
signals:
    void clicked(QMouseEvent* event);//箱子被按下信号
    void hover(QMouseEvent* event);//光标悬浮

};

#endif // BOX_H
