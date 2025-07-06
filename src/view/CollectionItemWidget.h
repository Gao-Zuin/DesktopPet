#ifndef COLLECTION_ITEM_WIDGET_H
#define COLLECTION_ITEM_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QPixmap>
#include <QColor>
#include <QFont>
#include <QDebug>
#include "../common/base/CollectionInfo.h"

class CollectionItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CollectionItemWidget(const CollectionItemInfo &info, QWidget *parent = nullptr);
    void updateInfo(const CollectionItemInfo &info);
    
    static const int WIDGET_SIZE = 80;

signals:
    void clicked(int itemId);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void drawBackground(QPainter &painter);
    void drawIcon(QPainter &painter);
    void drawRarityBorder(QPainter &painter);
    void drawStatusOverlay(QPainter &painter);
    void drawCountText(QPainter &painter);
    void drawTooltip(QPainter &painter);
    
    QColor getRarityColor(CollectionRarity rarity) const;
    QPixmap getStatusIcon(CollectionStatus status) const;
    
    CollectionItemInfo m_info;
    bool m_isHovered;
    QPixmap m_iconPixmap;
    QPixmap m_cachedPixmap;
    bool m_needsRepaint;
};

#endif // COLLECTION_ITEM_WIDGET_H
