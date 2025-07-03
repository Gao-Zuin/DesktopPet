#include "PetViewModel.h"
#include "../common/PropertyIds.h"

PetViewModel::PetViewModel() noexcept 
    : m_move_command(this)
    , m_switch_pet_command(this)
    , m_show_stats_panel_command(m_trigger)
    , m_add_experience_command(this)
    , m_add_money_command(this)
{
    // 注册所有命令到CommandManager
    m_command_manager.register_command(CommandType::MOVE_PET, &m_move_command);
    m_command_manager.register_command(CommandType::SWITCH_PET, &m_switch_pet_command);
    m_command_manager.register_command(CommandType::SHOW_STATS_PANEL, &m_show_stats_panel_command);
    m_command_manager.register_command(CommandType::ADD_EXPERIENCE, &m_add_experience_command);
    m_command_manager.register_command(CommandType::ADD_MONEY, &m_add_money_command);
}

void PetViewModel::notification_cb(uint32_t id, void *p)
{
    PetViewModel* pThis = (PetViewModel*)p;

    switch (id) {
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
    default:
        break;
    }

    pThis->m_trigger.fire(id);
}
