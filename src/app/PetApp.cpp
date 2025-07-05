#include "PetApp.h"
#include "../common/PropertyIds.h"

PetApp::PetApp()
    : m_sp_pet_viewmodel(std::make_shared<PetViewModel>()),
      m_sp_pet_model(std::make_shared<PetModel>()),
      m_main_wnd(m_sp_pet_viewmodel->get_command_manager()),
      m_stats_panel(nullptr),
      m_backpack_panel(nullptr),
      m_collection_panel(nullptr),
      m_work_panel(nullptr)
{
}

bool PetApp::initialize()
{
    // Binding
    m_sp_pet_viewmodel->set_pet_model(m_sp_pet_model);

    // Properties
    m_main_wnd.set_position(m_sp_pet_viewmodel->get_position());
    m_main_wnd.set_animation(m_sp_pet_viewmodel->get_current_animation());
    m_main_wnd.set_size(m_sp_pet_viewmodel->get_size());

    // Notification - 注册主窗口的通知回调
    m_sp_pet_viewmodel->get_trigger().add(m_main_wnd.get_notification(), &m_main_wnd);

    // Notification - 注册应用级别的通知回调
    m_sp_pet_viewmodel->get_trigger().add(&PetApp::app_notification_cb, this);

    // 加载持久化数据
    m_sp_pet_viewmodel->load_pet_data();

    // Initialize model with default values if no saved data
    m_sp_pet_model->change_animation(":/resources/gif/spider.gif");

    // Update UI
    m_main_wnd.update_ui();

    return true;
}

void PetApp::app_notification_cb(uint32_t id, void *p)
{
    PetApp *pThis = static_cast<PetApp *>(p);

    switch (id)
    {
    case PROP_ID_SHOW_STATS_PANEL:
        pThis->show_stats_panel();
        break;
    case PROP_ID_SHOW_BACKPACK_PANEL:
        pThis->show_backpack_panel();
        break;
    case PROP_ID_SHOW_COLLECTION_PANEL:
        pThis->show_collection_panel();
        break;
    case PROP_ID_SHOW_WORK_PANEL:
        pThis->show_work_panel();
        break;
    case PROP_ID_PET_LEVEL:
    case PROP_ID_PET_EXPERIENCE:
    case PROP_ID_PET_MONEY:
    case PROP_ID_PET_TYPE: // 增加宠物类型变化的处理
        // 如果数值面板存在，更新其数据
        if (pThis->m_stats_panel)
        {
            pThis->m_stats_panel->set_level(pThis->m_sp_pet_viewmodel->get_level());
            pThis->m_stats_panel->set_experience(pThis->m_sp_pet_viewmodel->get_experience());
            pThis->m_stats_panel->set_experience_to_next_level(pThis->m_sp_pet_viewmodel->get_experience_to_next_level());
            pThis->m_stats_panel->set_money(pThis->m_sp_pet_viewmodel->get_money());

            const QString *name = pThis->m_sp_pet_viewmodel->get_name();
            if (name)
            {
                pThis->m_stats_panel->set_name(*name);
            }

            pThis->m_stats_panel->update_display();
        }
        break;
    default:
        break;
    }
}

void PetApp::show_stats_panel()
{
    // 如果面板已经存在，直接显示
    if (m_stats_panel)
    {
        m_stats_panel->show();
        m_stats_panel->raise();
        m_stats_panel->activateWindow();
        return;
    }

    // 创建新的数值面板
    m_stats_panel = new PetStatsPanel(m_sp_pet_viewmodel->get_command_manager());

    // 设置面板的初始数据
    m_stats_panel->set_level(m_sp_pet_viewmodel->get_level());
    m_stats_panel->set_experience(m_sp_pet_viewmodel->get_experience());
    m_stats_panel->set_experience_to_next_level(m_sp_pet_viewmodel->get_experience_to_next_level());
    m_stats_panel->set_money(m_sp_pet_viewmodel->get_money());

    const QString *name = m_sp_pet_viewmodel->get_name();
    if (name)
    {
        m_stats_panel->set_name(*name);
    }

    // 更新显示
    m_stats_panel->update_display();

    // 重要：为数值面板注册通知回调，确保数据变化时能及时更新
    m_sp_pet_viewmodel->get_trigger().add(m_stats_panel->get_notification(), m_stats_panel);

    // 显示面板
    m_stats_panel->show();
    m_stats_panel->raise();
    m_stats_panel->activateWindow();

    // 当面板关闭时，清理指针和回调
    QObject::connect(m_stats_panel, &QWidget::destroyed, [this]()
                     {
                         m_stats_panel = nullptr;
                         // 注意：当面板销毁时，PropertyTrigger会自动清理相关的回调
                     });
}

void PetApp::show_backpack_panel()
{
    // 如果面板已经存在，直接显示
    if (m_backpack_panel)
    {
        m_backpack_panel->show();
        m_backpack_panel->raise();
        m_backpack_panel->activateWindow();
        return;
    }

    // 创建新的背包面板 - 通过ViewModel访问
    m_backpack_panel = new BackpackPanel(m_sp_pet_viewmodel->get_command_manager(), *m_sp_pet_viewmodel);

    // 更新显示
    m_backpack_panel->updateDisplay();

    // 重要：为背包面板注册通知回调，确保数据变化时能及时更新
    m_sp_pet_viewmodel->get_backpack_trigger().add(m_backpack_panel->getNotification(), m_backpack_panel);

    // 显示面板
    m_backpack_panel->show();
    m_backpack_panel->raise();
    m_backpack_panel->activateWindow();

    // 当面板关闭时，清理指针和回调
    QObject::connect(m_backpack_panel, &QWidget::destroyed, [this]()
                     {
                         m_backpack_panel = nullptr;
                         // 注意：当面板销毁时，PropertyTrigger会自动清理相关的回调
                     });
}

void PetApp::show_collection_panel()
{
    // 如果面板已经存在，直接显示
    if (m_collection_panel)
    {
        m_collection_panel->show();
        m_collection_panel->raise();
        m_collection_panel->activateWindow();
        return;
    }

    // 创建新的图鉴面板 - 通过ViewModel访问
    m_collection_panel = new CollectionPanel(*m_sp_pet_viewmodel);

    // 显示面板
    m_collection_panel->show();
    m_collection_panel->raise();
    m_collection_panel->activateWindow();

    // 当面板关闭时，清理指针和回调
    QObject::connect(m_collection_panel, &QWidget::destroyed, [this]()
                     {
                         m_collection_panel = nullptr;
                         // 注意：当面板销毁时，PropertyTrigger会自动清理相关的回调
                     });
}

void PetApp::show_work_panel()
{
    // 如果面板已经存在，直接显示
    if (m_work_panel)
    {
        m_work_panel->show();
        m_work_panel->raise();
        m_work_panel->activateWindow();
        return;
    }

    // 创建新的打工面板 - 通过ViewModel访问
    m_work_panel = new WorkPanel(m_sp_pet_viewmodel->get_command_manager(), *m_sp_pet_viewmodel);

    // 更新显示
    m_work_panel->updateDisplay();

    // 重要：为打工面板注册通知回调，确保数据变化时能及时更新
    m_sp_pet_viewmodel->get_work_trigger().add(m_work_panel->getNotification(), m_work_panel);

    // 显示面板
    m_work_panel->show();
    m_work_panel->raise();
    m_work_panel->activateWindow();

    // 当面板关闭时，清理指针和回调
    QObject::connect(m_work_panel, &QWidget::destroyed, [this]()
                     {
                         m_work_panel = nullptr;
                         // 注意：当面板销毁时，PropertyTrigger会自动清理相关的回调
                     });
}