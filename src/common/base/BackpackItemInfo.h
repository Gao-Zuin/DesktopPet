#ifndef BACKPACK_ITEM_INFO_H
#define BACKPACK_ITEM_INFO_H

// 背包物品信息结构
struct BackpackItemInfo {
    int itemId;         // 物品唯一ID
    int count;          // 物品数量
    
    BackpackItemInfo(int id = 0, int cnt = 0) 
        : itemId(id), count(cnt) {}
};

#endif // BACKPACK_ITEM_INFO_H