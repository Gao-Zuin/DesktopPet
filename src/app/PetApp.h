#ifndef __PET_APP_H__
#define __PET_APP_H__

#include "../model/PetModel.h"
#include "../model/BackpackModel.h"
#include "../model/CollectionModel.h"
#include "../viewmodel/PetViewModel.h"
#include "../view/PetMainWindow.h"
#include "../view/PetStatsPanel.h"
#include "../view/BackpackPanel.h"
#include "../view/CollectionPanel.h"
#include "../view/WorkPanel.h"
#include <memory>

class PetApp
{
public:
    PetApp();
    PetApp(const PetApp &) = delete;
    ~PetApp() noexcept
    {
        // 在应用程序关闭时保存数据
        if (m_sp_pet_viewmodel) {
            m_sp_pet_viewmodel->save_pet_data();
        }
        if (m_sp_backpack_model) {
            m_sp_backpack_model->saveToFile("backpack_data.json");
        }
        if (m_sp_collection_model) {
            m_sp_collection_model->saveToFile("collection_data.json");
        }
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
    void show_collection_panel();
    void show_work_panel();

private:
    std::shared_ptr<PetViewModel> m_sp_pet_viewmodel;
    std::shared_ptr<PetModel> m_sp_pet_model;
    std::shared_ptr<BackpackModel> m_sp_backpack_model;
    std::shared_ptr<CollectionModel> m_sp_collection_model;
    PetMainWindow m_main_wnd;
    PetStatsPanel *m_stats_panel;
    BackpackPanel *m_backpack_panel;
    CollectionPanel *m_collection_panel;
    WorkPanel *m_work_panel;
};

#endif
