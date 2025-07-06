#ifndef WORKMODEL_H
#define WORKMODEL_H

#include "../common/PropertyTrigger.h"
#include "../common/PropertyIds.h"
#include "../common/Types.h"
#include "base/WorkInfo.h"
#include <QTimer>
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>

class WorkModel : public QObject
{
    Q_OBJECT

public:
    WorkModel() noexcept;
    WorkModel(const WorkModel &) = delete;
    ~WorkModel() noexcept;

    WorkModel &operator=(const WorkModel &) = delete;

    // 获取属性触发器
    PropertyTrigger &get_trigger() noexcept
    {
        return m_trigger;
    }

    // 获取所有打工类型信息
    const QVector<WorkInfo> &getWorkTypes() const noexcept
    {
        return m_workTypes;
    }

    // 获取当前打工状态
    WorkStatus getCurrentStatus() const noexcept
    {
        return m_currentStatus;
    }

    // 获取当前打工类型
    WorkType getCurrentWorkType() const noexcept
    {
        return m_currentWorkType;
    }

    // 获取剩余工作时间
    int getRemainingTime() const noexcept
    {
        return m_remainingTime;
    }

    // 是否处于连续工作模式
    bool isContinuousMode() const noexcept
    {
        return m_continuousMode;
    }

    // 开始打工
    void startWork(WorkType type) noexcept;

    // 停止打工
    void stopWork() noexcept;

    // 获取指定类型的打工信息
    const WorkInfo *getWorkInfo(WorkType type) const noexcept;

private slots:
    void onWorkTimer();

private:
    void initializeWorkTypes();
    void fireWorkStatusUpdate();
    void generateSunshine(); // 生成随机阳光物品
    void generateMinerals(); // 生成随机矿石物品
    void generateWoods();    // 生成随机木头物品

private:
    QVector<WorkInfo> m_workTypes; // 所有打工类型
    PropertyTrigger m_trigger;     // 属性触发器

    WorkStatus m_currentStatus; // 当前打工状态
    WorkType m_currentWorkType; // 当前打工类型
    int m_remainingTime;        // 剩余工作时间(秒)
    bool m_continuousMode;      // 连续工作模式

    QTimer *m_workTimer; // 工作定时器
};

#endif // WORKMODEL_H