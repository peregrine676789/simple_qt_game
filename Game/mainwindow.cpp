#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 设置窗口获取焦点，否则可能接收不到按键
    this->setFocusPolicy(Qt::StrongFocus);

    // 【固定窗口大小】禁止用户调整窗口
    this->setFixedSize(960, 640);

    //设置初始位置
    ui->pushButton->move(50,50);

    // 初始化定时器
    timer = new QTimer(this);
    // 连接信号：定时器每隔一段时间触发一次移动逻辑，timer每记一次执行一次槽函数
    connect(timer, &QTimer::timeout, this, &MainWindow::moveTimeout);

    // ========== 初始化陷阱独立定时器 ==========
    trapTimer = new QTimer(this);
    // 陷阱独立运行，不受玩家状态影响
    connect(trapTimer, &QTimer::timeout, this, &MainWindow::trapLoop);
    // 启动陷阱定时器（始终运行）
    trapTimer->start(16);  // 约60fps


    // 安装事件过滤器，让 MainWindow 接管 widget 的绘制
    ui->widget->installEventFilter(this);

    // ========== 加载精灵图素材 ==========
    loadSprites();

    // 初始化墙体数据
    setupWalls();

    // 初始化开关
    setupSwitches();

    // 初始化开关门
    setupSwitchDoors();

    // 初始化陷阱
    setupTraps();

    // 初始化通关区域
    setupGoalZone();


    // 初始化动作状态为待机
    startAction(ActionState::Idle);
}

MainWindow::~MainWindow()
{
    // 释放陷阱内存
    for (Trap* trap : traps) {
        delete trap;
    }
    delete ui;
}


// ========== 【新增】重置游戏 ==========

void MainWindow::resetGame()
{
    // 重置游戏状态
    gameState = GameState::Playing;

    // 重置玩家位置
    ui->pushButton->move(50, 50);

    // 重置HP
    playerHP = maxHP;
    isInvincible = false;
    invincibleTimer = 0;

    // 重置动作状态
    startAction(ActionState::Idle);

    // 重置冷却
    dashCooldownTimer = 0;
    attackCooldownTimer = 0;
    dashInProgress = false;

    // 重置按键
    pressedKeys.clear();

    // 重置陷阱
    for (Trap* trap : traps) {
        delete trap;
    }
    traps.clear();
    setupTraps();

    // 重置开关和门
    switches.clear();
    switchDoors.clear();
    setupSwitches();
    setupSwitchDoors();

    // 重置通关区域
    setupGoalZone();

    // 启动游戏定时器
    if (!timer->isActive()) {
        timer->start(16);
    }

    // 确保陷阱定时器运行
    if (!trapTimer->isActive()) {
        trapTimer->start(16);
    }

    ui->widget->update();
    qDebug() << "Game reset!";
}


// ========== 初始化陷阱 ==========

void MainWindow::setupTraps()
{
    // 清空已有陷阱
    for (Trap* trap : traps) {
        delete trap;
    }
    traps.clear();

    // 【固定陷阱】地面尖刺
    traps.append(new Trap(170, 300, 60, 20, 20));      // 普通伤害20

    // 【固定陷阱】墙壁陷阱
    traps.append(new Trap(400, 150, 20, 100, 15));


    // 【可移动陷阱】水平来回移动
    // 起点(300,100) 终点(500,100) 速度3
    traps.append(new MovingTrap(300, 100, 30, 30, 25,
                                500, 100, 3.0));

    // 【可移动陷阱】垂直来回移动
    // 起点(600,200) 终点(600,400) 速度2
    traps.append(new MovingTrap(600, 100, 30, 30, 25,
                                600, 400, 2.0));

    // 【可移动陷阱】斜向移动（快速）
    traps.append(new MovingTrap(150, 200, 25, 25, 30,
                                200, 350, 2.0));

    traps.append(new Trap(150, 310, 20, 60, 15));
    traps.append(new Trap(220, 360, 150, 10, 15));
    traps.append(new MovingTrap(150, 480, 30, 30, 25,
                                500, 480, 3.0));
    traps.append(new MovingTrap(500, 460, 30, 30, 25,
                                940, 460, 3.0));


    ui->widget->update();
}

// ========== 初始化通关区域 ==========

void MainWindow::setupGoalZone()
{
    // 通关区域在地图右下角
    goalZone = GoalZone(850, 550, 80, 60, QColor("#00FF00"));
    goalZone.isActive = true;

    ui->widget->update();
}

// ========== 加载精灵图 ==========

void MainWindow::loadSprites()
{
    // ========== 加载背景图片 ==========
    backgroundPixmap = QPixmap(":/images/image/back.png");
    if (backgroundPixmap.isNull()) {
        qDebug() << "Warning: background.png not loaded!";
    } else {
        qDebug() << "Background loaded:" << backgroundPixmap.size();
    }

    // 根据实际帧数调整 frameCount
    // 假设每张精灵图有 4 帧，每帧 64x64 像素

    animIdle.load(":/images/image/trex/PNG/Trex_Idle.png", spriteWidth, spriteHeight, 2);
    animIdle.setFrameDelay(12);      // 待机动画慢一些
    animIdle.setLoop(true);

    animWalk.load(":/images/image/trex/PNG/Trex_Move.png", spriteWidth, spriteHeight, 4);
    animWalk.setFrameDelay(6);      // 行走动画正常速度
    animWalk.setLoop(true);

    animAttack.load(":/images/image/pterodactyl/PNG/ptero atack.png", spriteWidth, spriteHeight, 5);
    animAttack.setFrameDelay(2);    // 攻击动画快一些
    animAttack.setLoop(false);      // 攻击不循环

    animWalkAttack.load(":/images/image/pterodactyl/PNG/ptero walk atack.png", spriteWidth, spriteHeight, 2);
    animWalkAttack.setFrameDelay(4);
    animWalkAttack.setLoop(false);

    animHurt.load(":/images/image/trex/PNG/Trex_Hurt.png", spriteWidth, spriteHeight, 2);
    animHurt.setFrameDelay(6);
    animHurt.setLoop(false);

    animDeath.load(":/images/image/trex/PNG/Trex_Dead.png", spriteWidth, spriteHeight, 4);
    animDeath.setFrameDelay(10);
    animDeath.setLoop(false);
}

// ========== 获取当前动画 ==========

SpriteAnimation* MainWindow::getCurrentAnimation()
{
    switch (action.state) {
    case ActionState::Idle:
        return &animIdle;
    case ActionState::Move:
        return &animWalk;
    case ActionState::Attack:
        return &animAttack;
    case ActionState::MoveAttack:
        return &animWalkAttack;
    case ActionState::Hurt:
    case ActionState::Knockback:
        return &animHurt;
    case ActionState::Death:
        return &animDeath;
    default:
        return &animIdle;
    }
}

// ========== 绘制精灵图（带翻转）==========

void MainWindow::drawSprite(QPainter& painter, const QRect& destRect)
{
    SpriteAnimation* anim = getCurrentAnimation();
    if (!anim) return;

    QPixmap frame = anim->getCurrentFrame();
    if (frame.isNull()) {
        // 如果图片加载失败，画一个红色方块作为提示
        painter.setBrush(Qt::red);
        painter.drawRect(destRect);
        return;
    }

    //放大:16x16到destRect大小
    QPixmap scaled = frame.scaled(destRect.size(), Qt::KeepAspectRatio, Qt::FastTransformation);


    // 根据朝向翻转图片
    if (!facingRight) {
        QPixmap mirrored = frame.transformed(QTransform().scale(-1, 1));
        painter.drawPixmap(destRect, mirrored);
    } else {
        painter.drawPixmap(destRect, frame);
    }
}

// ========== 墙体初始化 ==========

void MainWindow::setupWalls()
{
    wallsBase.clear();

    // 以容器基准大小为参考的墙体坐标
    auto createWall = [&](int x, int y, int w, int h) {
        wallsBase.append(QRect(x, y, w, h));
    };

    createWall(150, 100, 20, 150);   // 竖墙
    createWall(220, 300, 140, 20);   // 横墙
    createWall(50, 250, 100, 20);    // 底部墙
    createWall(0,0,960,10);
    createWall(0,10,20,630);
    createWall(930,10,20,620);
    createWall(20,600,920,10);
    createWall(50,250,30,260);
    createWall(150,150,150,20);
    createWall(300,10,20,150);
    createWall(50,550,100,10);
    createWall(150,350,20,200);
    createWall(250,360,10,180);
    createWall(300,400,10,200);
    createWall(350,320,20,250);
    createWall(500,400,10,200);
    createWall(250,590,160,20);
    createWall(370,400,130,10);
    createWall(500,100,10,250);
    createWall(600,400,150,10);
    createWall(600,400,10,140);
    createWall(670,450,10,160);
    createWall(750,400,10,200);
    createWall(500,100,100,20);
    createWall(600,10,20,100);
    createWall(600,200,250,10);
    createWall(700,50,20,150);
    createWall(770,10,10,140);
    createWall(840,100,10,100);
    createWall(850,120,100,20);
    createWall(640,300,20,100);
    createWall(640,300,150,10);
    createWall(790,300,10,50);
    createWall(790,440,10,100);
    createWall(800,380,150,10);
    createWall(800,480,150,10);
    // createWall();

    // 触发重绘，显示墙体
    ui->widget->update();
}

// ========== 开关初始化 ==========

void MainWindow::setupSwitches()
{
    switches.clear();

    // 创建开关：位置、关闭颜色、打开颜色
    // 开关被攻击后会变色（打开状态）
    switches.append(gameSwitch(450, 450, 30, 30, QColor("#555555"), QColor("#00FF00")));   // 灰色→绿色
    switches.append(gameSwitch(200, 80, 30, 30, QColor("#555555"), QColor("#00FF00")));  // 灰色→绿色
    switches.append(gameSwitch(320, 330, 20, 20, QColor("#555555"), QColor("#00FF00")));
    switches.append(gameSwitch(480, 15, 30, 30, QColor("#555555"), QColor("#00FF00")));

    ui->widget->update();
}

// ========== 开关门初始化 ==========

void MainWindow::setupSwitchDoors()
{
    switchDoors.clear();

    // 创建开关门：开关打开后，门消失（可通过）
    // 门1：对应 switches[0]
    switchDoors.append(SwitchDoor(300, 150, 20, 150, QColor("#8B0000")));  // 深红色门

    // 门2：对应 switches[1]
    switchDoors.append(SwitchDoor(80, 350, 80, 20, QColor("#8B0000")));  // 深红色门

    switchDoors.append(SwitchDoor(250, 540, 10, 60, QColor("#8B0000")));

    switchDoors.append(SwitchDoor(500, 350, 10, 50, QColor("#8B0000")));
    ui->widget->update();
}

// ========== 获取碰撞墙体（包含关闭的开关门）==========

QVector<QRect> MainWindow::getCollisionWalls() const
{
    QVector<QRect> collisionWalls = wallsBase;

    // 添加关闭状态的开关门
    for (const SwitchDoor& door : switchDoors) {
        if (!door.isOpen) {
            collisionWalls.append(door.baseRect);
        }
    }

    return collisionWalls;
}

// ========== 事件过滤器：自定义绘制墙体 ==========

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    // 只拦截 ui->widget 的绘制事件
    if (watched == ui->widget && event->type() == QEvent::Paint) {
        QPainter painter(ui->widget);
        painter.setRenderHint(QPainter::Antialiasing); // 开启抗锯齿


        // ============================================================
        if (!backgroundPixmap.isNull()) {
            QRect widgetRect = ui->widget->rect();


            // 保持比例缩放填充（推荐）
            QPixmap scaledBg = backgroundPixmap.scaled(
                widgetRect.size(),
                Qt::KeepAspectRatioByExpanding,  // 保持比例，超出部分裁剪
                Qt::SmoothTransformation         // 平滑缩放
                );
            // 居中绘制
            int x = (widgetRect.width() - scaledBg.width()) / 2;
            int y = (widgetRect.height() - scaledBg.height()) / 2;
            painter.drawPixmap(x, y, scaledBg);

        } else {
            // 背景加载失败，用纯色填充
            painter.fillRect(ui->widget->rect(), QColor("#2d2d2d"));
        }


        // ===== 绘制普通墙体 =====
        painter.setPen(QPen(QColor("#5D2906"), 1));    // 边框颜色
        painter.setBrush(QColor("#8B4513"));           // 填充颜色（棕色）

        for (const QRect& wall : wallsBase) {
            painter.drawRect(wall);
        }

        // ===== 绘制开关门（关闭时显示）=====
        for (const SwitchDoor& door : switchDoors) {
            if (!door.isOpen) {
                painter.setPen(QPen(QColor("#4A0000"), 2));      // 深色边框
                painter.setBrush(door.closedColor);               // 深红色填充
                painter.drawRect(door.baseRect);
            }
        }

        // ===== 绘制开关 =====
        for (const gameSwitch& sw : switches) {
            painter.setPen(QPen(QColor("#333"), 2));           // 边框
            painter.setBrush(sw.getColor());                    // 根据状态变色

            // 画圆角矩形作为开关
            painter.drawRoundedRect(sw.baseRect, 5, 5);

            // 如果开关已打开，画一个发光效果
            if (sw.isOpen) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(0, 255, 0, 80));  // 半透明绿色光晕
                painter.drawRoundedRect(sw.baseRect.adjusted(-5, -5, 5, 5), 8, 8);
            }
        }

        // 【新增】绘制陷阱
        for (Trap* trap : traps) {
            trap->draw(painter);
        }

        // 【新增】绘制通关区域
        if (goalZone.isActive && gameState == GameState::Playing) {
            // 绘制闪烁效果
            int alpha = 100 + qAbs(qSin(QDateTime::currentMSecsSinceEpoch() / 500.0)) * 100;
            QColor glowColor = goalZone.color;
            glowColor.setAlpha(alpha);

            painter.setPen(QPen(goalZone.color, 2, Qt::DashLine));
            painter.setBrush(glowColor);
            painter.drawRoundedRect(goalZone.rect, 10, 10);

            // 绘制提示文字
            painter.setPen(Qt::white);
            QFont font = painter.font();
            font.setPointSize(10);
            painter.setFont(font);
            painter.drawText(goalZone.rect, Qt::AlignCenter, "按 J 通关");
        }


        // 在绘制精灵图之前，更新死亡动画帧
        if (gameState == GameState::Dead) {
            SpriteAnimation* anim = getCurrentAnimation();
            if (anim) anim->update();
        }

        // ===== 绘制角色精灵图 =====
        // 【关键】pushButton 作为碰撞体和位置参考，实际显示用精灵图
        QRect playerRect = ui->pushButton->geometry();
        drawSprite(painter, playerRect);

        // 无敌时闪烁效果
        if (isInvincible && (invincibleTimer / 4) % 2 == 0) {
            // 闪烁时不绘制（透明）
        } else {
            drawSprite(painter, playerRect);
        }

        // 绘制UI（血条、状态等）
        drawUI(painter);

        // ===== 根据游戏状态绘制覆盖层 =====
        if (gameState == GameState::Dead) {
            drawGameOver(painter);
        } else if (gameState == GameState::Clear) {
            drawGameClear(painter);
        }

        // 通关画面
        if(gameState == GameState::Clear){}//////////增加后续关卡



        // 返回 true 表示我们已经处理了绘制，
        // 阻止 Qt 使用默认的绘制逻辑
        return true;
    }

    // 其他事件交给基类处理
    return QMainWindow::eventFilter(watched, event);

}


// ========== 绘制UI ==========

void MainWindow::drawUI(QPainter& painter)
{
    // 绘制血条背景
    QRect hpBg(20, 20, 200, 20);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(80, 0, 0));
    painter.drawRect(hpBg);

    // 绘制血条
    int hpWidth = (playerHP * 200) / maxHP;
    QRect hpBar(20, 20, hpWidth, 20);
    painter.setBrush(QColor(220, 50, 50));
    painter.drawRect(hpBar);

    // 血条边框
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(hpBg);

    // 血量文字
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPointSize(10);
    painter.setFont(font);
    painter.drawText(hpBg, Qt::AlignCenter,
                     QString("HP: %1/%2").arg(playerHP).arg(maxHP));
}

// ========== 检测陷阱碰撞 ==========

void MainWindow::checkTrapCollision()
{
    if (isInvincible || gameState != GameState::Playing) return;

    QRect playerRect = ui->pushButton->geometry();

    for (Trap* trap : traps) {
        if (trap->collidesWith(playerRect)) {
            // 受到伤害
            playerTakeDamage(trap->getDamage());

            // 击退效果
            QVector2D knockDir = QVector2D(ui->pushButton->pos() - trap->getRect().center());
            if (knockDir.length() < 0.1) knockDir = QVector2D(1, 0);
            knockDir.normalize();
            facing = knockDir;

            startAction(ActionState::Knockback);
            action.velocity = knockDir * 8;  // 击退速度
            return;  // 一帧只受一次伤害
        }
    }
}

// ========== 玩家受伤 ==========

void MainWindow::playerTakeDamage(int damage)
{
    playerHP = qMax(0, playerHP - damage);

    // 启动无敌帧
    isInvincible = true;
    invincibleTimer = INVINCIBLE_DURATION;

    qDebug() << "Player took damage:" << damage << "HP:" << playerHP;

    // 死亡判定
    if (playerHP <= 0) {
        startAction(ActionState::Death);
        gameState = GameState::Dead;
        timer->stop();  // 停止游戏逻辑
        pressedKeys.clear();  // 清空按键
        ui->widget->update();        // 触发重绘显示死亡画面
        return;
    }
}

// ========== 检测通关交互 ==========

void MainWindow::checkGoalInteraction()
{
    if (!goalZone.isActive || gameState == GameState::Clear) return;

    QRect playerRect = ui->pushButton->geometry();

    // 检查是否在通关区域内
    if (goalZone.rect.contains(playerRect.center())) {
        // 在区域内，按J键通关
        // 注意：攻击键也是J，所以在区域内优先判定为交互
        if (pressedKeys.contains(Qt::Key_J) && action.canInterrupt) {
            gameState = GameState::Clear;
            qDebug() << "Game Clear!";

            // 停止游戏循环
            timer->stop();
            pressedKeys.clear();

            // 显示通关画面
            ui->widget->update();
        }
    }
}

// ========== 按键事件处理 ==========

// 按下按键时
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 忽略自动重复产生的 keyPress
    if (event->isAutoRepeat()) {
        return;
    }

    int key = event->key();         // key来存每次按下的键位
    pressedKeys.insert(key);        // 将key存入pressedKeys集合

    // ========== 死亡/通关状态下按空格重新开始 ==========
    if (key == Qt::Key_Space && (gameState == GameState::Dead || gameState == GameState::Clear)) {
        resetGame();
        return;
    }

    // 死亡状态下不响应其他按键
    if (gameState == GameState::Dead) return;

    if (!timer->isActive()) {
        timer->start(16);           // 每16ms记一次，相当于60帧
    }
}

// 松开按键时
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    // 忽略自动重复产生的 keyRelease
    if (event->isAutoRepeat()) {
        return;
    }

    int key = event->key();
    pressedKeys.remove(key);

    // 死亡状态下不处理按键释放
    if (gameState == GameState::Dead) return;

    // 集合中没有key时timer停止
    if (pressedKeys.isEmpty()) {
        timer->stop();
    }
}

// ========== 冷却检查方法 ==========

// 检查冲刺是否可用
bool MainWindow::canDash() const
{
    // 冷却完毕且不在冲刺中
    return dashCooldownTimer <= 0 && !dashInProgress;
}

// 检查攻击是否可用
bool MainWindow::canAttack() const
{
    // 冷却完毕
    return attackCooldownTimer <= 0;
}

// ========== 动作系统 ==========

// 启动新动作
void MainWindow::startAction(ActionState newState)
{
    // 状态切换时重置动画
    if (action.state != newState) {
        SpriteAnimation* anim = getCurrentAnimation();
        if (anim) anim->reset();
    }

    action.state = newState;
    action.frame = 0;

    switch (newState) {
    case ActionState::Dash:
    {
        action.duration = DASH_DURATION;
        action.speed = DASH_SPEED;
        action.canInterrupt = false;            // 冲刺不可被打断

        // 【冲刺限制】记录冲刺起点和固定方向
        dashInProgress = true;
        dashStartPos = ui->pushButton->pos();

        // 使用当前面向作为固定冲刺方向
        QVector2D dashDir = facing;
        if (dashDir.length() < 0.1) {
            // 如果没有面向（刚启动时），默认朝右
            dashDir = QVector2D(1, 0);
        }
        dashDir.normalize();

        // 设置速度向量
        action.velocity = dashDir * action.speed;

        // 启动冲刺冷却
        dashCooldownTimer = DASH_COOLDOWN;
    }
    break;

    case ActionState::Attack:
    case ActionState::MoveAttack:
        action.duration = ATTACK_DURATION;
        action.speed = 0;
        // 【关键】攻击允许被打断，这样移动可以同时执行
        action.canInterrupt = true;
        action.velocity = QVector2D(0, 0);

        // 【攻击冷却】启动冷却计时
        attackCooldownTimer = ATTACK_COOLDOWN;

        // 生成攻击判定框（只朝 facing 方向）
        spawnAttackHitbox();

        //检查是否打开开关
        checkAttackHitSwitches();
        break;

    case ActionState::Knockback:
        action.duration = KNOCKBACK_DURATION;
        action.canInterrupt = false;            // 击退不可被打断
        break;

    case ActionState::Move:
        action.canInterrupt = true;              // 移动可以随时切换
        break;

    case ActionState::Idle:
        action.speed = 0;
        action.velocity = QVector2D(0, 0);
        action.canInterrupt = true;
        break;
    case ActionState::Death:
        action.duration = 65;         // 1秒死亡动画
        action.speed = 0;
        action.canInterrupt = false;
        action.velocity = QVector2D(0, 0);
        break;

    }

    // 更新按钮颜色表现
    updateVisual();
}

// 更新当前动作（每帧调用）
void MainWindow::updateAction()
{
    action.frame++;

    // 更新当前动画帧
    SpriteAnimation* anim = getCurrentAnimation();
    if (anim) anim->update();

    // ========== 冲刺结束判断（距离限制）==========
    if (action.state == ActionState::Dash) {
        QPoint currentPos = ui->pushButton->pos();

        // 计算已冲刺距离
        qreal dx = currentPos.x() - dashStartPos.x();
        qreal dy = currentPos.y() - dashStartPos.y();
        qreal dist = qSqrt(dx*dx + dy*dy);
        int scaledTargetDist = DASH_DISTANCE;

        // 结束条件1：到达或超过目标距离
        bool reachedDistance = dist >= scaledTargetDist;

        // 结束条件2：超出最大持续时间
        bool timeout = action.frame >= action.duration;

        // 结束条件3：撞墙（通过碰撞检测自然停止）

        if (reachedDistance || timeout) {
            // 冲刺结束，强制到达目标点或停止
            dashInProgress = false;

            // 恢复为待机或移动
            bool wantMove = pressedKeys.contains(Qt::Key_W) ||
                            pressedKeys.contains(Qt::Key_S) ||
                            pressedKeys.contains(Qt::Key_A) ||
                            pressedKeys.contains(Qt::Key_D);
            startAction(wantMove ? ActionState::Move : ActionState::Idle);
            return;
        }
    }

    // 攻击结束判断
    if (action.state == ActionState::Attack && action.frame >= action.duration) {
        bool wantMove = pressedKeys.contains(Qt::Key_W) ||
                        pressedKeys.contains(Qt::Key_S) ||
                        pressedKeys.contains(Qt::Key_A) ||
                        pressedKeys.contains(Qt::Key_D);
        startAction(wantMove ? ActionState::Move : ActionState::Idle);
        return;
    }

    // 其他动作结束判断
    if (action.frame >= action.duration && action.state != ActionState::Dash) {
        bool wantMove = pressedKeys.contains(Qt::Key_W) ||
                        pressedKeys.contains(Qt::Key_S) ||
                        pressedKeys.contains(Qt::Key_A) ||
                        pressedKeys.contains(Qt::Key_D);
        startAction(wantMove ? ActionState::Move : ActionState::Idle);
        return;
    }

    // 击退中：速度快速衰减
    if (action.state == ActionState::Knockback) {
        qreal t = (qreal)action.frame / action.duration;
        action.velocity *= (1 - t);  // 线性减速到0
    }
}

// 执行移动（根据输入和动作状态计算位移）
void MainWindow::executeMovement()
{
    // 死亡状态下不能移动
    if (gameState == GameState::Dead) return;
    // 获取输入方向
    QVector2D input(0, 0);
    if (pressedKeys.contains(Qt::Key_W)) input.setY(-1);
    if (pressedKeys.contains(Qt::Key_S)) input.setY(1);
    if (pressedKeys.contains(Qt::Key_A)) input.setX(-1);
    if (pressedKeys.contains(Qt::Key_D)) input.setX(1);

    // 更新朝向（用于精灵图翻转）
    if (input.x() > 0) facingRight = true;
    if (input.x() < 0) facingRight = false;

    // 归一化方向并更新面向
    if (input.length() > 0) {
        input.normalize();
        facing = input;  // 更新面向为当前输入方向
    }

    // 计算位移
    QVector2D delta(0, 0);

    // 【关键改动】攻击时也可以移动！只要按了方向键就移动
    if (input.length() > 0) {
        delta = input * MOVE_SPEED;
    }

    // 冲刺时：使用固定方向和速度，忽略输入
    if (action.state == ActionState::Dash) {
        delta = action.velocity;
    }
    // 击退时：使用动作自带位移
    else if (action.state == ActionState::Knockback) {
        delta = action.velocity;
    }

    // // 应用位移（带碰撞检测）
    // if (delta.length() > 0) {
    //     applyMovement(delta);
    // }
    applyMovement(delta);
}


// 应用位移（带墙体碰撞检测）
void MainWindow::applyMovement(const QVector2D& delta)
{
    QPoint pos = ui->pushButton->pos();
    QSize sz = ui->pushButton->size();
    auto walls = getCollisionWalls();

    // X方向移动
    int newX = pos.x() + delta.x();
    // 限制在容器边界内，防止越界
    newX = qBound(0, newX, qMax(0, ui->widget->width() - sz.width()));
    // 检测是否与墙体碰撞
    QRect testX(newX, pos.y(), sz.width(), sz.height());
    bool blockedX = false;
    for (const auto& wall : walls) {
        if (testX.intersects(wall)) { blockedX = true; break; }
    }
    if (!blockedX) pos.setX(newX);

    // Y方向移动
    int newY = pos.y() + delta.y();
    newY = qBound(0, newY, qMax(0, ui->widget->height() - sz.height()));
    QRect testY(pos.x(), newY, sz.width(), sz.height());
    bool blockedY = false;
    for (const auto& wall : walls) {
        if (testY.intersects(wall)) { blockedY = true; break; }
    }
    if (!blockedY) pos.setY(newY);

    // 移动按钮
    ui->pushButton->move(pos);
}

// 更新视觉表现（按钮颜色根据状态变化）
void MainWindow::updateVisual()
{
    // QString style;
    // switch (action.state) {
    // case ActionState::Idle:
    //     style = "background-color: #4CAF50;";        // 绿色：待机
    //     break;
    // case ActionState::Move:
    //     style = "background-color: #2196F3;";  // 蓝色：移动
    //     break;
    // case ActionState::Dash:
    //     style = "background-color: #FF9800; border: 3px solid #FFF;";  // 橙色+白边：冲刺
    //     break;
    // case ActionState::Attack:
    //     style = "background-color: #F44336; border: 2px solid #FF0;";  // 红色+黄边：攻击
    //     break;
    // case ActionState::Knockback:
    //     style = "background-color: #9C27B0;";  // 紫色：击退
    //     break;
    // }
    // ui->pushButton->setStyleSheet(style);

    // 精灵图通过 eventFilter 绘制，这里不需要设置样式
    // 可以保留用于调试或隐藏 pushButton
    ui->pushButton->setStyleSheet("background-color: transparent; border: none;");
    ui->pushButton->setText("");
}

// 生成冲刺拖尾特效
void MainWindow::spawnTrail()
{
    QLabel* trail = new QLabel(ui->widget);
    trail->setGeometry(ui->pushButton->geometry());
    trail->setStyleSheet("background-color: rgba(255,152,0,0.4); border-radius: 4px;");
    trail->show();

    // 淡出动画
    QPropertyAnimation* fade = new QPropertyAnimation(trail, "windowOpacity");
    fade->setDuration(200);
    fade->setStartValue(1.0);
    fade->setEndValue(0.0);
    connect(fade, &QPropertyAnimation::finished, trail, &QLabel::deleteLater);
    fade->start();
}

// 生成攻击判定框（只朝 facing 方向）
void MainWindow::spawnAttackHitbox()
{
    QPoint p = ui->pushButton->pos();
    QSize s = ui->pushButton->size();
    int r = 40;  // 固定为40px

    // 【关键】只朝 facing 方向生成攻击框
    QRect hitbox;
    if (facing.x() > 0.5) {           // 朝右攻击
        hitbox = QRect(p.x() + s.width(), p.y() + (s.height() - r) / 2, r, r);
    } else if (facing.x() < -0.5) {   // 朝左攻击
        hitbox = QRect(p.x() - r, p.y() + (s.height() - r) / 2, r, r);
    } else if (facing.y() < -0.5) {   // 朝上攻击
        hitbox = QRect(p.x() + (s.width() - r) / 2, p.y() - r, r, r);
    } else {                          // 朝下攻击（默认）
        hitbox = QRect(p.x() + (s.width() - r) / 2, p.y() + s.height(), r, r);
    }

    // 限制在容器内
    hitbox.setX(qMax(0, hitbox.x()));
    hitbox.setY(qMax(0, hitbox.y()));
    hitbox.setWidth(qMin(hitbox.width(), qMax(0, ui->widget->width() - hitbox.x())));
    hitbox.setHeight(qMin(hitbox.height(), qMax(0, ui->widget->height() - hitbox.y())));

    // 创建可视化攻击框
    QWidget* vis = new QWidget(ui->widget);
    vis->setGeometry(hitbox);
    vis->setAttribute(Qt::WA_StyledBackground);  // 确保样式表生效
    vis->setStyleSheet("background-color: rgba(255,0,0,0.6); border: 2px solid #FF0000;");
    vis->raise();   // 提升到最上层
    vis->show();    // 强制显示

    qDebug() << "Attack hitbox:" << hitbox << "facing:" << facing;

    // 150ms后自动删除
    QTimer::singleShot(150, vis, &QWidget::deleteLater);

}

// ========== 开关检测核心 ==========

// 【开关机制】检测攻击判定框是否命中开关，命中则打开开关并开启对应门
void MainWindow::checkAttackHitSwitches()
{
    QPoint p = ui->pushButton->pos();
    QSize s = ui->pushButton->size();
    int r = 40;  // 攻击范围固定40px

    // 重新计算攻击框（与 spawnAttackHitbox 一致）
    QRect hitbox;
    if (facing.x() > 0.5) {
        hitbox = QRect(p.x() + s.width(), p.y() + (s.height() - r) / 2, r, r);
    } else if (facing.x() < -0.5) {
        hitbox = QRect(p.x() - r, p.y() + (s.height() - r) / 2, r, r);
    } else if (facing.y() < -0.5) {
        hitbox = QRect(p.x() + (s.width() - r) / 2, p.y() - r, r, r);
    } else {
        hitbox = QRect(p.x() + (s.width() - r) / 2, p.y() + s.height(), r, r);
    }

    // 遍历所有开关，检测碰撞
    for (int i = 0; i < switches.size(); ++i) {
        gameSwitch& sw = switches[i];  // 注意：这里需要非const引用才能修改

        // 如果开关已经打开，跳过
        if (sw.isOpen) continue;

        // 检测攻击框与开关是否相交
        if (hitbox.intersects(sw.baseRect)) {
            // 【开关被击中】打开开关
            sw.isOpen = true;
            qDebug() << "Switch" << i << "activated!";

            // 【开启对应门】打开对应的开关门
            if (i < switchDoors.size()) {
                switchDoors[i].isOpen = true;
                qDebug() << "Door" << i << "opened!";
            }

            // 触发重绘，显示开关变化和门消失
            ui->widget->update();
        }
    }
}


// ========== 绘制死亡画面 ==========

void MainWindow::drawGameOver(QPainter& painter)
{
    // 半透明黑色覆盖层
    painter.fillRect(ui->widget->rect(), QColor(0, 0, 0, 180));

    // "You Are Dead" 大字
    painter.setPen(QColor("#FF0000"));
    QFont bigFont = painter.font();
    bigFont.setPointSize(72);
    bigFont.setBold(true);
    painter.setFont(bigFont);
    painter.drawText(ui->widget->rect(), Qt::AlignCenter, "You Are Dead");

    // 重新开始提示
    painter.setPen(Qt::white);
    QFont smallFont = painter.font();
    smallFont.setPointSize(20);
    painter.setFont(smallFont);
    painter.drawText(ui->widget->rect().adjusted(0, 120, 0, 0),
                     Qt::AlignCenter, "按 空格 重新开始");
}

// ========== 绘制通关画面 ==========

void MainWindow::drawGameClear(QPainter& painter)
{
    // 半透明黑色覆盖层
    painter.fillRect(ui->widget->rect(), QColor(0, 0, 0, 180));

    // "通关" 大字
    painter.setPen(QColor("#00FF00"));
    QFont bigFont = painter.font();
    bigFont.setPointSize(72);
    bigFont.setBold(true);
    painter.setFont(bigFont);
    painter.drawText(ui->widget->rect(), Qt::AlignCenter, "通关！");

    // 重新开始提示
    painter.setPen(Qt::white);
    QFont smallFont = painter.font();
    smallFont.setPointSize(20);
    painter.setFont(smallFont);
    painter.drawText(ui->widget->rect().adjusted(0, 120, 0, 0),
                     Qt::AlignCenter, "按 空格 重新开始");
}


// ========== 陷阱独立循环 ==========

void MainWindow::trapLoop()
{
    // 陷阱始终独立运行，不受游戏状态影响
    // 更新所有陷阱位置
    for (Trap* trap : traps) {
        trap->update();
    }

    // 即使人物不动，也要检测陷阱碰撞
    if (gameState == GameState::Playing) {
        checkTrapCollision();
    }

    // 无敌帧递减
    if (invincibleTimer > 0) {
        invincibleTimer--;
        if (invincibleTimer <= 0) {
            isInvincible = false;
        }
    }

    // 触发重绘，显示陷阱新位置
    ui->widget->update();
}

// ========== 主循环：定时器槽函数 ==========

void MainWindow::moveTimeout()
{
    this->setFocus();

    // 如果已死亡或已通关，不执行游戏逻辑
    if (gameState == GameState::Dead || gameState == GameState::Clear) return;

    // ========== 0. 冷却计时器递减 ==========
    // 冲刺冷却计时
    if (dashCooldownTimer > 0) {
        dashCooldownTimer--;
    }
    // 攻击冷却计时
    if (attackCooldownTimer > 0) {
        attackCooldownTimer--;
    }



    // ========== 1. 处理输入，更新面向 ==========
    QVector2D input(0, 0);
    bool isMoving = false;
    if (pressedKeys.contains(Qt::Key_W)) {input.setY(-1);isMoving = true;}
    if (pressedKeys.contains(Qt::Key_S)) {input.setY(1);isMoving = true;}
    if (pressedKeys.contains(Qt::Key_A)) {input.setX(-1);isMoving = true;}
    if (pressedKeys.contains(Qt::Key_D)) {input.setX(1);isMoving = true;}

    // 归一化方向，更新面向
    if (input.length() > 0) {
        input.normalize();
        facing = input;
    }

    // ========== 2. 检测动作触发键 ==========
    // 空格 = 冲刺（检查冷却和是否正在冲刺）
    if (pressedKeys.contains(Qt::Key_Space) && canDash()) {
        startAction(ActionState::Dash);
    }
    // J = 通关交互（优先）/攻击（检查冷却，且不可打断状态下不能触发）
    // 【攻击冷却】使用 canAttack() 检查
    if (pressedKeys.contains(Qt::Key_J)) {
        // 检查是否在通关区域内
        QRect playerRect = ui->pushButton->geometry();
        bool inGoalZone = goalZone.rect.contains(playerRect.center());

        if (inGoalZone && goalZone.isActive &&gameState == GameState::Playing) {
            // 在通关区域内，执行交互
            checkGoalInteraction();
        } else if (canAttack() && action.canInterrupt) {
            // 不在通关区域，执行攻击
            if (isMoving) {
                startAction(ActionState::MoveAttack);
            } else {
                startAction(ActionState::Attack);
            }
        }
    }


    // ========== 更新当前动作状态 ==========
    updateAction();

    // ==========  执行移动（攻击时也能移动！）==========
    executeMovement();

    // ==========  检测陷阱碰撞 ==========
    checkTrapCollision();

    // ==========  检测通关交互（每帧检查位置）==========
    checkGoalInteraction();


    // ==========  特效 ==========
    // 冲刺时添加拖尾效果
    if (action.state == ActionState::Dash && action.frame % 2 == 0) {
        spawnTrail();
    }

    // 触发重绘，更新精灵图动画帧
    ui->widget->update();

}

