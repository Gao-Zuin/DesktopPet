#ifndef __PROPERTY_TRIGGER_H__
#define __PROPERTY_TRIGGER_H__

#include <vector>
#include <cstdint>
#include <cassert>
#include <algorithm>

typedef void (* PropertyNotification)(uint32_t, void *p);

class PropertyTrigger
{
public:
    // 类型别名
    using NotificationFunc = PropertyNotification;
    
private:
    struct _notification
    {
        PropertyNotification pn{nullptr};
        void *pv{nullptr};
        uintptr_t cookie{0};
    };

public:
    PropertyTrigger() noexcept
    {
    }
    PropertyTrigger(const PropertyTrigger&) = delete;
    ~PropertyTrigger() noexcept
    {
        clear();
    }

    PropertyTrigger& operator=(const PropertyTrigger&) = delete;

    void clear() noexcept
    {
        m_vec_nf.clear();
        m_next_cookie = 1;
    }

    // 改进：添加重复检查，返回cookie用于移除
    uintptr_t add(PropertyNotification pn, void *pv);
    
    // 改进：更安全的移除方法
    void remove(uintptr_t cookie) noexcept;
    
    // 改进：移除指定对象的所有回调
    void remove_by_object(void *pv) noexcept;
    
    // 改进：检查是否已注册
    bool contains(PropertyNotification pn, void *pv) const noexcept;

    void fire(uint32_t id);
    
    // 触发方法（不需要参数）
    void trigger();
    
    // 获取通知函数指针 - 用于回调注册
    NotificationFunc getNotification() const;

private:
    std::vector<_notification> m_vec_nf;
    uintptr_t m_next_cookie{1};
};

#endif
