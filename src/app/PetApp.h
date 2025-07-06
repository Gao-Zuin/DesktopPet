#ifndef __PET_APP_H__
#define __PET_APP_H__

#include "../viewmodel/PetViewModel.h"
#include "../view/PetMainWindow.h"
#include "../view/PetStatsPanel.h"
#include "../view/BackpackPanel.h"
#include "../view/CollectionPanel.h"
#include "../view/WorkPanel.h"
#include "../view/ForgePanel.h"
#include <memory>

class PetApp
{
public:
    PetApp();
    PetApp(const PetApp &) = delete;
    ~PetApp() noexcept
    {
        // 在应用程序关闭时保存数据 - 通过ViewModel保存
        if (m_sp_pet_viewmodel) {
            m_sp_pet_viewmodel->save_pet_data();
            // 通过ViewModel保存其他数据
            auto backpackModel = m_sp_pet_viewmodel->get_backpack_model();
            if (backpackModel) {
                backpackModel->saveToFile("backpack_data.json");
            }
            auto collectionModel = m_sp_pet_viewmodel->get_collection_model();
            if (collectionModel) {
                collectionModel->saveToFile("collection_data.json");
            }
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
    void show_forge_panel();
    
    // 数据更新方法 - 用于向解耦的View层传递数据
    void updateBackpackPanelData();

private:
    std::shared_ptr<PetViewModel> m_sp_pet_viewmodel;
    std::shared_ptr<PetModel> m_sp_pet_model;  // 保留，因为需要与ViewModel绑定
    PetMainWindow m_main_wnd;
    PetStatsPanel *m_stats_panel;
    BackpackPanel *m_backpack_panel;
    CollectionPanel *m_collection_panel;
    WorkPanel *m_work_panel;
    ForgePanel *m_forge_panel;  // 添加锻造面板成员
};

#endif
