#include "PetViewModel.h"
#include "../common/PropertyIds.h"

PetViewModel::PetViewModel() noexcept 
    : m_move_command(this)
    , m_switch_pet_command(this)
{
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
    default:
        break;
    }

    pThis->m_trigger.fire(id);
}
