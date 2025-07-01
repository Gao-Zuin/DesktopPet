#ifndef __PROPERTY_TRIGGER_H__
#define __PROPERTY_TRIGGER_H__

#include <vector>
#include <cstdint>
#include <cassert>

typedef void (* PropertyNotification)(uint32_t, void *p);

class PropertyTrigger
{
private:
    struct _notification
    {
        PropertyNotification pn{nullptr};
        void *pv{nullptr};
    };

public:
    PropertyTrigger() noexcept
    {
    }
    PropertyTrigger(const PropertyTrigger&) = delete;
    ~PropertyTrigger() noexcept
    {
    }

    PropertyTrigger& operator=(const PropertyTrigger&) = delete;

    void clear() noexcept
    {
        m_vec_nf.clear();
    }

    uintptr_t add(PropertyNotification pn, void *pv);
    void remove(uintptr_t cookie) noexcept
    {
        assert( cookie > 0 && cookie <= m_vec_nf.size() );
        m_vec_nf[cookie - 1].pn = nullptr;
    }

    void fire(uint32_t id);

private:
    std::vector<_notification> m_vec_nf;
};

#endif
