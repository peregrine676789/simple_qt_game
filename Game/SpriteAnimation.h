#ifndef SPRITEANIMATION_H
#define SPRITEANIMATION_H

#include <QPixmap>
#include <QVector>
#include <QString>


class SpriteAnimation
{
public:
    SpriteAnimation();

    // 加载精灵图并切割帧
    // filePath: 资源路径  frameWidth/frameHeight: 单帧尺寸  frameCount: 总帧数
    bool load(const QString& filePath, int frameWidth, int frameHeight, int frameCount);

    // 获取当前帧
    QPixmap getCurrentFrame() const;

    // 更新动画（每帧调用）
    void update();

    // 重置到第一帧
    void reset();

    // 是否播放完毕（非循环动画用）
    bool isFinished() const;

    // 设置播放速度（每N帧切换一帧）
    void setFrameDelay(int delay);

    // 设置是否循环
    void setLoop(bool loop);

    int getFrameCount() const { return frames.size(); }

private:
    QVector<QPixmap> frames;        // 切割后的帧数组
    int currentFrame = 0;           // 当前帧索引
    int frameDelay = 4;             // 帧延迟（每4游戏帧切换一帧）
    int frameTimer = 0;             // 帧计时器
    bool loop = true;               // 是否循环
    bool finished = false;            // 是否播放完毕
};

#endif // SPRITEANIMATION_H
