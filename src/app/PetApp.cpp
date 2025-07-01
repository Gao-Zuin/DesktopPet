#include "PetApp.h"

PetApp::PetApp() 
    : m_sp_pet_viewmodel(std::make_shared<PetViewModel>())
    , m_sp_pet_model(std::make_shared<PetModel>())
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
    
    // Commands
    m_main_wnd.set_move_command(m_sp_pet_viewmodel->get_move_command());
    m_main_wnd.set_switch_pet_command(m_sp_pet_viewmodel->get_switch_pet_command());
    
    // Notification
    m_sp_pet_viewmodel->get_trigger().add(m_main_wnd.get_notification(), &m_main_wnd);

    // Initialize model with default values
    m_sp_pet_model->change_animation(":/resources/gif/spider.gif");
    
    // Update UI
    m_main_wnd.update_ui();

    return true;
}
