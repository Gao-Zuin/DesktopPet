#ifndef __PET_MODEL_H__
#define __PET_MODEL_H__

#include "../common/PropertyTrigger.h"
#include "base/PetInfo.h"
#include <iostream>
#include <string>
#include <QMap>
#include "../common/EventMgr.h"
#include "../common/EventDefine.h"

class PetModel : public EventListener<TestEvent>, public EventListener<AddExperienceEvent>
{
public:
    //响应事件的函数
    void OnEvent(TestEvent event) override;
    void OnEvent(AddExperienceEvent event) override;

    PetModel() noexcept;
    PetModel(const PetModel &) = delete;
    ~PetModel() noexcept;

    PetModel &operator=(const PetModel &) = delete;

    const PetInfo *get_info() const noexcept
    {
        return &m_current_info;
    }

    PropertyTrigger &get_trigger() noexcept
    {
        return m_trigger;
    }

    // Methods
    void change_position(const QPoint &position) noexcept;
    void change_state(PetState state) noexcept;
    void change_animation(const QString &animation) noexcept;
    void change_visibility(bool visible) noexcept;
    void change_size(const QSize &size) noexcept;
    void change_pet_type(PetType type) noexcept;

    // 经验值和等级系统
    void add_experience(int exp) noexcept;
    void set_level(int level) noexcept;
    void add_money(int amount) noexcept;
    void spend_money(int amount) noexcept;

    // 持久化方法
    void save_to_file(const QString &filename) const;
    void load_from_file(const QString &filename);

private:
    // 等级计算辅助方法
    int calculate_experience_needed(int level) const noexcept;

    // 保存当前宠物数据到对应的类型数据中
    void save_current_pet_data() noexcept;

    // 从对应类型数据中加载到当前宠物数据
    void load_pet_data_for_type(PetType type) noexcept;
    void check_level_up() noexcept;

private:
    PetInfo m_current_info;            // 当前显示的宠物信息
    QMap<PetType, PetInfo> m_pet_data; // 每个宠物类型的独立数据
    PropertyTrigger m_trigger;
};

#endif