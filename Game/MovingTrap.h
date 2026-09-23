#ifndef MOVINGTRAP_H
#define MOVINGTRAP_H

#include "trap.h"

// 可移动陷阱：沿路径来回移动
class MovingTrap : public Trap
{
public:
    MovingTrap();

    // 创建可移动陷阱
    // x,y: 起点  w,h: 尺寸  damage: 伤害
    // endX,endY: 终点  speed: 移动速度
    MovingTrap(int x, int y, int w, int h, int damage,
               int endX, int endY, qreal speed);

    // 更新位置（来回移动）
    void update() override;

    // 绘制（带方向指示）
    void draw(QPainter& painter) const override;

    // 获取当前矩形
    QRect getRect() const override;

private:
    QPoint startPos;        // 起点
    QPoint endPos;          // 终点
    qreal speed = 2;        // 移动速度
    qreal progress = 0;     // 移动进度 0~1
    qreal direction = 1;    // 移动方向 1或-1
};

#endif // MOVINGTRAP_H
