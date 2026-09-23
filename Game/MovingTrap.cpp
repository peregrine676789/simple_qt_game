#include "movingtrap.h"
#include <QtMath>

MovingTrap::MovingTrap()
{
    type = TrapType::Moving;
}

MovingTrap::MovingTrap(int x, int y, int w, int h, int damage,
                       int endX, int endY, qreal speed)
    : Trap(x, y, w, h, damage), startPos(x, y), endPos(endX, endY), speed(speed)
{
    type = TrapType::Moving;
}

QRect MovingTrap::getRect() const
{
    // 根据进度计算当前位置
    int currentX = startPos.x() + (endPos.x() - startPos.x()) * progress;
    int currentY = startPos.y() + (endPos.y() - startPos.y()) * progress;
    return QRect(currentX, currentY, rect.width(), rect.height());
}

void MovingTrap::update()
{
    // 更新进度
    progress += (speed / 100.0) * direction;

    // 到达终点反向
    if (progress >= 1.0) {
        progress = 1.0;
        direction = -1;
    } else if (progress <= 0.0) {
        progress = 0.0;
        direction = 1;
    }

    // 更新rect用于碰撞检测
    rect = getRect();
}

void MovingTrap::draw(QPainter& painter) const
{
    QRect currentRect = getRect();



    // 绘制移动轨迹（半透明）
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 0, 0, 30));
    painter.drawLine(startPos, endPos);

    // // 绘制起点和终点标记
    // painter.setBrush(QColor(255, 0, 0, 60));
    // painter.drawEllipse(startPos, 5, 5);
    // painter.drawEllipse(endPos, 5, 5);

    // 绘制陷阱本体（橙色，区别于固定陷阱）
    painter.setPen(QPen(QColor("#CC6600"), 2));
    painter.setBrush(QColor("#FF8800"));
    painter.drawRect(currentRect);

    // 绘制移动方向箭头
    int centerX = currentRect.center().x();
    int centerY = currentRect.center().y();
    int arrowSize = 6;

    QPoint arrowTip, arrowBase;
    if (qAbs(endPos.x() - startPos.x()) > qAbs(endPos.y() - startPos.y())) {
        // 水平移动
        arrowBase = QPoint(centerX, centerY);
        arrowTip = QPoint(centerX + (direction > 0 ? arrowSize : -arrowSize), centerY);
    } else {
        // 垂直移动
        arrowBase = QPoint(centerX, centerY);
        arrowTip = QPoint(centerX, centerY + (direction > 0 ? arrowSize : -arrowSize));
    }

    painter.setPen(QPen(Qt::white, 2));
    painter.drawLine(arrowBase, arrowTip);
}
