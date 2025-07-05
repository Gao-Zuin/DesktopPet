#ifndef COLLECTION_PANEL_H
#define COLLECTION_PANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QProgressBar>
#include <QMap>
#include <QVector>
#include "../model/base/CollectionInfo.h"
#include "../view/CollectionItemWidget.h"

// 前向声明
class PetViewModel;

class CollectionPanel : public QWidget
{
    Q_OBJECT

public:
    explicit CollectionPanel(PetViewModel &viewModel, QWidget *parent = nullptr);
    ~CollectionPanel();

private slots:
    void onCategoryChanged(int category);
    void onRarityFilterChanged(int rarity);
    void onStatusFilterChanged(int status);
    void onSearchTextChanged(const QString &text);
    void onItemClicked(int itemId);
    void onCollectionUpdated();
    void onRefreshClicked();

private:
    void setupUi();
    void updateDisplay();
    void updateItemGrid();
    void updateStatistics();
    void applyFilters();
    void showItemDetail(int itemId);

    // 静态回调函数
    static void collection_notification_cb(uint32_t id, void *p);

    // 辅助函数
    QString getCategoryName(CollectionCategory category) const;
    QString getRarityName(CollectionRarity rarity) const;
    QString getStatusName(CollectionStatus status) const;

    PetViewModel &m_viewModel;

    // UI组件
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_controlLayout;
    QGridLayout *m_itemGridLayout;
    QScrollArea *m_scrollArea;
    QWidget *m_itemGridWidget;

    // 控制组件
    QComboBox *m_categoryCombo;
    QComboBox *m_rarityCombo;
    QComboBox *m_statusCombo;
    QLineEdit *m_searchEdit;
    QPushButton *m_refreshButton;

    // 统计组件
    QLabel *m_statsLabel;
    QProgressBar *m_progressBar;

    // 物品控件
    QVector<CollectionItemWidget *> m_itemWidgets;

    // 过滤器状态
    CollectionCategory m_currentCategory;
    CollectionRarity m_currentRarity;
    CollectionStatus m_currentStatus;
    QString m_searchKeyword;

    static const int GRID_COLUMNS = 6;
    static const int ITEM_SIZE = 80;
};

#endif // COLLECTION_PANEL_H
