#include "PetViewModel.h"
#include "../common/PropertyIds.h"
#include "../common/CollectionManager.h"

PetViewModel::PetViewModel() noexcept
    : m_sp_work_model(std::make_shared<WorkModel>()),
      m_sp_backpack_model(std::make_shared<BackpackModel>()),
      m_sp_collection_model(std::make_shared<CollectionModel>()),
      m_sp_auto_movement_model(std::make_shared<AutoMovementModel>()),
      m_sp_forge_model(std::make_shared<ForgeModel>()),
      m_move_command(this),
      m_switch_pet_command(this),
      m_show_stats_panel_command(m_trigger),
      m_show_backpack_panel_command(m_trigger),
      m_show_collection_panel_command(this),
      m_show_work_panel_command(m_trigger),
      m_start_work_command(this),
      m_stop_work_command(this),
      m_add_experience_command(this),
      m_add_money_command(this),
      m_auto_movement_command(this),
      m_forge_command(this),
      m_show_forge_panel_command(m_trigger),
      m_show_work_upgrade_panel_command()  // 修复构造函数参数
{
    // 注册事件监听器
    EventMgr::GetInstance().RegisterEvent<AddItemEvent>(this);

    // 初始化图鉴系统
    if (m_sp_collection_model)
    {
        // 加载图鉴物品配置
        m_sp_collection_model->loadItemsFromCSV(":/resources/csv/collection_items.csv");
        // 加载已保存的图鉴数据
        m_sp_collection_model->loadFromFile("collection_data.json");

        // 设置CollectionManager
        CollectionManager::getInstance().setCollectionModel(m_sp_collection_model);
    }

    // 初始化背包系统（在图鉴系统初始化后）
    if (m_sp_backpack_model)
    {
        // 先加载已保存的背包数据
        m_sp_backpack_model->loadFromFile("backpack_data.json");

        // 如果没有保存的数据，则从图鉴系统初始化
        if (m_sp_backpack_model->getItems().isEmpty())
        {
            m_sp_backpack_model->initializeFromCollection();
        }

        // 同步背包数据到图鉴系统
        CollectionManager::getInstance().syncFromBackpack(m_sp_backpack_model->getItems());
    }

    // 初始化锻造系统
    if (m_sp_forge_model) {
        // 设置锻造系统的依赖关系
        m_sp_forge_model->setBackpackModel(m_sp_backpack_model);
        m_sp_forge_model->setCollectionModel(m_sp_collection_model);
        m_sp_forge_model->setWorkModel(m_sp_work_model);
        
        // 加载锻造数据
        m_sp_forge_model->loadFromFile("forge_data.json");
    }

    // 注册所有命令到CommandManager
    m_command_manager.register_command(CommandType::MOVE_PET, &m_move_command);
    m_command_manager.register_command(CommandType::SWITCH_PET, &m_switch_pet_command);
    m_command_manager.register_command(CommandType::SHOW_STATS_PANEL, &m_show_stats_panel_command);
    m_command_manager.register_command(CommandType::SHOW_BACKPACK_PANEL, &m_show_backpack_panel_command);
    m_command_manager.register_command(CommandType::SHOW_COLLECTION_PANEL, &m_show_collection_panel_command);
    m_command_manager.register_command(CommandType::SHOW_WORK_PANEL, &m_show_work_panel_command);
    m_command_manager.register_command(CommandType::START_WORK, &m_start_work_command);
    m_command_manager.register_command(CommandType::STOP_WORK, &m_stop_work_command);
    m_command_manager.register_command(CommandType::ADD_EXPERIENCE, &m_add_experience_command);
    m_command_manager.register_command(CommandType::ADD_MONEY, &m_add_money_command);
    m_command_manager.register_command(CommandType::AUTO_MOVEMENT, &m_auto_movement_command);
    m_command_manager.register_command(CommandType::FORGE, &m_forge_command);
    m_command_manager.register_command(CommandType::SHOW_FORGE_PANEL, &m_show_forge_panel_command);
    m_command_manager.register_command(CommandType::SHOW_WORK_UPGRADE_PANEL, &m_show_work_upgrade_panel_command);  // 注册工作升级面板命令
}

void PetViewModel::OnEvent(AddItemEvent event)
{
    // 处理添加物品到背包事件
    qDebug() << "[PetViewModel] 收到添加物品事件: 物品ID=" << event.itemId << "数量=" << event.count;

    // 通过ViewModel的add_backpack_item方法添加物品到背包
    // 这会自动触发背包更新和图鉴解锁
    add_backpack_item(event.itemId, event.count);

    qDebug() << "[PetViewModel] 物品已添加到背包并自动解锁图鉴";
}


void PetViewModel::notification_cb(uint32_t id, void *p)
{
    PetViewModel *pThis = (PetViewModel *)p;

    switch (id)
    {
    case PROP_ID_PET_POSITION:
        // 位置变化，可能需要更新UI
        break;
    case PROP_ID_PET_ANIMATION:
        // 动画变化
        break;
    case PROP_ID_PET_SIZE:
        // 尺寸变化
        break;
    case PROP_ID_PET_TYPE:
        // 宠物类型变化
        break;
    case PROP_ID_PET_LEVEL:
        // 等级变化
        break;
    case PROP_ID_PET_EXPERIENCE:
        // 经验值变化
        break;
    case PROP_ID_PET_MONEY:
        // 金钱变化
        break;
    case PROP_ID_SHOW_FORGE_PANEL:
        // 显示锻造面板
        pThis->showForgePanel();
        break;
    default:
        break;
    }

    pThis->m_trigger.fire(id);
}

void PetViewModel::showForgePanel()
{
    qDebug() << "PetViewModel::showForgePanel() called";
    
    // 解耦后，ViewModel不再直接创建View，而是通过PropertyTrigger通知PetApp
    // PetApp会监听PROP_ID_SHOW_FORGE_PANEL事件并创建ForgePanel
    qDebug() << "ForgePanel show request sent via PropertyTrigger";
}
