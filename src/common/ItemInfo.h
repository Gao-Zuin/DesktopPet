#ifndef __ITEM_INFO_H__
#define __ITEM_INFO_H__

#include <QString>

struct ItemInfo
{
  int id{-1};
  QString name{"未知物品"};
  QString desc{"这是一个未知的物品"};
  QString iconPath;
};


#endif