#include "trap.h"

Trap::Trap()
{
}

Trap::Trap(int x, int y, int w, int h, int damage)
    : rect(x, y, w, h), damage(damage)
{
}

QRect Trap::getRect() const
{
    return rect;
}

void Trap::update()
{
    // 基类不移动
}

void Trap::draw(QPainter& painter) const
{
    // 基类默认绘制：红色尖刺样式
    painter.setPen(QPen(QColor("#8B0000"), 2));
    painter.setBrush(QColor("#DC143C"));

    // 绘制主体
    painter.drawRect(rect);

    // 绘制尖刺效果（三角形）
    int spikeCount = rect.width() / 10;
    for (int i = 0; i < spikeCount; ++i) {
        int x = rect.x() + i * 10 + 5;
        QPoint points[3] = {
            QPoint(x, rect.y()),
            QPoint(x - 4, rect.y() + 8),
            QPoint(x + 4, rect.y() + 8)
        };
        painter.setBrush(QColor("#FF4444"));
        painter.drawPolygon(points, 3);
    }
}

bool Trap::collidesWith(const QRect& target) const
{
    return rect.intersects(target);
}
