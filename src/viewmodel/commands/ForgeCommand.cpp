#include "ForgeCommand.h"
#include "../PetViewModel.h"
#include "../../model/ForgeModel.h"
#include <QDebug>

ForgeCommand::ForgeCommand(PetViewModel* viewModel)
    : m_view_model(viewModel)
{
}

ForgeCommand::~ForgeCommand()
{
}

int ForgeCommand::exec(ICommandParameter* param)
{
    ForgeCommandParameter* forgeParam = dynamic_cast<ForgeCommandParameter*>(param);
    if (!forgeParam || !m_view_model) {
        qDebug() << "ForgeCommand: Invalid parameters";
        return -1;
    }
    
    // 获取锻造模型
    ForgeModel* forgeModel = m_view_model->getForgeModel();
    if (!forgeModel) {
        qDebug() << "ForgeCommand: ForgeModel not available";
        return -1;
    }
    
    switch (forgeParam->action) {
    case ForgeCommandParameter::Action::ForgeItem:
        {
            qDebug() << "ForgeCommand: Forging item with recipe ID" << forgeParam->forgeRecipeId;
            bool success = forgeModel->forgeItem(forgeParam->forgeRecipeId);
            return success ? 0 : -1;
        }
        break;
        
    case ForgeCommandParameter::Action::ForgeItemWithCustomMaterials:
        {
            qDebug() << "ForgeCommand: Forging item with custom materials, recipe ID" << forgeParam->forgeRecipeId;
            bool success = forgeModel->forgeItemWithCustomMaterials(forgeParam->forgeRecipeId, forgeParam->customMaterials);
            return success ? 0 : -1;
        }
        break;
        
    case ForgeCommandParameter::Action::UpgradeWorkSystem:
        {
            qDebug() << "ForgeCommand: Upgrading work system" 
                     << static_cast<int>(forgeParam->workType)
                     << "to level" << static_cast<int>(forgeParam->targetLevel);
            bool success = forgeModel->upgradeWorkSystem(forgeParam->workType, forgeParam->targetLevel);
            return success ? 0 : -1;
        }
        break;
        
    case ForgeCommandParameter::Action::GetRecipes:
        {
            qDebug() << "ForgeCommand: Getting available recipes";
            auto recipes = forgeModel->getAvailableRecipes();
            qDebug() << "Found" << recipes.size() << "available recipes";
            return 0;
        }
        break;
        
    case ForgeCommandParameter::Action::GetMaterials:
        {
            qDebug() << "ForgeCommand: Getting material requirements for recipe" << forgeParam->forgeRecipeId;
            auto recipe = forgeModel->getRecipeById(forgeParam->forgeRecipeId);
            if (recipe.recipeId != 0) {
                qDebug() << "Recipe found:" << recipe.name;
                return 0;
            } else {
                qDebug() << "Recipe not found";
                return -1;
            }
        }
        break;
        
    default:
        qDebug() << "ForgeCommand: Unknown action";
        return -1;
    }
    
    return 0;
}
