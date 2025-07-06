#include "ForgePanel.h"
#include "../common/CommandParameters.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

// ============ ForgePanel 实现 ============
ForgePanel::ForgePanel(CommandManager &commandManager, QWidget *parent)
    : QWidget(parent), m_commandManager(commandManager)
{
    qDebug() << "ForgePanel::ForgePanel constructor called";

    setupUI();
    connectSignals();

    qDebug() << "ForgePanel constructor completed";
}

ForgePanel::~ForgePanel()
{
}

void ForgePanel::updateForgeDisplayInfo(const ForgeDisplayInfo &info)
{
    m_displayInfo = info;
    updateResourceDisplay();
}

void ForgePanel::forge_notification_cb(uint32_t prop_id, void *pv)
{
    ForgePanel *pThis = static_cast<ForgePanel *>(pv);
    if (pThis)
    {
        // 当收到通知时，更新资源显示
        pThis->updateResourceDisplay();
    }
}

void ForgePanel::closeEvent(QCloseEvent *event)
{
    // 隐藏窗口而不是销毁，这样不会导致应用程序退出
    hide();
    event->ignore(); // 忽略关闭事件，阻止窗口销毁
    qDebug() << "ForgePanel closed (hidden)";
}

void ForgePanel::setupUI()
{
    setWindowTitle("物品合成台");
    setFixedSize(800, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 顶部资源显示区域
    setupResourceDisplay();
    mainLayout->addWidget(m_resourceDisplayWidget);

    // 中间合成按钮区域
    setupSynthesisButtons();
    mainLayout->addWidget(m_synthesisWidget);

    // 底部关闭按钮
    QPushButton *closeButton = new QPushButton("关闭");
    closeButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; border-radius: 4px; }");
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);
    mainLayout->addWidget(closeButton);
}

void ForgePanel::setupResourceDisplay()
{
    m_resourceDisplayWidget = new QWidget();
    QVBoxLayout *resourceLayout = new QVBoxLayout(m_resourceDisplayWidget);

    // 标题
    QLabel *titleLabel = new QLabel("拥有的材料");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #2E8B57; padding: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    resourceLayout->addWidget(titleLabel);

    // 资源显示网格
    QGridLayout *gridLayout = new QGridLayout();

    // 阳光材料显示
    QLabel *sunshineLabel = new QLabel("阳光材料:");
    sunshineLabel->setStyleSheet("font-weight: bold; color: #FF8C00;");
    gridLayout->addWidget(sunshineLabel, 0, 0);

    // 阳光数量标签
    m_sunshineLabels.resize(5);
    QStringList sunshineNames = {"微光阳光", "温暖阳光", "炽热阳光", "灿烂阳光", "神圣阳光"};
    for (int i = 0; i < 5; i++)
    {
        m_sunshineLabels[i] = new QLabel(QString("%1: 0").arg(sunshineNames[i]));
        m_sunshineLabels[i]->setStyleSheet("color: #333; padding: 2px;");
        gridLayout->addWidget(m_sunshineLabels[i], 0, i + 1);
    }

    // 矿石材料显示
    QLabel *mineralLabel = new QLabel("矿石材料:");
    mineralLabel->setStyleSheet("font-weight: bold; color: #8B4513;");
    gridLayout->addWidget(mineralLabel, 1, 0);

    // 矿石数量标签
    m_mineralLabels.resize(5);
    QStringList mineralNames = {"粗糙矿石", "普通矿石", "优质矿石", "稀有矿石", "传说矿石"};
    for (int i = 0; i < 5; i++)
    {
        m_mineralLabels[i] = new QLabel(QString("%1: 0").arg(mineralNames[i]));
        m_mineralLabels[i]->setStyleSheet("color: #333; padding: 2px;");
        gridLayout->addWidget(m_mineralLabels[i], 1, i + 1);
    }

    // 木材材料显示
    QLabel *woodLabel = new QLabel("木材材料:");
    woodLabel->setStyleSheet("font-weight: bold; color: #228B22;");
    gridLayout->addWidget(woodLabel, 2, 0);

    // 木材数量标签
    m_woodLabels.resize(5);
    QStringList woodNames = {"枯木", "普通木材", "优质木材", "稀有木材", "神木"};
    for (int i = 0; i < 5; i++)
    {
        m_woodLabels[i] = new QLabel(QString("%1: 0").arg(woodNames[i]));
        m_woodLabels[i]->setStyleSheet("color: #333; padding: 2px;");
        gridLayout->addWidget(m_woodLabels[i], 2, i + 1);
    }

    resourceLayout->addLayout(gridLayout);

    // 设置背景样式
    m_resourceDisplayWidget->setStyleSheet("QWidget { background-color: #f0f8ff; border: 2px solid #4682B4; border-radius: 8px; margin: 5px; }");
}

void ForgePanel::setupSynthesisButtons()
{
    m_synthesisWidget = new QWidget();
    QVBoxLayout *synthesisLayout = new QVBoxLayout(m_synthesisWidget);

    // 标题
    QLabel *titleLabel = new QLabel("物品合成");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #2E8B57; padding: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    synthesisLayout->addWidget(titleLabel);

    // 合成按钮网格
    QGridLayout *buttonGrid = new QGridLayout();

    // 阳光合成按钮 (第1行)
    QLabel *sunshineRowLabel = new QLabel("阳光合成:");
    sunshineRowLabel->setStyleSheet("font-weight: bold; color: #FF8C00; padding: 10px;");
    buttonGrid->addWidget(sunshineRowLabel, 0, 0);

    QStringList sunshineButtonNames = {"合成温暖阳光", "合成炽热阳光", "合成灿烂阳光", "合成神圣阳光"};
    QStringList sunshineButtonColors = {"#FFE4B5", "#FFA500", "#FF6347", "#FFD700"};
    for (int i = 0; i < 4; i++)
    {
        QPushButton *btn = new QPushButton(sunshineButtonNames[i]);
        btn->setStyleSheet(QString("QPushButton { background-color: %1; color: #333; padding: 10px; margin: 2px; border-radius: 6px; font-weight: bold; }")
                               .arg(sunshineButtonColors[i]));
        int recipeId = i + 1; // 配方ID 1-4
        connect(btn, &QPushButton::clicked, [this, recipeId]()
                { onSynthesisButtonClicked(recipeId); });
        buttonGrid->addWidget(btn, 0, i + 1);
    }

    // 矿石合成按钮 (第2行)
    QLabel *mineralRowLabel = new QLabel("矿石合成:");
    mineralRowLabel->setStyleSheet("font-weight: bold; color: #8B4513; padding: 10px;");
    buttonGrid->addWidget(mineralRowLabel, 1, 0);

    QStringList mineralButtonNames = {"合成普通矿石", "合成优质矿石", "合成稀有矿石", "合成传说矿石"};
    QStringList mineralButtonColors = {"#D2B48C", "#CD853F", "#A0522D", "#8B4513"};
    for (int i = 0; i < 4; i++)
    {
        QPushButton *btn = new QPushButton(mineralButtonNames[i]);
        btn->setStyleSheet(QString("QPushButton { background-color: %1; color: white; padding: 10px; margin: 2px; border-radius: 6px; font-weight: bold; }")
                               .arg(mineralButtonColors[i]));
        int recipeId = i + 5; // 配方ID 5-8
        connect(btn, &QPushButton::clicked, [this, recipeId]()
                { onSynthesisButtonClicked(recipeId); });
        buttonGrid->addWidget(btn, 1, i + 1);
    }

    // 木材合成按钮 (第3行)
    QLabel *woodRowLabel = new QLabel("木材合成:");
    woodRowLabel->setStyleSheet("font-weight: bold; color: #228B22; padding: 10px;");
    buttonGrid->addWidget(woodRowLabel, 2, 0);

    QStringList woodButtonNames = {"合成普通木材", "合成优质木材", "合成稀有木材", "合成神木"};
    QStringList woodButtonColors = {"#90EE90", "#32CD32", "#228B22", "#006400"};
    for (int i = 0; i < 4; i++)
    {
        QPushButton *btn = new QPushButton(woodButtonNames[i]);
        btn->setStyleSheet(QString("QPushButton { background-color: %1; color: white; padding: 10px; margin: 2px; border-radius: 6px; font-weight: bold; }")
                               .arg(woodButtonColors[i]));
        int recipeId = i + 9; // 配方ID 9-12
        connect(btn, &QPushButton::clicked, [this, recipeId]()
                { onSynthesisButtonClicked(recipeId); });
        buttonGrid->addWidget(btn, 2, i + 1);
    }

    synthesisLayout->addLayout(buttonGrid);

    // 设置背景样式
    m_synthesisWidget->setStyleSheet("QWidget { background-color: #f9f9f9; border: 2px solid #ddd; border-radius: 8px; margin: 5px; }");
}

void ForgePanel::connectSignals()
{
    // 解耦版本无需连接ViewModel信号
    // 数据更新通过updateForgeDisplayInfo接口进行
}

void ForgePanel::onSynthesisButtonClicked(int recipeId)
{
    // 执行合成操作
    ICommandBase *command = m_commandManager.get_command(CommandType::FORGE);
    if (command)
    {
        ForgeCommandParameter param(recipeId);
        command->exec(&param);
    }
    else
    {
        QMessageBox::warning(this, "错误", "无法找到锻造命令！");
    }
}

void ForgePanel::updateResourceDisplay()
{
    // 更新阳光材料显示 (ID 6-10)
    QStringList sunshineNames = {"微光阳光", "温暖阳光", "炽热阳光", "灿烂阳光", "神圣阳光"};
    for (int i = 0; i < m_sunshineLabels.size() && i < 5; ++i)
    {
        int itemId = 6 + i; // 阳光材料ID从6开始
        int count = m_displayInfo.materialCounts.value(itemId, 0);
        QString name = m_displayInfo.materialNames.value(itemId, sunshineNames[i]);
        
        m_sunshineLabels[i]->setText(QString("%1: %2").arg(name).arg(count));
        // 根据数量设置颜色
        if (count > 0) {
            m_sunshineLabels[i]->setStyleSheet("color: #FF8C00; font-weight: bold; padding: 3px;");
        } else {
            m_sunshineLabels[i]->setStyleSheet("color: #999; padding: 3px;");
        }
    }

    // 更新矿石材料显示 (ID 11-15)
    QStringList mineralNames = {"粗糙矿石", "普通矿石", "优质矿石", "稀有矿石", "传说矿石"};
    for (int i = 0; i < m_mineralLabels.size() && i < 5; ++i)
    {
        int itemId = 11 + i; // 矿石材料ID从11开始
        int count = m_displayInfo.materialCounts.value(itemId, 0);
        QString name = m_displayInfo.materialNames.value(itemId, mineralNames[i]);
        
        m_mineralLabels[i]->setText(QString("%1: %2").arg(name).arg(count));
        // 根据数量设置颜色
        if (count > 0) {
            m_mineralLabels[i]->setStyleSheet("color: #8B4513; font-weight: bold; padding: 3px;");
        } else {
            m_mineralLabels[i]->setStyleSheet("color: #999; padding: 3px;");
        }
    }

    // 更新木材材料显示 (ID 16-20)
    QStringList woodNames = {"枯木", "普通木材", "优质木材", "稀有木材", "神木"};
    for (int i = 0; i < m_woodLabels.size() && i < 5; ++i)
    {
        int itemId = 16 + i; // 木材材料ID从16开始
        int count = m_displayInfo.materialCounts.value(itemId, 0);
        QString name = m_displayInfo.materialNames.value(itemId, woodNames[i]);
        
        m_woodLabels[i]->setText(QString("%1: %2").arg(name).arg(count));
        // 根据数量设置颜色
        if (count > 0) {
            m_woodLabels[i]->setStyleSheet("color: #228B22; font-weight: bold; padding: 3px;");
        } else {
            m_woodLabels[i]->setStyleSheet("color: #999; padding: 3px;");
        }
    }
}
