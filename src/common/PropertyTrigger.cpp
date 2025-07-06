#include "PropertyTrigger.h"

uintptr_t PropertyTrigger::add(PropertyNotification pn, void *pv)
{
    // 检查是否已经注册过相同的回调
    if (contains(pn, pv)) {
        // 如果已存在，返回现有的cookie
        for (const auto &nf : m_vec_nf) {
            if (nf.pn == pn && nf.pv == pv) {
                return nf.cookie;
            }
        }
    }
    
    _notification nf;
    nf.pn = pn;
    nf.pv = pv;
    nf.cookie = m_next_cookie++;
    m_vec_nf.push_back(nf);
    return nf.cookie;
}

void PropertyTrigger::remove(uintptr_t cookie) noexcept
{
    m_vec_nf.erase(
        std::remove_if(m_vec_nf.begin(), m_vec_nf.end(),
                      [cookie](const _notification& nf) {
                          return nf.cookie == cookie;
                      }),
        m_vec_nf.end());
}

void PropertyTrigger::remove_by_object(void *pv) noexcept
{
    m_vec_nf.erase(
        std::remove_if(m_vec_nf.begin(), m_vec_nf.end(),
                      [pv](const _notification& nf) {
                          return nf.pv == pv;
                      }),
        m_vec_nf.end());
}

bool PropertyTrigger::contains(PropertyNotification pn, void *pv) const noexcept
{
    return std::any_of(m_vec_nf.begin(), m_vec_nf.end(),
                      [pn, pv](const _notification& nf) {
                          return nf.pn == pn && nf.pv == pv;
                      });
}

void PropertyTrigger::fire(uint32_t id)
{
    // 创建回调列表的副本，避免在回调过程中修改原列表导致的问题
    auto callbacks = m_vec_nf;
    
    for (const auto &nf : callbacks) {
        if (nf.pn != nullptr) {
            try {
                nf.pn(id, nf.pv);
            } catch (...) {
                // 捕获回调中的异常，防止整个应用崩溃
                // 在实际项目中应该记录日志
            }
        }
    }
}

void PropertyTrigger::trigger()
{
    // 无参数触发 - 使用默认ID 0
    fire(0);
}

PropertyTrigger::NotificationFunc PropertyTrigger::getNotification() const
{
    // 返回一个静态函数指针，用于回调注册
    // 这个方法通常不会被直接使用，因为每个PropertyTrigger都有自己的回调列表
    // 这里返回nullptr，实际的回调机制通过add/remove方法管理
    return nullptr;
}
