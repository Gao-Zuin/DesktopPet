#ifndef __PET_APP_H__
#define __PET_APP_H__

#include "../model/PetModel.h"
#include "../model/BackpackModel.h"
#include "../viewmodel/PetViewModel.h"
#include "../view/PetMainWindow.h"
#include "../view/PetStatsPanel.h"
#include "../view/BackpackPanel.h"
#include <memory>

class PetApp
{
public:
    PetApp();
    PetApp(const PetApp &) = delete;
    ~PetApp() noexcept
    {
    }

    PetApp &operator=(const PetApp &) = delete;

    bool initialize();

    void show_main_window()
    {
        m_main_wnd.show();
    }

private:
    // Notification callback for app-level operations
    static void app_notification_cb(uint32_t id, void *p);

    void show_stats_panel();
    void show_backpack_panel();

private:
    std::shared_ptr<PetViewModel> m_sp_pet_viewmodel;
    std::shared_ptr<PetModel> m_sp_pet_model;
    std::shared_ptr<BackpackModel> m_sp_backpack_model;
    PetMainWindow m_main_wnd;
    PetStatsPanel *m_stats_panel;
    BackpackPanel *m_backpack_panel;
};

#endif
