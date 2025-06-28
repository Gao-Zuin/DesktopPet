#pragma once
#include "../model/PetModel.h"
#include <QMainWindow>
#include <QLabel>
#include <QMovie>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QMouseEvent>
#include <QDebug>
#include "../viewmodel/PetViewModel.h"

class DesktopPet : public QMainWindow
{
    Q_OBJECT

public:
    DesktopPet(QWidget *parent = nullptr);
    ~DesktopPet();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void onPetPositionChanged(const QPoint &position);
    void onPetImageChanged(const QString &imagePath);
    void onMovingModeChanged(bool enabled);
    void onMoveActionTriggered();
    void onChangeActionTriggered();
    void onExitActionTriggered();
    void onConfirmMove();

private:
    void setupUI();
    void setupContextMenu();
    void loadPetImage(const QString &imagePath);
    void showChangeDialog();

private:
    PetViewModel *viewModel;

    // UI 组件
    QLabel *petLabel;
    QMovie *petMovie;
    QMenu *contextMenu;
    QAction *moveAction;
    QAction *changeAction;
    QAction *exitAction;

    // 移动相关
    QPoint lastMousePosition;
    QPushButton *confirmButton;
    bool isDragging;
};