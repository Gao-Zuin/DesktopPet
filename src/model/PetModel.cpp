#include "PetModel.h"
#include "../common/PropertyIds.h"

void PetModel::change_position(const QPoint& position) noexcept
{
    // 只有当位置变化超过阈值时才更新，减少频繁通知
    QPoint diff = position - m_info.position;
    if (abs(diff.x()) > 2 || abs(diff.y()) > 2) {
        m_info.position = position;
        m_trigger.fire(PROP_ID_PET_POSITION);
    }
}

void PetModel::change_state(PetState state) noexcept
{
    if (m_info.state != state) {
        m_info.state = state;
        m_trigger.fire(PROP_ID_PET_STATE);
    }
}

void PetModel::change_animation(const QString& animation) noexcept
{
    if (m_info.currentAnimation != animation) {
        m_info.currentAnimation = animation;
        m_trigger.fire(PROP_ID_PET_ANIMATION);
    }
}

void PetModel::change_visibility(bool visible) noexcept
{
    if (m_info.isVisible != visible) {
        m_info.isVisible = visible;
        m_trigger.fire(PROP_ID_PET_VISIBLE);
    }
}

void PetModel::change_size(const QSize& size) noexcept
{
    if (m_info.size != size) {
        m_info.size = size;
        m_trigger.fire(PROP_ID_PET_SIZE);
    }
}

void PetModel::change_pet_type(PetType type) noexcept
{
    if (m_info.petType != type) {
        m_info.petType = type;
        m_trigger.fire(PROP_ID_PET_TYPE);
    }
}
