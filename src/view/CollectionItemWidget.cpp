#include "CollectionItemWidget.h"
#include <QPainterPath>
#include <QToolTip>
#include <QEnterEvent>

CollectionItemWidget::CollectionItemWidget(const CollectionItemInfo &info, QWidget *parent)
    : QWidget(parent)
    , m_info(info)
    , m_isHovered(false)
    , m_needsRepaint(true)
{
    setFixedSize(WIDGET_SIZE, WIDGET_SIZE);
    setMouseTracking(true);
    
    // 加载图标
    if (!m_info.iconPath.isEmpty()) {
        m_iconPixmap = QPixmap(m_info.iconPath);
        if (m_iconPixmap.isNull()) {
            qDebug() << "无法加载图标:" << m_info.iconPath;
        }
    }
    
    // 设置工具提示
    updateInfo(info);
}

void CollectionItemWidget::updateInfo(const CollectionItemInfo &info)
{
    m_info = info;
    m_needsRepaint = true;
    
    // 重新加载图标
    if (!m_info.iconPath.isEmpty()) {
        m_iconPixmap = QPixmap(m_info.iconPath);
    }
    
    // 更新工具提示
    QString tooltipText = QString("%1\n%2\n稀有度: %3\n状态: %4")
                         .arg(m_info.name)
                         .arg(m_info.description)
                         .arg(getRarityName(m_info.rarity))
                         .arg(getStatusName(m_info.status));
    
    if (m_info.status == CollectionStatus::Collected && m_info.totalObtained > 0) {
        tooltipText += QString("\n获得数量: %1").arg(m_info.totalObtained);
    }
    
    setToolTip(tooltipText);
    update();
}

void CollectionItemWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制背景
    drawBackground(painter);
    
    // 绘制稀有度边框
    drawRarityBorder(painter);
    
    // 绘制图标
    drawIcon(painter);
    
    // 绘制状态覆盖层
    drawStatusOverlay(painter);
    
    // 绘制数量文本
    drawCountText(painter);
}

void CollectionItemWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_info.id);
    }
    QWidget::mousePressEvent(event);
}

void CollectionItemWidget::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event)
    m_isHovered = true;
    update();
}

void CollectionItemWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    m_isHovered = false;
    update();
}

void CollectionItemWidget::drawBackground(QPainter &painter)
{
    QRect rect = this->rect().adjusted(2, 2, -2, -2);
    
    // 基础背景色
    QColor bgColor;
    if (m_isHovered) {
        bgColor = QColor(200, 200, 200, 180);
    } else {
        bgColor = QColor(240, 240, 240, 150);
    }
    
    painter.fillRect(rect, bgColor);
    
    // 绘制边框
    painter.setPen(QPen(QColor(180, 180, 180), 1));
    painter.drawRect(rect);
}

void CollectionItemWidget::drawIcon(QPainter &painter)
{
    QRect iconRect = this->rect().adjusted(8, 8, -8, -8);
    
    if (m_info.status == CollectionStatus::Unknown) {
        // 未发现状态：绘制问号
        painter.setPen(QPen(QColor(100, 100, 100), 2));
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.drawText(iconRect, Qt::AlignCenter, "?");
    } else if (m_info.status == CollectionStatus::Discovered) {
        // 已发现状态：半透明显示
        if (!m_iconPixmap.isNull()) {
            painter.setOpacity(0.5);
            painter.drawPixmap(iconRect, m_iconPixmap.scaled(iconRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            painter.setOpacity(1.0);
        } else {
            // 没有图标时显示占位符
            painter.setPen(QPen(QColor(150, 150, 150), 1));
            painter.setFont(QFont("Arial", 10));
            painter.drawText(iconRect, Qt::AlignCenter | Qt::TextWordWrap, m_info.name);
        }
    } else {
        // 已收集状态：完整显示
        if (!m_iconPixmap.isNull()) {
            painter.drawPixmap(iconRect, m_iconPixmap.scaled(iconRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            // 没有图标时显示占位符
            painter.setPen(QPen(QColor(80, 80, 80), 1));
            painter.setFont(QFont("Arial", 10));
            painter.drawText(iconRect, Qt::AlignCenter | Qt::TextWordWrap, m_info.name);
        }
    }
}

void CollectionItemWidget::drawRarityBorder(QPainter &painter)
{
    QColor rarityColor = getRarityColor(m_info.rarity);
    
    // 绘制稀有度边框
    painter.setPen(QPen(rarityColor, 3));
    painter.drawRect(this->rect().adjusted(1, 1, -1, -1));
    
    // 绘制内边框光晕效果
    if (m_info.rarity >= CollectionRarity::Epic) {
        painter.setPen(QPen(rarityColor.lighter(150), 1));
        painter.drawRect(this->rect().adjusted(3, 3, -3, -3));
    }
}

void CollectionItemWidget::drawStatusOverlay(QPainter &painter)
{
    if (m_info.status == CollectionStatus::Discovered) {
        // 已发现但未收集：绘制"NEW"标签
        QRect labelRect(width() - 25, 5, 20, 12);
        painter.fillRect(labelRect, QColor(255, 165, 0, 200));
        painter.setPen(QPen(Qt::white));
        painter.setFont(QFont("Arial", 8, QFont::Bold));
        painter.drawText(labelRect, Qt::AlignCenter, "NEW");
    } else if (m_info.status == CollectionStatus::Collected) {
        // 已收集：绘制对勾
        QRect checkRect(width() - 20, height() - 20, 15, 15);
        painter.fillRect(checkRect, QColor(0, 200, 0, 180));
        painter.setPen(QPen(Qt::white, 2));
        painter.drawLine(checkRect.left() + 3, checkRect.center().y(),
                        checkRect.center().x(), checkRect.bottom() - 3);
        painter.drawLine(checkRect.center().x(), checkRect.bottom() - 3,
                        checkRect.right() - 3, checkRect.top() + 3);
    }
}

void CollectionItemWidget::drawCountText(QPainter &painter)
{
    if (m_info.status == CollectionStatus::Collected && m_info.totalObtained > 1) {
        // 显示数量
        QRect countRect(5, height() - 20, 30, 15);
        painter.fillRect(countRect, QColor(0, 0, 0, 150));
        painter.setPen(QPen(Qt::white));
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(countRect, Qt::AlignCenter, QString::number(m_info.totalObtained));
    }
}

QColor CollectionItemWidget::getRarityColor(CollectionRarity rarity) const
{
    switch (rarity) {
        case CollectionRarity::Common:
            return QColor(128, 128, 128);  // 灰色
        case CollectionRarity::Rare:
            return QColor(0, 120, 255);    // 蓝色
        case CollectionRarity::Epic:
            return QColor(160, 0, 255);    // 紫色
        case CollectionRarity::Legendary:
            return QColor(255, 215, 0);    // 金色
        default:
            return QColor(100, 100, 100);
    }
}

QPixmap CollectionItemWidget::getStatusIcon(CollectionStatus status) const
{
    // 这里可以加载不同状态的图标
    // 目前返回空，使用绘制的方式
    Q_UNUSED(status)
    return QPixmap();
}
