#ifndef __PET_MODEL_H__
#define __PET_MODEL_H__

#include "../common/PropertyTrigger.h"
#include "base/PetInfo.h"

class PetModel
{
public:
    PetModel() noexcept
    {
    }
    PetModel(const PetModel&) = delete;
    ~PetModel() noexcept
    {
    }

    PetModel& operator=(const PetModel&) = delete;

    const PetInfo *get_info() const noexcept
    {
        return &m_info;
    }

    PropertyTrigger& get_trigger() noexcept
    {
        return m_trigger;
    }

    // Methods
    void change_position(const QPoint& position) noexcept;
    void change_state(PetState state) noexcept;
    void change_animation(const QString& animation) noexcept;
    void change_visibility(bool visible) noexcept;
    void change_size(const QSize& size) noexcept;
    void change_pet_type(PetType type) noexcept;

private:
    PetInfo m_info;
    PropertyTrigger m_trigger;
};

#endif