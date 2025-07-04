#ifndef __PET_VIEW_MODEL_H__
#define __PET_VIEW_MODEL_H__

#include "../model/PetModel.h"
#include "../common/PropertyTrigger.h"
#include "../common/CommandManager.h"
#include "commands/MovePetCommand.h"
#include "commands/SwitchPetCommand.h"
#include "commands/ShowStatsPanelCommand.h"
#include "commands/ShowBackpackPanelCommand.h"
#include "commands/AddExperienceCommand.h"
#include "commands/AddMoneyCommand.h"

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
    
    // 经验值和等级相关属性
    int get_level() const noexcept
    {
        return m_sp_pet_model ? m_sp_pet_model->get_info()->level : 1;
    }
    
    int get_experience() const noexcept
    {
        return m_sp_pet_model ? m_sp_pet_model->get_info()->experience : 0;
    }
    
    int get_experience_to_next_level() const noexcept
    {
        return m_sp_pet_model ? m_sp_pet_model->get_info()->experienceToNextLevel : 100;
    }
    
    int get_money() const noexcept
    {
        return m_sp_pet_model ? m_sp_pet_model->get_info()->money : 0;
    }
    
    const QString* get_name() const noexcept
    {
        return m_sp_pet_model ? &(m_sp_pet_model->get_info()->name) : nullptr;
    }

    // Commands - 通过CommandManager统一管理
    CommandManager& get_command_manager() noexcept
    {
        return m_command_manager;
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
    
    // 经验值和金钱操作方法
    void add_experience(int exp) noexcept
    {
        if (m_sp_pet_model) {
            m_sp_pet_model->add_experience(exp);
        }
    }
    
    void add_money(int amount) noexcept
    {
        if (m_sp_pet_model) {
            m_sp_pet_model->add_money(amount);
        }
    }
    
    bool spend_money(int amount) noexcept
    {
        if (m_sp_pet_model && m_sp_pet_model->get_info()->money >= amount) {
            m_sp_pet_model->spend_money(amount);
            return true;
        }
        return false;
    }
    
    // 持久化方法
    void save_pet_data(const QString& filename = "pet_data.json") const
    {
        if (m_sp_pet_model) {
            m_sp_pet_model->save_to_file(filename);
        }
    }
    
    void load_pet_data(const QString& filename = "pet_data.json")
    {
        if (m_sp_pet_model) {
            m_sp_pet_model->load_from_file(filename);
        }
    }

private:
    // Notification
    static void notification_cb(uint32_t id, void *p);

private:
    // Model
    std::shared_ptr<PetModel> m_sp_pet_model;

    // Commands
    CommandManager m_command_manager;
    MovePetCommand m_move_command;
    SwitchPetCommand m_switch_pet_command;
    ShowStatsPanelCommand m_show_stats_panel_command;
    ShowBackpackPanelCommand m_show_backpack_panel_command;
    AddExperienceCommand m_add_experience_command;
    AddMoneyCommand m_add_money_command;

    // Trigger
    PropertyTrigger m_trigger;
};

#endif