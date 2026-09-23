#ifndef TRAP_H
#define TRAP_H

#include <QRect>
#include <QPainter>
#include <QVector2D>

// 陷阱类型枚举
enum class TrapType {
    Static,     // 固定陷阱
    Moving      // 可移动陷阱
};

// 陷阱基类
class Trap
{
public:
    Trap();
    Trap(int x, int y, int w, int h, int damage);
    virtual ~Trap() = default;

    // 获取当前碰撞矩形
    virtual QRect getRect() const;

    // 更新陷阱状态（每帧调用）
    virtual void update();

    // 绘制陷阱
    virtual void draw(QPainter& painter) const;

    // 获取伤害值
    int getDamage() const { return damage; }

    // 检查是否与目标碰撞
    bool collidesWith(const QRect& target) const;

    TrapType getType() const { return type; }

protected:
    QRect rect;             // 当前位置
    int damage = 10;        // 伤害值
    TrapType type = TrapType::Static;
};

#endif // TRAP_H

