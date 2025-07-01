#ifndef __PET_APP_H__
#define __PET_APP_H__

#include "../model/PetModel.h"
#include "../viewmodel/PetViewModel.h"
#include "../view/PetMainWindow.h"
#include <memory>

class PetApp
{
public:
    PetApp();
    PetApp(const PetApp&) = delete;
    ~PetApp() noexcept
    {
    }

    PetApp& operator=(const PetApp&) = delete;

    bool initialize();

    void show_main_window()
    {
        m_main_wnd.show();
    }

private:
    PetMainWindow m_main_wnd;
    std::shared_ptr<PetViewModel> m_sp_pet_viewmodel;
    std::shared_ptr<PetModel> m_sp_pet_model;
};

#endif
