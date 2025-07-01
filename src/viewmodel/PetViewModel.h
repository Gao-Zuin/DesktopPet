#ifndef __PET_VIEW_MODEL_H__
#define __PET_VIEW_MODEL_H__

#include "../model/PetModel.h"
#include "../common/PropertyTrigger.h"
#include "commands/MovePetCommand.h"
#include "commands/SwitchPetCommand.h"

class PetViewModel 
{
public:
    PetViewModel() noexcept;
    PetViewModel(const PetViewModel&) = delete;
    ~PetViewModel() noexcept
    {
    }

    PetViewModel& operator=(const PetViewModel&) = delete;
    // {
    //     if (m_sp_pet_model) {
    //         m_sp_pet_model->change_pet_type(type);
    //     }
    // }
    
    // virtual PetType getCurrentPetType() const override
    // {
    //     return m_sp_pet_model ? m_sp_pet_model->get_info()->petType : PetType::Spider;
    // }
    
    // virtual void setImageSize(const QSize &size) override
    // {
    //     if (m_sp_pet_model) {
    //         m_sp_pet_model->change_size(size);
    //     }
    // }
    
    // virtual QSize getImageSize() const override
    // {
    //     return m_sp_pet_model ? m_sp_pet_model->get_info()->size : QSize();
    // }
    
    // virtual void enterMovingMode() override
    // {
    //     m_isInMovingMode = true;
    // }
    
    // virtual void exitMovingMode() override
    // {
    //     m_isInMovingMode = false;
    // Properties
    const QPoint* get_position() const noexcept
    {
        return &(m_sp_pet_model->get_info()->position);
    }
    
    const QString* get_current_animation() const noexcept
    {
        return &(m_sp_pet_model->get_info()->currentAnimation);
    }
    
    const QSize* get_size() const noexcept
    {
        return &(m_sp_pet_model->get_info()->size);
    }

    // Commands
    ICommandBase* get_move_command() noexcept
    {
        return &m_move_command;
    }
    
    ICommandBase* get_switch_pet_command() noexcept
    {
        return &m_switch_pet_command;
    }

    // Trigger
    PropertyTrigger& get_trigger() noexcept
    {
        return m_trigger;
    }

    // Methods
    void set_pet_model(const std::shared_ptr<PetModel>& sp)
    {
        m_sp_pet_model = sp;
        m_sp_pet_model->get_trigger().add(&notification_cb, this);
    }

    std::shared_ptr<PetModel> get_pet_model() const noexcept
    {
        return m_sp_pet_model;
    }

private:
    // Notification
    static void notification_cb(uint32_t id, void *p);

private:
    // Model
    std::shared_ptr<PetModel> m_sp_pet_model;

    // Commands
    MovePetCommand m_move_command;
    SwitchPetCommand m_switch_pet_command;

    // Trigger
    PropertyTrigger m_trigger;
};

#endif