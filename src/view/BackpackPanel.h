#ifndef BACKPACKPANEL_H
#define BACKPACKPANEL_H

#include <QWidget>
#include <QGridLayout>
#include <QVector>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include "../common/CommandBase.h"
#include "../common/CommandManager.h"
#include "../common/PropertyTrigger.h"
#include "../common/ItemInfo.h"
#include "../model/base/BackpackItemInfo.h"  // 只包含数据结构，不包含Model类

// 前向声明
class PetViewModel;

// 背包格子控件
class ItemSlot : public QWidget
{
    Q_OBJECT
public:
    explicit ItemSlot(QWidget *parent = nullptr);
    void setItem(const BackpackItemInfo &item, const QString &name, const QString &iconPath);
    void clearItem();
    bool isEmpty() const { return m_itemId == 0; }
    int itemId() const { return m_itemId; }

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    void setupUi();

    QLabel *m_iconLabel;  // 物品图标
    QLabel *m_countLabel; // 数量标签
    int m_itemId;         // 物品ID
    int m_itemCount;      // 物品数量
};

// 背包主面板
class BackpackPanel : public QWidget
{
    Q_OBJECT

public:
    explicit BackpackPanel(CommandManager &command_manager, PetViewModel &view_model, QWidget *parent = nullptr);
    ~BackpackPanel();

    // 更新背包显示
    void updateDisplay();

    // 通知回调
    PropertyNotification getNotification() const noexcept
    {
        return &notification_cb;
    }

private slots:
    void onSlotClicked(int index);

private:
    void setupUi();
    void updateSlots();

    // 根据物品ID获取物品信息
    void getItemInfo(int itemId, QString &name, QString &iconPath) const;

    // 通知回调
    static void notification_cb(uint32_t id, void *p);
    void loadItemInfoFromCSV(const QString &filePath);

private:
    static const int GRID_SIZE = 4; // 4x4网格
    QMap<int, ItemInfo> m_itemInfos;

    // 模型和命令管理器（按声明顺序初始化）
    CommandManager &m_command_manager;
    PetViewModel &m_view_model;  // 改为引用ViewModel而不是BackpackModel

    // UI组件
    QGridLayout *m_gridLayout;
    QVector<ItemSlot *> m_slots; // 所有格子

    // 操作按钮
    QPushButton *m_sortButton;
    QPushButton *m_clearButton;

    // 状态标签
    QLabel *m_statusLabel;
};

#endif // BACKPACKPANEL_H