#ifndef VIEWDATAMANAGER_H
#define VIEWDATAMANAGER_H

#include <map>
#include <vector>
#include <initializer_list>
#include <QVariant>
#include "commonConst.h"

class ViewDataManager {
public:
    ViewDataManager() = delete;
    ViewDataManager(int userId) : m_userId(userId)
    {
        InitSqlAndViewList();
    }
    ~ViewDataManager() = default;

    int AddData(WorkData& data);
    bool DeleteData(const std::vector<int>& workId);
    bool ModifyData(int workId, EventTableHeader e, const QVariant& newValue);
    std::vector<WorkData> GetViewDataWithFilter(WorkData& filterDate, std::initializer_list<EventTableHeader> filterType);
    std::vector<WorkData> GetViewData() const
    {
        return m_viewList;
    }

private:
    void InitSqlAndViewList();
    
private:
    std::vector<WorkData> m_viewList;
    const int m_userId;
};

#endif // VIEWDATAMANAGER_H
