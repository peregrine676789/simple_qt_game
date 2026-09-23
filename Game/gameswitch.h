#ifndef GAMESWITCH_H
#define GAMESWITCH_H
#include<QRect>
#include<QGraphicsRectItem>


class gameSwitch
{
public:
    gameSwitch();


    gameSwitch(int x, int y, int w, int h, QColor closed, QColor open)
        : baseRect(x, y, w, h), closedColor(closed), openColor(open) {}

    QRect baseRect;         // 基准位置（用于缩放）
    bool isOpen = false;    // 是否已打开
    QColor closedColor;     // 关闭状态颜色
    QColor openColor;       // 打开状态颜色

    // 获取当前颜色
    QColor getColor() const {
        return isOpen ? openColor : closedColor;
    }


};


// ========== 开关门类 ==========
struct SwitchDoor {
    QRect baseRect;         // 基准位置
    bool isOpen = false;    // 是否开启（可通过）
    QColor closedColor;     // 关闭时颜色（墙体）

    SwitchDoor() {}
    SwitchDoor(int x, int y, int w, int h, QColor color)
        : baseRect(x, y, w, h), closedColor(color) {}

};

#endif // GAMESWITCH_H
