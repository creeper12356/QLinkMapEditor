#include "box.h"
#include "mapwidget.h"
#include <QDebug>
Box::Box(QWidget *parent):QLabel(parent)
{
    setMouseTracking(true);
}

void Box::setLCpos(const QPoint &p)
{
    LCpos = p;
}

const QPoint &Box::getLCpos() const
{
    return LCpos;
}

void Box::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "You pressed box at " << this->getLCpos();
    emit clicked(event);
}

void Box::mouseMoveEvent(QMouseEvent *event)
{
    emit hover(event);
}


