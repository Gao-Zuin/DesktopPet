#ifndef __PET_VIEW_MODEL_H__
#define __PET_VIEW_MODEL_H__

#include "../model/PetModel.h"
#include "../model/WorkModel.h"
#include "../model/BackpackModel.h"
#include "../model/CollectionModel.h"
#include "../model/AutoMovementModel.h"
#include "../common/PropertyTrigger.h"
#include "../common/CommandManager.h"
#include "../common/EventMgr.h"
#include "../common/EventDefine.h"
#include "commands/MovePetCommand.h"
#include "commands/SwitchPetCommand.h"
#include "commands/ShowStatsPanelCommand.h"
#include "commands/ShowBackpackPanelCommand.h"
#include "commands/ShowCollectionPanelCommand.h"
#include "commands/ShowWorkPanelCommand.h"
#include "commands/StartWorkCommand.h"
#include "commands/StopWorkCommand.h"
#include "commands/AddExperienceCommand.h"
#include "commands/AddMoneyCommand.h"
#include "commands/AutoMovementCommand.h"
#include "../common/EventMgr.h"
#include "../common/EventDefine.h"

class PetViewModel : public EventListener<AddItemEvent>
{
public:
    // 响应事件的函数
    void OnEvent(AddItemEvent event) override;
    PetViewModel() noexcept;
    PetViewModel(const PetViewModel &) = delete;
    ~PetViewModel() noexcept
    {
        // 注销事件监听器
        EventMgr::GetInstance().UnregisterEvent<AddItemEvent>(this);
    }

    PetViewModel &operator=(const PetViewModel &) = delete;
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
    const QPoint *get_position() const noexcept
    {
        return &(m_sp_pet_model->get_info()->position);
    }

    const QString *get_current_animation() const noexcept
    {
        return &(m_sp_pet_model->get_info()->currentAnimation);
    }

    const QSize *get_size() const noexcept
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

    const QString *get_name() const noexcept
    {
        return m_sp_pet_model ? &(m_sp_pet_model->get_info()->name) : nullptr;
    }

    // Commands - 通过CommandManager统一管理
    CommandManager &get_command_manager() noexcept
    {
        return m_command_manager;
    }

    // Trigger
    PropertyTrigger &get_trigger() noexcept
    {
        return m_trigger;
    }

    // Methods
    void set_pet_model(const std::shared_ptr<PetModel> &sp)
    {
        m_sp_pet_model = sp;
        m_sp_pet_model->get_trigger().add(&notification_cb, this);
        
        // 将宠物模型绑定到自动移动模型
        if (m_sp_auto_movement_model) {
            m_sp_auto_movement_model->setPetModel(m_sp_pet_model);
        }
    }

    std::shared_ptr<PetModel> get_pet_model() const noexcept
    {
        return m_sp_pet_model;
    }

    // 获取工作模型
    WorkModel *get_work_model() const noexcept
    {
        return m_sp_work_model.get();
    }

    // 获取背包模型 - 新增
    BackpackModel *get_backpack_model() const noexcept
    {
        return m_sp_backpack_model.get();
    }

    // 获取图鉴模型 - 新增
    CollectionModel *get_collection_model() const noexcept
    {
        return m_sp_collection_model.get();
    }

    // 经验值和金钱操作方法
    void add_experience(int exp) noexcept
    {
        if (m_sp_pet_model)
        {
            m_sp_pet_model->add_experience(exp);
        }
    }

    void add_money(int amount) noexcept
    {
        if (m_sp_pet_model)
        {
            m_sp_pet_model->add_money(amount);
        }
    }

    bool spend_money(int amount) noexcept
    {
        if (m_sp_pet_model && m_sp_pet_model->get_info()->money >= amount)
        {
            m_sp_pet_model->spend_money(amount);
            return true;
        }
        return false;
    }

    // 背包操作方法 - 通过ViewModel封装
    const QVector<BackpackItemInfo> &get_backpack_items() const noexcept
    {
        static QVector<BackpackItemInfo> empty;
        return m_sp_backpack_model ? m_sp_backpack_model->getItems() : empty;
    }

    void add_backpack_item(int itemId, int count = 1) noexcept
    {
        if (m_sp_backpack_model)
        {
            m_sp_backpack_model->addItem(itemId, count);
        }
    }

    void remove_backpack_item(int itemId, int count = 1) noexcept
    {
        if (m_sp_backpack_model)
        {
            m_sp_backpack_model->removeItem(itemId, count);
        }
    }

    int get_backpack_item_count(int itemId) const noexcept
    {
        return m_sp_backpack_model ? m_sp_backpack_model->getItemCount(itemId) : 0;
    }

    PropertyTrigger &get_backpack_trigger() noexcept
    {
        static PropertyTrigger empty_trigger;
        return m_sp_backpack_model ? m_sp_backpack_model->get_trigger() : empty_trigger;
    }

    // 图鉴操作方法 - 通过ViewModel封装
    QVector<CollectionItemInfo> get_collection_items_by_category(CollectionCategory category) const
    {
        return m_sp_collection_model ? m_sp_collection_model->getItemsByCategory(category) : QVector<CollectionItemInfo>();
    }

    QVector<CollectionItemInfo> get_all_collection_items() const
    {
        return m_sp_collection_model ? m_sp_collection_model->getAllItems() : QVector<CollectionItemInfo>();
    }

    bool unlock_collection_item(int itemId)
    {
        return m_sp_collection_model ? m_sp_collection_model->unlockItem(itemId) : false;
    }

    bool collect_item(int itemId, int count = 1)
    {
        return m_sp_collection_model ? m_sp_collection_model->collectItem(itemId, count) : false;
    }

    CollectionItemInfo get_collection_item_info(int itemId) const
    {
        return m_sp_collection_model ? m_sp_collection_model->getItemInfo(itemId) : CollectionItemInfo();
    }

    PropertyTrigger &get_collection_trigger() noexcept
    {
        static PropertyTrigger empty_trigger;
        return m_sp_collection_model ? m_sp_collection_model->get_trigger() : empty_trigger;
    }

    // 打工操作方法 - 通过ViewModel封装
    const QVector<WorkInfo> &get_work_types() const noexcept
    {
        static QVector<WorkInfo> empty;
        return m_sp_work_model ? m_sp_work_model->getWorkTypes() : empty;
    }

    WorkStatus get_current_work_status() const noexcept
    {
        return m_sp_work_model ? m_sp_work_model->getCurrentStatus() : WorkStatus::Idle;
    }

    WorkType get_current_work_type() const noexcept
    {
        return m_sp_work_model ? m_sp_work_model->getCurrentWorkType() : WorkType::Photosynthesis;
    }

    int get_work_remaining_time() const noexcept
    {
        return m_sp_work_model ? m_sp_work_model->getRemainingTime() : 0;
    }

    bool is_continuous_work_mode() const noexcept
    {
        return m_sp_work_model ? m_sp_work_model->isContinuousMode() : false;
    }

    PropertyTrigger &get_work_trigger() noexcept
    {
        static PropertyTrigger empty_trigger;
        return m_sp_work_model ? m_sp_work_model->get_trigger() : empty_trigger;
    }
    
    // 自动移动操作方法
    AutoMovementModel* getAutoMovementModel() const noexcept
    {
        return m_sp_auto_movement_model.get();
    }
    
    bool isAutoMovementActive() const noexcept
    {
        return m_sp_auto_movement_model ? m_sp_auto_movement_model->isAutoMovementActive() : false;
    }
    
    AutoMovementMode getCurrentAutoMovementMode() const noexcept
    {
        return m_sp_auto_movement_model ? m_sp_auto_movement_model->getCurrentMode() : AutoMovementMode::Disabled;
    }
    
    PropertyTrigger& getAutoMovementTrigger() noexcept
    {
        static PropertyTrigger empty_trigger;
        return m_sp_auto_movement_model ? m_sp_auto_movement_model->getTrigger() : empty_trigger;
    }

    // 持久化方法
    void save_pet_data(const QString &filename = "pet_data.json") const
    {
        if (m_sp_pet_model)
        {
            m_sp_pet_model->save_to_file(filename);
        }
    }

    void load_pet_data(const QString &filename = "pet_data.json")
    {
        if (m_sp_pet_model)
        {
            m_sp_pet_model->load_from_file(filename);
        }
    }

private:
    // Notification
    static void notification_cb(uint32_t id, void *p);

private:
    // Model
    std::shared_ptr<PetModel> m_sp_pet_model;
    std::shared_ptr<WorkModel> m_sp_work_model;
    std::shared_ptr<BackpackModel> m_sp_backpack_model;
    std::shared_ptr<CollectionModel> m_sp_collection_model;
    std::shared_ptr<AutoMovementModel> m_sp_auto_movement_model;

    // Commands
    CommandManager m_command_manager;
    MovePetCommand m_move_command;
    SwitchPetCommand m_switch_pet_command;
    ShowStatsPanelCommand m_show_stats_panel_command;
    ShowBackpackPanelCommand m_show_backpack_panel_command;
    ShowCollectionPanelCommand m_show_collection_panel_command;
    ShowWorkPanelCommand m_show_work_panel_command;
    StartWorkCommand m_start_work_command;
    StopWorkCommand m_stop_work_command;
    AddExperienceCommand m_add_experience_command;
    AddMoneyCommand m_add_money_command;
    AutoMovementCommand m_auto_movement_command;

    // Trigger
    PropertyTrigger m_trigger;
};

#endif