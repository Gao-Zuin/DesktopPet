#include "PropertyTrigger.h"

uintptr_t PropertyTrigger::add(PropertyNotification pn, void *pv)
{
    _notification nf;
    nf.pn = pn;
    nf.pv = pv;
    m_vec_nf.push_back(nf);
    return m_vec_nf.size();
}

void PropertyTrigger::fire(uint32_t id)
{
    for ( auto &nf : m_vec_nf ) {
        if ( nf.pn != nullptr )
            nf.pn(id, nf.pv);
    }
}
