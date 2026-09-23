#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>      // 按键事件
#include <QTimer>         // 定时器
#include <QSet>           // 按键集合
#include <QVector>        // 墙体数组
#include <QRect>          // 矩形碰撞
#include <QPainter>       // 绘制墙体
#include <QVector2D>      // 2D向量（方向、速度）
#include <QLabel>         // 拖尾/攻击特效
#include <QPropertyAnimation> // 淡出动画
#include"gameswitch.h"
#include"SpriteAnimation.h"
#include "trap.h"           // 新增：陷阱基类
#include "movingtrap.h"     // 新增：可移动陷阱
#include<QDateTime>
#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// 动作状态枚举
enum class ActionState {
    Idle,       // 待机
    Move,       // 移动
    Dash,       // 冲刺
    Attack,     // 攻击
    MoveAttack, // 移动攻击
    Hurt,       // 受击
    Knockback,  // 击退
    Death       // 死亡
};

enum class GameState {
    Playing,    // 游戏中
    Dead,       // 死亡
    Clear       // 通关
};

// 动作模组结构体
struct ActionModule {
    ActionState state = ActionState::Idle;  // 当前状态
    int frame = 0;                          // 当前帧计数
    int duration = 0;                       // 动作总时长(帧)
    QVector2D velocity;                     // 速度向量
    qreal speed = 0;                        // 当前速度大小
    bool canInterrupt = true;               // 能否被其他动作打断
};


// ========== 通关区域类 ==========
struct GoalZone {
    QRect rect;             // 区域位置
    bool isActive = true;   // 是否可交互
    QColor color;           // 颜色

    GoalZone() {}
    GoalZone(int x, int y, int w, int h, QColor c)
        : rect(x, y, w, h), color(c) {}
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // 按键事件处理
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    // 初始化墙体
    void setupWalls();

    // 初始化开关
    void setupSwitches();

    // 初始化开关门
    void setupSwitchDoors();

    // 初始化陷阱
    void setupTraps();

    // 初始化通关区域
    void setupGoalZone();

    // 重置游戏
    void resetGame();


protected:
    // 事件过滤器：拦截 widget 的绘制事件，自定义绘制墙体、开关、门
    bool eventFilter(QObject *watched, QEvent *event) override;


private:
    Ui::MainWindow *ui;

    // 定时器，控制游戏循环刷新率
    QTimer *timer;
    QTimer *trapTimer;      // 【新增】陷阱独立定时器

    // 记录当前按下的按键集合
    QSet<int> pressedKeys;

    // 墙体数据（基准坐标，不变）
    QVector<QRect> wallsBase;

    // 开关数组
    QVector<gameSwitch> switches;

    // 开关门数组
    QVector<SwitchDoor> switchDoors;

    // ========== 陷阱系统 ==========
    QVector<Trap*> traps;           // 所有陷阱（基类指针）

    // ========== 通关区域 ==========
    GoalZone goalZone;

    // ========== 游戏状态 ==========
    GameState gameState = GameState::Playing;

    // ========== 玩家生命 ==========
    int playerHP =100;
    int maxHP = 100;
    bool isInvincible = false;      // 无敌帧
    int invincibleTimer = 0;
    static constexpr int INVINCIBLE_DURATION = 30;  // 0.5秒无敌

    //背景图片
    QPixmap backgroundPixmap;

    // ========== 精灵图动画系统 ==========
    SpriteAnimation animIdle;           // 待机动画
    SpriteAnimation animWalk;         // 行走动画
    SpriteAnimation animAttack;       // 攻击动画
    SpriteAnimation animWalkAttack;     // 行走攻击动画
    SpriteAnimation animHurt;           // 受击动画
    SpriteAnimation animDeath;          // 死亡动画

    bool facingRight = true;            // 是否朝右（用于翻转图片）
    int spriteWidth = 16;               // 精灵图单帧宽度
    int spriteHeight =16;        // 精灵图单帧高度
    int spriteScale=4;         //放大倍数

    // 加载所有精灵图
    void loadSprites();

    // 获取当前应播放的动画
    SpriteAnimation* getCurrentAnimation();

    // 绘制精灵图（带翻转）
    void drawSprite(QPainter& painter, const QRect& destRect);

    // ========== 动作系统成员 ==========
    ActionModule action;        // 当前动作状态
    QVector2D facing{1, 0};     // 当前朝向，默认朝右

    // ========== 冲刺限制成员 ==========
    int dashCooldownTimer = 0;          // 冲刺冷却计时（帧），2秒 = 120帧
    static constexpr int DASH_COOLDOWN = 60;  // 1秒冷却（60fps）
    QPoint dashStartPos;                // 冲刺起点
    QPoint dashTargetPos;               // 冲刺目标终点
    bool dashInProgress = false;        // 是否正在冲刺中
    static constexpr int DASH_DISTANCE = 100;  // 冲刺固定距离100px（基准值）

    // ========== 攻击冷却成员 ==========
    int attackCooldownTimer = 0;        // 攻击冷却计时（帧），0.3秒 = 18帧
    static constexpr int ATTACK_COOLDOWN = 24;  // 0.4秒冷却（60fps * 0.4）

    // 动作参数常量
    static constexpr int MOVE_SPEED = 3;           // 普通移动速度
    static constexpr int DASH_SPEED = 15;          // 冲刺速度（加快）
    static constexpr int DASH_DURATION = 20;       // 冲刺最大持续帧数（约333ms）
    static constexpr int ATTACK_DURATION = 12;     // 攻击持续帧数
    static constexpr int KNOCKBACK_DURATION = 20;  // 击退持续帧数

    // ========== 内部方法 ==========

    // 启动新动作
    void startAction(ActionState state);

    // 更新当前动作（帧计数、自动结束判断）
    void updateAction();

    // 执行移动（根据输入和动作状态计算位移）
    void executeMovement();

    // 应用位移（带墙体碰撞检测）
    void applyMovement(const QVector2D& delta);

    // 更新视觉表现（按钮颜色变化）
    void updateVisual();

    // 生成冲刺拖尾特效
    void spawnTrail();

    // 生成攻击判定框（只朝 facing 方向）
    void spawnAttackHitbox();

    // 检测攻击是否命中开关
    void checkAttackHitSwitches();

    // 获取当前碰撞墙体（包含关闭的开关门）
    QVector<QRect> getCollisionWalls() const;

    // 检测陷阱碰撞
    void checkTrapCollision();

    // 检测通关交互
    void checkGoalInteraction();

    // 玩家受伤
    void playerTakeDamage(int damage);

    // 绘制UI（血条、通关提示等）
    void drawUI(QPainter& painter);

    // 绘制死亡画面
    void drawGameOver(QPainter& painter);

    // 绘制通关画面
    void drawGameClear(QPainter& painter);

    // 检查冲刺是否可用
    bool canDash() const;

    // 检查攻击是否可用
    bool canAttack() const;

private slots:
    // 定时器槽函数：每帧执行的游戏逻辑
    void moveTimeout();
    void trapLoop();        // 陷阱独立循环
};

#endif // MAINWINDOW_H
