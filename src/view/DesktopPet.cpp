#include "DesktopPet.h"
#include <QApplication>
#include <QScreen>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QThread>

DesktopPet::DesktopPet(QWidget *parent)
    : QMainWindow(parent), viewModel(new PetViewModel(this)), petLabel(nullptr), petMovie(nullptr), contextMenu(nullptr), confirmButton(nullptr), isDragging(false)
{
    setupUI();
    setupContextMenu();

    // 连接ViewModel信号
    connect(viewModel, &PetViewModel::petPositionChanged,
            this, &DesktopPet::onPetPositionChanged);
    connect(viewModel, &PetViewModel::petImageChanged,
            this, &DesktopPet::onPetImageChanged);
    connect(viewModel, &PetViewModel::movingModeChanged,
            this, &DesktopPet::onMovingModeChanged);

    // 初始化桌宠图像和位置
    loadPetImage(viewModel->getCurrentImagePath());
    move(viewModel->getPetPosition());
}

DesktopPet::~DesktopPet()
{
    if (petMovie)
    {
        delete petMovie;
    }
}

void DesktopPet::setupUI()
{
    // 设置窗口属性：无边框、置顶、透明背景
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    // 创建显示桌宠的标签
    petLabel = new QLabel(this);
    setCentralWidget(petLabel);

    // 设置窗口大小
    resize(150, 150);

    // 创建确认按钮（移动模式时显示）
    confirmButton = new QPushButton("确定", this);
    confirmButton->hide();
    confirmButton->setStyleSheet(
        "QPushButton {"
        "background-color: rgba(70, 130, 180, 200);"
        "color: white;"
        "border: 2px solid white;"
        "border-radius: 15px;"
        "padding: 5px 15px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: rgba(100, 149, 237, 200);"
        "}");
    confirmButton->resize(60, 30);
    connect(confirmButton, &QPushButton::clicked, this, &DesktopPet::onConfirmMove);
}

void DesktopPet::setupContextMenu()
{
    contextMenu = new QMenu(this);

    moveAction = new QAction("移动桌宠", this);
    changeAction = new QAction("改变桌宠", this);
    exitAction = new QAction("退出桌宠", this);

    contextMenu->addAction(moveAction);
    contextMenu->addAction(changeAction);
    contextMenu->addSeparator(); // 添加分隔线
    contextMenu->addAction(exitAction);

    connect(moveAction, &QAction::triggered, this, &DesktopPet::onMoveActionTriggered);
    connect(changeAction, &QAction::triggered, this, &DesktopPet::onChangeActionTriggered);
    connect(exitAction, &QAction::triggered, this, &DesktopPet::onExitActionTriggered);
}

void DesktopPet::loadPetImage(const QString &imagePath)
{
    QFileInfo fileInfo(imagePath);

    if (fileInfo.suffix().toLower() == "gif")
    {
        // 加载GIF动画
        if (petMovie)
        {
            delete petMovie;
            petMovie = nullptr;
        }

        petMovie = new QMovie(imagePath);
        if (petMovie->isValid())
        {
            petLabel->setMovie(petMovie);
            petMovie->start();
        }
        else
        {
            // 如果GIF加载失败，显示错误信息
            petLabel->setText("GIF加载失败\n路径：" + imagePath);
            petLabel->setStyleSheet("color: red; background: white; padding: 10px;");
        }
    }
    else
    {
        // 加载静态图片
        QPixmap pixmap(imagePath);
        if (!pixmap.isNull())
        {
            petLabel->setPixmap(pixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        else
        {
            // 如果图片加载失败，显示错误信息
            petLabel->setText("图片加载失败\n路径：" + imagePath);
            petLabel->setStyleSheet("color: red; background: white; padding: 10px;");
        }
    }

    petLabel->setAlignment(Qt::AlignCenter);
}

void DesktopPet::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && viewModel->isInMovingMode())
    {
        isDragging = true;
        lastMousePosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
    }
    QMainWindow::mousePressEvent(event);
}

void DesktopPet::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging && viewModel->isInMovingMode())
    {
        move(event->globalPosition().toPoint() - lastMousePosition);
        viewModel->updatePetPosition(pos());
    }
    QMainWindow::mouseMoveEvent(event);
}

void DesktopPet::contextMenuEvent(QContextMenuEvent *event)
{
    if (!viewModel->isInMovingMode())
    {
        contextMenu->exec(event->globalPos());
    }
}

void DesktopPet::onPetPositionChanged(const QPoint &position)
{
    move(position);
}

void DesktopPet::onPetImageChanged(const QString &imagePath)
{
    loadPetImage(imagePath);
}

void DesktopPet::onMovingModeChanged(bool enabled)
{
    if (enabled)
    {
        confirmButton->show();
        confirmButton->move(width() - confirmButton->width() - 10, height() - confirmButton->height() - 10);
        setWindowOpacity(0.8); // 移动时稍微透明
    }
    else
    {
        confirmButton->hide();
        setWindowOpacity(1.0);
        isDragging = false;
    }
}

void DesktopPet::onMoveActionTriggered()
{
    viewModel->enterMovingMode();
}

void DesktopPet::onChangeActionTriggered()
{
    showChangeDialog();
}

void DesktopPet::onExitActionTriggered()
{
    // 添加确认对话框
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "退出确认",
        "确定要退出桌宠吗？",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        QApplication::quit(); // 完全退出应用程序
    }
}

void DesktopPet::onConfirmMove()
{
    viewModel->exitMovingMode();
}

void DesktopPet::showChangeDialog()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("选择桌宠");
    dialog->resize(250, 120);

    QVBoxLayout *layout = new QVBoxLayout(dialog);

    QComboBox *petComboBox = new QComboBox(dialog);
    petComboBox->addItem("蜘蛛", static_cast<int>(PetType::Spider));
    petComboBox->addItem("卡西迪", static_cast<int>(PetType::Cassidy));

    // 设置当前选中项
    int currentIndex = static_cast<int>(viewModel->getCurrentPetType());
    petComboBox->setCurrentIndex(currentIndex);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("确定", dialog);
    QPushButton *cancelButton = new QPushButton("取消", dialog);

    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    layout->addWidget(petComboBox);
    layout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, [=]()
            {
        int selectedType = petComboBox->currentData().toInt();
        viewModel->changePetType(static_cast<PetType>(selectedType));
        dialog->accept(); });

    connect(cancelButton, &QPushButton::clicked, dialog, &QDialog::reject);

    dialog->exec();
    dialog->deleteLater();
}