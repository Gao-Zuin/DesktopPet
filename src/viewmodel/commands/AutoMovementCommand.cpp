#include "AutoMovementCommand.h"
#include "../PetViewModel.h"
#include <QDebug>

int AutoMovementCommand::exec(ICommandParameter *p)
{
    AutoMovementCommandParameter *param = dynamic_cast<AutoMovementCommandParameter*>(p);
    if (!param || !m_pvm) {
        qDebug() << "AutoMovementCommand: Invalid parameters";
        return -1;
    }
    
    // 获取自动移动模型
    AutoMovementModel* autoMovementModel = m_pvm->getAutoMovementModel();
    if (!autoMovementModel) {
        qDebug() << "AutoMovementCommand: AutoMovementModel not available";
        return -1;
    }
    
    switch (param->action) {
    case AutoMovementCommandParameter::Action::Start:
        qDebug() << "AutoMovementCommand: Starting auto movement";
        autoMovementModel->startAutoMovement();
        break;
        
    case AutoMovementCommandParameter::Action::Stop:
        qDebug() << "AutoMovementCommand: Stopping auto movement";
        autoMovementModel->stopAutoMovement();
        break;
        
    case AutoMovementCommandParameter::Action::SetMode:
        qDebug() << "AutoMovementCommand: Setting movement mode to" 
                 << static_cast<int>(param->movementMode);
        // 先设置模式配置，然后根据模式启动或停止
        {
            AutoMovementConfig config;
            config.mode = param->movementMode;
            config.speed = 10;
            config.updateInterval = 50;
            config.enableRandomPause = false;
            config.pauseProbability = 0;
            config.pauseDuration = 0;
            config.enableIconInteraction = true;
            config.iconInteractionProbability = 50;
            config.iconDetectionRadius = 50;
            config.interactionAnimation = ":/resources/gif/kicking.gif";
            config.interactionDuration = 1000;
            
            autoMovementModel->setConfig(config);
            
            if (param->movementMode == AutoMovementMode::RandomMovement) {
                autoMovementModel->startAutoMovement();
            } else {
                autoMovementModel->stopAutoMovement();
            }
        }
        break;
        
    default:
        qDebug() << "AutoMovementCommand: Unknown action";
        return -1;
    }
    
    return 0;
}
