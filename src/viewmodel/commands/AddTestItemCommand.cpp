#include "AddTestItemCommand.h"
#include "../PetViewModel.h"
#include <QDebug>

int AddTestItemCommand::exec(ICommandParameter* p)
{
    Q_UNUSED(p)
    
    // 添加一些测试物品到背包（这会自动解锁图鉴）
    static int testCounter = 1;
    
    // 循环添加不同的物品
    int itemIds[] = {1, 2, 3, 51, 52, 101, 102, 103, 151, 152, 201, 202, 301, 302};
    int numItems = sizeof(itemIds) / sizeof(itemIds[0]);
    
    int itemId = itemIds[testCounter % numItems];
    
    m_pvm->add_backpack_item(itemId, 1);
    
    qDebug() << "测试添加物品到背包:" << itemId;
    
    testCounter++;
    
    return 0;
}
