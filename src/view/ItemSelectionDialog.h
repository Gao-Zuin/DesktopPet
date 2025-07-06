#ifndef __ITEM_SELECTION_DIALOG_H__
#define __ITEM_SELECTION_DIALOG_H__

#include <QDialog>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include "../common/Types.h"

class PetViewModel;

// 物品选择项
class ItemSelectionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemSelectionWidget(int itemId, const QString& itemName, int ownedCount, QWidget* parent = nullptr);
    
    bool isSelected() const { return m_selected; }
    int getItemId() const { return m_itemId; }
    int getSelectedCount() const;
    void setMaxSelectableCount(int maxCount);

signals:
    void selectionChanged();

private slots:
    void onSelectionChanged();

private:
    void setupUI();
    void updateUI();

private:
    int m_itemId;
    QString m_itemName;
    int m_ownedCount;
    int m_maxSelectableCount;
    bool m_selected;
    
    QCheckBox* m_checkBox;
    QLabel* m_nameLabel;
    QLabel* m_countLabel;
    QSpinBox* m_countSpinBox;
};

// 物品选择对话框
class ItemSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ItemSelectionDialog(PetViewModel* viewModel, ItemQuality targetQuality, int requiredCount, QWidget* parent = nullptr);
    ~ItemSelectionDialog();

    QVector<QPair<int, int>> getSelectedItems() const; // 返回 <itemId, count> 对

private slots:
    void onSelectionChanged();
    void onConfirmClicked();
    void onCancelClicked();

private:
    void setupUI();
    void loadAvailableItems();
    void updateConfirmButton();

private:
    PetViewModel* m_viewModel;
    ItemQuality m_targetQuality;
    int m_requiredCount;
    int m_currentSelectedCount;
    
    QScrollArea* m_scrollArea;
    QWidget* m_scrollWidget;
    QVBoxLayout* m_scrollLayout;
    QLabel* m_infoLabel;
    QLabel* m_selectedCountLabel;
    QPushButton* m_confirmButton;
    QPushButton* m_cancelButton;
    
    QVector<ItemSelectionWidget*> m_selectionWidgets;
};

#endif // __ITEM_SELECTION_DIALOG_H__
