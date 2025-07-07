#include "PetApp.h"
#include "../common/PropertyIds.h"
#include "../view/ForgePanel.h"
#include <QObject>
#include <QDebug>
#include <QMetaObject>

// 解决Windows头文件冲突问题
#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#endif

PetApp::PetApp()
    : m_sp_pet_viewmodel(std::make_shared<PetViewModel>()),
      m_sp_pet_model(std::make_shared<PetModel>()),
      m_main_wnd(m_sp_pet_viewmodel->get_command_manager()),
      m_stats_panel(nullptr),
      m_backpack_panel(nullptr),
      m_collection_panel(nullptr),
      m_work_panel(nullptr),
      m_forge_panel(nullptr),
      m_work_upgrade_panel(nullptr) // 初始化工作升级面板指针
{
}

bool PetApp::initialize()
{
    // Binding
    m_sp_pet_viewmodel->set_pet_model(m_sp_pet_model);

    // Properties
    m_main_wnd.set_position(m_sp_pet_viewmodel->get_position());
    m_main_wnd.set_animation(m_sp_pet_viewmodel->get_current_animation());
    m_main_wnd.set_size(m_sp_pet_viewmodel->get_size());

    // Notification - 注册主窗口的通知回调
    m_sp_pet_viewmodel->get_trigger().add(m_main_wnd.get_notification(), &m_main_wnd);

    // Notification - 注册应用级别的通知回调
    m_sp_pet_viewmodel->get_trigger().add(&PetApp::app_notification_cb, this);

    // 加载持久化数据
    m_sp_pet_viewmodel->load_pet_data();

    // Initialize model with default values if no saved data
    m_sp_pet_model->change_animation(":/resources/gif/spider.gif");

    // Update UI
    m_main_wnd.update_ui();

    return true;
}

void PetApp::app_notification_cb(uint32_t id, void *p)
{
    PetApp *pThis = static_cast<PetApp *>(p);

    switch (id)
    {
    case PROP_ID_SHOW_STATS_PANEL:
        pThis->show_stats_panel();
        break;
    case PROP_ID_SHOW_BACKPACK_PANEL:
        pThis->show_backpack_panel();
        break;
    case PROP_ID_SHOW_COLLECTION_PANEL:
        pThis->show_collection_panel();
        break;
    case PROP_ID_SHOW_WORK_PANEL:
        pThis->show_work_panel();
        break;
    case PROP_ID_SHOW_FORGE_PANEL:
        pThis->show_forge_panel();
        break;
    case PROP_ID_SHOW_WORK_UPGRADE_PANEL:  // 添加工作升级面板处理
        pThis->show_work_upgrade_panel();
        break;
    case PROP_ID_BACKPACK_UPDATE:
        // 当背包数据更新时，更新背包面板数据
        if (pThis->m_backpack_panel)
        {
            pThis->updateBackpackPanelData();
        }
        // 背包数据更新时也需要更新锻造面板数据
        if (pThis->m_forge_panel)
        {
            pThis->updateForgePanelData();
        }
        break;
    case PROP_ID_COLLECTION_UPDATE:
        // 当图鉴数据更新时，更新图鉴面板数据
        if (pThis->m_collection_panel)
        {
            pThis->updateCollectionPanelData();
        }
        break;
    case PROP_ID_WORK_STATUS_UPDATE:
        // 当工作状态更新时，更新工作面板数据
        if (pThis->m_work_panel)
        {
            pThis->updateWorkPanelData();
        }
        break;
    case PROP_ID_FORGE_UPDATE:
        // 当锻造数据更新时，更新工作升级面板数据
        if (pThis->m_work_upgrade_panel)
        {
            pThis->updateWorkUpgradePanelData();
        }
        break;
    case PROP_ID_PET_LEVEL:
    case PROP_ID_PET_EXPERIENCE:
    case PROP_ID_PET_MONEY:
    case PROP_ID_PET_TYPE: // 增加宠物类型变化的处理
        // 如果数值面板存在，更新其数据
        if (pThis->m_stats_panel)
        {
            pThis->m_stats_panel->set_level(pThis->m_sp_pet_viewmodel->get_level());
            pThis->m_stats_panel->set_experience(pThis->m_sp_pet_viewmodel->get_experience());
            pThis->m_stats_panel->set_experience_to_next_level(pThis->m_sp_pet_viewmodel->get_experience_to_next_level());
            pThis->m_stats_panel->set_money(pThis->m_sp_pet_viewmodel->get_money());

            const QString *name = pThis->m_sp_pet_viewmodel->get_name();
            if (name)
            {
                pThis->m_stats_panel->set_name(*name);
            }

            pThis->m_stats_panel->update_display();
        }
        break;
    default:
        break;
    }
}

void PetApp::show_stats_panel()
{
    // 如果面板已经存在，直接显示
    if (m_stats_panel)
    {
        m_stats_panel->show();
        m_stats_panel->raise();
        m_stats_panel->activateWindow();
        return;
    }

    // 创建新的数值面板
    m_stats_panel = new PetStatsPanel(m_sp_pet_viewmodel->get_command_manager());

    // 设置面板的初始数据
    m_stats_panel->set_level(m_sp_pet_viewmodel->get_level());
    m_stats_panel->set_experience(m_sp_pet_viewmodel->get_experience());
    m_stats_panel->set_experience_to_next_level(m_sp_pet_viewmodel->get_experience_to_next_level());
    m_stats_panel->set_money(m_sp_pet_viewmodel->get_money());

    const QString *name = m_sp_pet_viewmodel->get_name();
    if (name)
    {
        m_stats_panel->set_name(*name);
    }

    // 更新显示
    m_stats_panel->update_display();

    // 重要：为数值面板注册通知回调（改进：避免重复注册）
    uintptr_t cookie = m_sp_pet_viewmodel->get_trigger().add(m_stats_panel->get_notification(), m_stats_panel);

    // 显示面板
    m_stats_panel->show();
    m_stats_panel->raise();
    m_stats_panel->activateWindow();

    // 当面板关闭时，清理指针和回调（改进：主动清理回调）
    QObject::connect(m_stats_panel, &QWidget::destroyed, [this, cookie]()
                     {
                         // 主动移除回调，避免悬空指针
                         m_sp_pet_viewmodel->get_trigger().remove(cookie);
                         m_stats_panel = nullptr; });
}

void PetApp::show_backpack_panel()
{
    // 如果面板已经存在，直接显示
    if (m_backpack_panel)
    {
        m_backpack_panel->show();
        m_backpack_panel->raise();
        m_backpack_panel->activateWindow();
        return;
    }

    // 创建新的背包面板 - 解耦后只需要CommandManager
    m_backpack_panel = new BackpackPanel(m_sp_pet_viewmodel->get_command_manager());

    // 初始化背包数据
    updateBackpackPanelData();

    // 重要：为背包面板注册通知回调（改进：使用cookie机制）
    uintptr_t cookie = m_sp_pet_viewmodel->get_backpack_trigger().add(m_backpack_panel->getNotification(), m_backpack_panel);

    // 显示面板
    m_backpack_panel->show();
    m_backpack_panel->raise();
    m_backpack_panel->activateWindow();

    // 当面板关闭时，清理指针和回调（改进：主动清理回调）
    QObject::connect(m_backpack_panel, &QWidget::destroyed, [this, cookie]()
                     {
                         // 主动移除回调，避免悬空指针
                         m_sp_pet_viewmodel->get_backpack_trigger().remove(cookie);
                         m_backpack_panel = nullptr; });
}

void PetApp::show_collection_panel()
{
    // 如果面板已经存在，直接显示
    if (m_collection_panel)
    {
        m_collection_panel->show();
        m_collection_panel->raise();
        m_collection_panel->activateWindow();
        return;
    }

    // 创建新的图鉴面板 - 解耦后只需要CommandManager
    m_collection_panel = new CollectionPanel(m_sp_pet_viewmodel->get_command_manager());

    // 初始化图鉴数据
    updateCollectionPanelData();

    // 重要：为图鉴面板注册通知回调（改进：使用cookie机制）
    uintptr_t cookie = m_sp_pet_viewmodel->get_collection_trigger().add(&CollectionPanel::collection_notification_cb, m_collection_panel);

    // 显示面板
    m_collection_panel->show();
    m_collection_panel->raise();
    m_collection_panel->activateWindow();

    // 当面板关闭时，清理指针和回调（改进：主动清理回调）
    QObject::connect(m_collection_panel, &QWidget::destroyed, [this, cookie]()
                     {
                         // 主动移除回调，避免悬空指针
                         m_sp_pet_viewmodel->get_collection_trigger().remove(cookie);
                         m_collection_panel = nullptr; });
}

void PetApp::show_work_panel()
{
    // 如果面板已经存在，直接显示
    if (m_work_panel)
    {
        m_work_panel->show();
        m_work_panel->raise();
        m_work_panel->activateWindow();
        return;
    }

    // 创建新的打工面板 - 解耦后不再依赖ViewModel
    m_work_panel = new WorkPanel(m_sp_pet_viewmodel->get_command_manager());

    // 初始化工作类型数据
    updateWorkPanelData();

    // 重要：为打工面板注册通知回调（改进：使用cookie机制）
    uintptr_t cookie = m_sp_pet_viewmodel->get_work_trigger().add(m_work_panel->getNotification(), m_work_panel);

    // 显示面板
    m_work_panel->show();
    m_work_panel->raise();
    m_work_panel->activateWindow();

    // 当面板关闭时，清理指针和回调（改进：主动清理回调）
    QObject::connect(m_work_panel, &QWidget::destroyed, [this, cookie]()
                     {
                         // 主动移除回调，避免悬空指针
                         m_sp_pet_viewmodel->get_work_trigger().remove(cookie);
                         m_work_panel = nullptr; });
}

void PetApp::show_forge_panel()
{
    qDebug() << "PetApp::show_forge_panel() called";

    // 如果面板已经存在，直接显示
    if (m_forge_panel)
    {
        m_forge_panel->show();
        m_forge_panel->raise();
        m_forge_panel->activateWindow();
        return;
    }

    // 创建新的锻造面板 - 解耦后只需要CommandManager
    m_forge_panel = new ForgePanel(m_sp_pet_viewmodel->get_command_manager());

    // 初始化锻造面板数据
    updateForgePanelData();

    // 重要：为锻造面板注册通知回调（改进：使用cookie机制）
    uintptr_t cookie = m_sp_pet_viewmodel->get_backpack_trigger().add(&ForgePanel::forge_notification_cb, m_forge_panel);

    // 显示面板
    m_forge_panel->show();
    m_forge_panel->raise();
    m_forge_panel->activateWindow();

    // 当面板关闭时，清理指针和回调（改进：主动清理回调）
    QObject::connect(m_forge_panel, &QWidget::destroyed, [this, cookie]()
                     {
                         // 主动移除回调，避免悬空指针
                         m_sp_pet_viewmodel->get_backpack_trigger().remove(cookie);
                         m_forge_panel = nullptr; });

    qDebug() << "ForgePanel created and displayed";
}

void PetApp::show_work_upgrade_panel()
{
    qDebug() << "PetApp::show_work_upgrade_panel() called";

    // 如果面板已经存在，直接显示
    if (m_work_upgrade_panel)
    {
        m_work_upgrade_panel->show();
        m_work_upgrade_panel->raise();
        m_work_upgrade_panel->activateWindow();
        return;
    }

    // 创建新的工作升级面板 - 解耦后不再依赖ViewModel
    m_work_upgrade_panel = new WorkUpgradePanel(m_sp_pet_viewmodel->get_command_manager());
    m_work_upgrade_panel->setWindowTitle("打工系统升级");
    m_work_upgrade_panel->resize(900, 700);

    // 初始化工作升级面板数据
    updateWorkUpgradePanelData();

    // 设置窗口标志，确保关闭时不会退出应用程序
    m_work_upgrade_panel->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    m_work_upgrade_panel->setAttribute(Qt::WA_DeleteOnClose, false); // 关闭时不删除，由PetApp管理

    // 重要：为工作升级面板注册通知回调
    uintptr_t cookie = m_sp_pet_viewmodel->get_forge_trigger().add(m_work_upgrade_panel->getNotification(), m_work_upgrade_panel);

    // 显示面板
    m_work_upgrade_panel->show();
    m_work_upgrade_panel->raise();
    m_work_upgrade_panel->activateWindow();

    // 当面板关闭时，清理指针和回调
    QObject::connect(m_work_upgrade_panel, &QWidget::destroyed, [this, cookie]()
                     { 
                         // 主动移除回调，避免悬空指针
                         m_sp_pet_viewmodel->get_forge_trigger().remove(cookie);
                         m_work_upgrade_panel = nullptr; });

    qDebug() << "WorkUpgradePanel created and displayed";
}

// 静态实例指针定义
PetApp* s_appInstance = nullptr;

PetApp* PetApp::getInstance()
{
    return s_appInstance;
}

void PetApp::showWorkUpgradePanel()
{
    show_work_upgrade_panel();
}

void PetApp::updateBackpackPanelData()
{
    if (!m_backpack_panel || !m_sp_pet_viewmodel)
    {
        return;
    }

    // 从ViewModel获取背包数据
    const QVector<BackpackItemInfo> &backpackItems = m_sp_pet_viewmodel->get_backpack_items();

    // 更新背包面板的数据（不立即刷新显示）
    m_backpack_panel->updateBackpackData(backpackItems);

    // 批量更新物品显示信息
    auto collectionModel = m_sp_pet_viewmodel->get_collection_model();
    if (collectionModel)
    {
        for (const auto &item : backpackItems)
        {
            CollectionItemInfo itemInfo = collectionModel->getItemInfo(item.itemId);
            if (itemInfo.id != 0)
            {
                ItemDisplayInfo displayInfo;
                displayInfo.name = itemInfo.name;
                displayInfo.iconPath = itemInfo.iconPath;
                displayInfo.description = itemInfo.description;

                // 转换类别和稀有度为中文
                switch (static_cast<CollectionCategory>(itemInfo.category))
                {
                case CollectionCategory::Material:
                    displayInfo.category = "材料";
                    break;
                case CollectionCategory::Item:
                    displayInfo.category = "物品";
                    break;
                case CollectionCategory::Skin:
                    displayInfo.category = "皮肤";
                    break;
                case CollectionCategory::Achievement:
                    displayInfo.category = "成就";
                    break;
                default:
                    displayInfo.category = "未知";
                    break;
                }

                switch (static_cast<CollectionRarity>(itemInfo.rarity))
                {
                case CollectionRarity::Common:
                    displayInfo.rarity = "普通";
                    break;
                case CollectionRarity::Rare:
                    displayInfo.rarity = "稀有";
                    break;
                case CollectionRarity::Epic:
                    displayInfo.rarity = "史诗";
                    break;
                case CollectionRarity::Legendary:
                    displayInfo.rarity = "传说";
                    break;
                default:
                    displayInfo.rarity = "未知";
                    break;
                }

                // 只更新数据，不立即刷新显示
                m_backpack_panel->updateItemDisplayInfo(item.itemId, displayInfo);
            }
        }
    }

    // 所有数据更新完毕后，统一刷新显示
    m_backpack_panel->refreshDisplay();
}

void PetApp::updateCollectionPanelData()
{
    if (!m_collection_panel || !m_sp_pet_viewmodel)
    {
        return;
    }

    // 从ViewModel获取图鉴数据
    auto collectionModel = m_sp_pet_viewmodel->get_collection_model();
    if (!collectionModel)
    {
        return;
    }

    CollectionDisplayData data;
    data.items = m_sp_pet_viewmodel->get_all_collection_items();
    data.totalItems = data.items.size();

    // 计算已拥有的物品数量
    data.ownedItems = 0;
    for (const auto &item : data.items)
    {
        if (item.status == CollectionStatus::Collected)
        {
            data.ownedItems++;
        }
    }

    // 统计各类别和稀有度数量
    data.categoryStats.clear();
    data.rarityStats.clear();
    for (const auto &item : data.items)
    {
        data.categoryStats[item.category]++;
        data.rarityStats[item.rarity]++;
    }

    // 更新图鉴面板的数据
    m_collection_panel->updateCollectionData(data);

    // 所有数据更新完毕后，统一刷新显示
    m_collection_panel->refreshDisplay();
}

void PetApp::updateWorkPanelData()
{
    if (!m_work_panel || !m_sp_pet_viewmodel)
    {
        return;
    }

    // 从ViewModel获取工作类型数据
    const QVector<WorkInfo> &workTypes = m_sp_pet_viewmodel->get_work_types();
    
    // 更新工作面板的工作类型数据
    m_work_panel->updateWorkTypes(workTypes);

    // 获取当前工作状态
    WorkStatus status = m_sp_pet_viewmodel->get_current_work_status();
    WorkType currentType = m_sp_pet_viewmodel->get_current_work_type();
    int remainingTime = m_sp_pet_viewmodel->get_work_remaining_time();

    // 构建工作状态信息
    WorkStatusInfo statusInfo(status, currentType, remainingTime, "");
    
    // 更新工作面板的状态信息
    m_work_panel->updateWorkStatus(statusInfo);

    // 所有数据更新完毕后，统一刷新显示
    m_work_panel->refreshDisplay();
}

void PetApp::updateWorkUpgradePanelData()
{
    if (!m_work_upgrade_panel || !m_sp_pet_viewmodel)
    {
        return;
    }

    // 从ViewModel获取相关数据
    auto workModel = m_sp_pet_viewmodel->get_work_model();
    auto forgeModel = m_sp_pet_viewmodel->getForgeModel();
    auto backpackModel = m_sp_pet_viewmodel->get_backpack_model();
    
    if (!workModel || !forgeModel || !backpackModel)
    {
        return;
    }

    // 构建工作系统显示信息
    QVector<WorkSystemDisplayInfo> workSystemsInfo;
    QVector<WorkType> workTypes = {WorkType::Photosynthesis, WorkType::Mining, WorkType::Adventure};

    for (WorkType workType : workTypes)
    {
        WorkSystemDisplayInfo info;
        info.workType = workType;
        info.currentLevel = workModel->getWorkSystemLevel(workType);
        
        // 设置工作类型名称
        switch (workType)
        {
        case WorkType::Photosynthesis:
            info.workTypeName = "🌱 光合作用";
            break;
        case WorkType::Mining:
            info.workTypeName = "⛏️ 挖矿";
            break;
        case WorkType::Adventure:
            info.workTypeName = "🌲 冒险探索";
            break;
        }
        
        // 设置等级名称
        auto getLevelName = [](WorkSystemLevel level) -> QString {
            switch (level)
            {
            case WorkSystemLevel::Basic:
                return "基础级";
            case WorkSystemLevel::Advanced:
                return "进阶级";
            case WorkSystemLevel::Expert:
                return "专家级";
            case WorkSystemLevel::Master:
                return "大师级";
            default:
                return "未知等级";
            }
        };
        
        info.currentLevelName = getLevelName(info.currentLevel);
        
        WorkSystemLevel nextLevel = static_cast<WorkSystemLevel>(static_cast<int>(info.currentLevel) + 1);
        info.nextLevelName = getLevelName(nextLevel);
        
        // 获取升级材料需求
        auto availableUpgrades = forgeModel->getAvailableWorkUpgrades();
        for (const auto &upgrade : availableUpgrades)
        {
            if (upgrade.workType == workType)
            {
                info.upgradeMaterials = upgrade.upgradeMaterials;
                break;
            }
        }
        
        // 获取拥有的材料数量和名称
        for (const auto &material : info.upgradeMaterials)
        {
            int ownedCount = backpackModel->getItemCount(material.itemId);
            QString itemName = backpackModel->getItemName(material.itemId);
            
            info.ownedMaterials[material.itemId] = ownedCount;
            info.materialNames[material.itemId] = itemName;
        }
        
        // 检查是否可以升级
        info.canUpgrade = forgeModel->canUpgradeWorkSystem(workType);
        
        workSystemsInfo.append(info);
    }

    // 更新工作升级面板的数据
    m_work_upgrade_panel->updateWorkSystemDisplayInfo(workSystemsInfo);

    // 所有数据更新完毕后，统一刷新显示
    m_work_upgrade_panel->refreshDisplay();
}

void PetApp::updateForgePanelData()
{
    if (!m_forge_panel)
    {
        return;
    }

    // 组装锻造面板显示信息
    ForgeDisplayInfo info;
    
    qDebug() << "PetApp::updateForgePanelData: 开始更新锻造面板数据";
    
    if (m_sp_pet_viewmodel && m_sp_pet_viewmodel->get_backpack_model())
    {
        auto backpackModel = m_sp_pet_viewmodel->get_backpack_model();
        qDebug() << "PetApp::updateForgePanelData: 获取到背包模型";
        
        // 获取材料物品的数量和名称
        // 阳光材料 (ID 6-10)
        QStringList sunshineNames = {"微光阳光", "温暖阳光", "炽热阳光", "灿烂阳光", "神圣阳光"};
        for (int i = 0; i < 5; ++i)
        {
            int itemId = 6 + i;
            int count = backpackModel->getItemCount(itemId);
            QString name = backpackModel->getItemName(itemId);
            
            info.materialCounts[itemId] = count;
            info.materialNames[itemId] = name.isEmpty() ? sunshineNames[i] : name;
            
            qDebug() << "PetApp::updateForgePanelData: 阳光材料" << itemId << "名称:" << info.materialNames[itemId] << "数量:" << count;
        }
        
        // 矿石材料 (ID 11-15)
        QStringList mineralNames = {"粗糙矿石", "普通矿石", "优质矿石", "稀有矿石", "传说矿石"};
        for (int i = 0; i < 5; ++i)
        {
            int itemId = 11 + i;
            int count = backpackModel->getItemCount(itemId);
            QString name = backpackModel->getItemName(itemId);
            
            info.materialCounts[itemId] = count;
            info.materialNames[itemId] = name.isEmpty() ? mineralNames[i] : name;
            
            qDebug() << "PetApp::updateForgePanelData: 矿石材料" << itemId << "名称:" << info.materialNames[itemId] << "数量:" << count;
        }
        
        // 木材材料 (ID 16-20)
        QStringList woodNames = {"枯木", "普通木材", "优质木材", "稀有木材", "神木"};
        for (int i = 0; i < 5; ++i)
        {
            int itemId = 16 + i;
            int count = backpackModel->getItemCount(itemId);
            QString name = backpackModel->getItemName(itemId);
            
            info.materialCounts[itemId] = count;
            info.materialNames[itemId] = name.isEmpty() ? woodNames[i] : name;
            
            qDebug() << "PetApp::updateForgePanelData: 木材材料" << itemId << "名称:" << info.materialNames[itemId] << "数量:" << count;
        }
        
        qDebug() << "PetApp::updateForgePanelData: 总共准备了" << info.materialCounts.size() << "种材料数据";
    }
    else
    {
        qDebug() << "PetApp::updateForgePanelData: ViewModel或背包模型为空";
    }
    
    // 更新锻造面板的数据
    qDebug() << "PetApp::updateForgePanelData: 调用ForgePanel::updateForgeDisplayInfo";
    m_forge_panel->updateForgeDisplayInfo(info);
    qDebug() << "PetApp::updateForgePanelData: 锻造面板数据更新完成";
}

