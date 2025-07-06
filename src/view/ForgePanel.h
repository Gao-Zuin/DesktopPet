#ifndef __FORGE_PANEL_H__
#define __FORGE_PANEL_H__

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QCloseEvent>
#include <QVector>
#include <QMap>
#include <QString>
#include "../common/CommandManager.h"
#include "../common/PropertyTrigger.h"

// 锻造面板显示信息结构体 - View层需要的显示信息
struct ForgeDisplayInfo
{
    QMap<int, int> materialCounts; // itemId -> count
    QMap<int, QString> materialNames; // itemId -> name
    
    ForgeDisplayInfo() {}
};

// 锻造面板 - 解耦版，不依赖ViewModel
class ForgePanel : public QWidget
{
    Q_OBJECT

public:
    explicit ForgePanel(CommandManager &commandManager, QWidget *parent = nullptr);
    ~ForgePanel();

    // 数据更新接口 - 批量更新显示信息
    void updateForgeDisplayInfo(const ForgeDisplayInfo &info);
    
    // 静态通知回调函数 - 用于PropertyTrigger注册
    static void forge_notification_cb(uint32_t prop_id, void *pv);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onSynthesisButtonClicked(int recipeId);

private:
    void setupUI();
    void setupResourceDisplay();
    void setupSynthesisButtons();
    void updateResourceDisplay();
    void connectSignals();

private:
    CommandManager &m_commandManager;
    
    // 显示信息
    ForgeDisplayInfo m_displayInfo;

    // UI组件
    QWidget *m_resourceDisplayWidget;
    QWidget *m_synthesisWidget;

    // 资源显示标签
    QVector<QLabel *> m_sunshineLabels; // 阳光材料标签 (5个)
    QVector<QLabel *> m_mineralLabels;  // 矿石材料标签 (5个)
    QVector<QLabel *> m_woodLabels;     // 木材材料标签 (5个)
};

#endif // __FORGE_PANEL_H__
