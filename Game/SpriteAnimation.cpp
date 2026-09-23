#include "SpriteAnimation.h"
#include <QDebug>

SpriteAnimation::SpriteAnimation() {}

bool SpriteAnimation::load(const QString& filePath, int frameWidth, int frameHeight, int frameCount)
{
    QPixmap spriteSheet(filePath);

    if (spriteSheet.isNull()) {
        qDebug() << "Failed to load sprite:" << filePath;
        return false;
    }

    frames.clear();

    // 从左到右切割帧
    for (int i = 0; i < frameCount; ++i) {
        int x = i * frameWidth;
        // 确保不超出图片宽度
        if (x + frameWidth <= spriteSheet.width()) {
            QRect frameRect(x, 0, frameWidth, frameHeight);
            frames.append(spriteSheet.copy(frameRect));
        } else {
            qDebug() << "Frame" << i << "out of bounds, sprite width:" << spriteSheet.width();
            break;
        }
    }

    qDebug() << "Loaded" << frames.size() << "frames from" << filePath
             << "size:" << spriteSheet.size() << "frame:" << frameWidth << "x" << frameHeight;
    return !frames.isEmpty();
}

QPixmap SpriteAnimation::getCurrentFrame() const
{
    if (frames.isEmpty()) return QPixmap();
    return frames[currentFrame];
}

void SpriteAnimation::update()
{
    if (frames.isEmpty() || finished) return;

    frameTimer++;
    if (frameTimer >= frameDelay) {
        frameTimer = 0;
        currentFrame++;

        if (currentFrame >= frames.size()) {
            if (loop) {
                currentFrame = 0;  // 循环播放
            } else {
                currentFrame = frames.size() - 1;  // 停在最后一帧
                finished = true;
            }
        }
    }
}

void SpriteAnimation::reset()
{
    currentFrame = 0;
    frameTimer = 0;
    finished = false;
}

bool SpriteAnimation::isFinished() const
{
    return finished;
}

void SpriteAnimation::setFrameDelay(int delay)
{
    frameDelay = qMax(1, delay);
}

void SpriteAnimation::setLoop(bool loop)
{
    this->loop = loop;
}
