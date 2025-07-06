#ifndef __BACKPACK_PANEL_H__
#define __BACKPACK_PANEL_H__

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QEnterEvent>
#include <QVector>
#include <QMap>
#include "../common/CommandManager.h"
#include "../common/PropertyTrigger.h"
#include "../common/base/BackpackItemInfo.h"

// 物品显示信息结构体 - View层需要的显示信息
struct ItemDisplayInfo
{
    QString name;
    QString iconPath;
    QString description;
    QString category;
    QString rarity;
    
    ItemDisplayInfo() {}
    ItemDisplayInfo(const QString& n, const QString& i, const QString& d, const QString& c, const QString& r)
        : name(n), iconPath(i), description(d), category(c), rarity(r) {}
};

// 物品格子类
class ItemSlot : public QWidget
{
    Q_OBJECT

public:
    explicit ItemSlot(QWidget* parent = nullptr);
    
    void setItem(const BackpackItemInfo& item, const ItemDisplayInfo& displayInfo);
    void clearItem();
    
    bool isEmpty() const { return m_itemId == 0; }
    int getItemId() const { return m_itemId; }
    int getItemCount() const { return m_itemCount; }

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void setupUi();
    void showDetailedTooltip();

private:
    QLabel* m_iconLabel;
    QLabel* m_countLabel;
    
    int m_itemId;
    int m_itemCount;
    QString m_itemName;
    QString m_itemDescription;
    QString m_itemCategory;
    QString m_itemRarity;
    
    static const int GRID_SIZE = 5;
};

// 背包面板类 - 完全解耦，不依赖任何ViewModel
class BackpackPanel : public QWidget
{
    Q_OBJECT

public:
    explicit BackpackPanel(CommandManager& commandManager, QWidget* parent = nullptr);
    ~BackpackPanel();

    // 通知回调接口
    PropertyNotification getNotification() const noexcept
    {
        return &notification_cb;
    }

    // 数据更新接口 - 由外部调用更新数据
    void updateBackpackData(const QVector<BackpackItemInfo>& items);
    void updateItemDisplayInfo(int itemId, const ItemDisplayInfo& displayInfo);
    void refreshDisplay(); // 批量更新后调用此方法刷新显示

private slots:
    void onSlotClicked(int index);

private:
    void setupUi();
    void updateDisplay();
    void updateSlots();
    
    // 静态通知回调函数
    static void notification_cb(uint32_t id, void* p);

private:
    CommandManager& m_commandManager;
    
    // UI组件
    QGridLayout* m_gridLayout;
    QLabel* m_statusLabel;
    QVector<ItemSlot*> m_slots;
    
    // 数据存储 - View层自己管理显示数据
    QVector<BackpackItemInfo> m_backpackItems;
    QMap<int, ItemDisplayInfo> m_itemDisplayInfos;
    
    static const int GRID_SIZE = 4;
};

#endif // __BACKPACK_PANEL_H__
