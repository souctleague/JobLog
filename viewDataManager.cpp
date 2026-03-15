#include "viewDataManager.h"
#include <QtWidgets/QMessageBox>
#include "sqliteOperator.h"

int ViewDataManager::AddData(WorkData& data)
{
    int id = SqliteOperator::getInstance().singleInsertData(m_userId, data);
    if (id < 0) {
        return -1;
    }

    data.workId = id;
    m_viewList.push_back(data);
    return 0;
}

bool ViewDataManager::DeleteData(const std::vector<int>& workId)
{
    return SqliteOperator::getInstance().deleteData(workId);
}

bool ViewDataManager::ModifyData(int workId, EventTableHeader e, const QVariant& newValue)
{
    return SqliteOperator::getInstance().modifyData(workId, e, newValue);
}

std::vector<WorkData> ViewDataManager::GetViewDataWithFilter(WorkData& filterDate, std::initializer_list<EventTableHeader> filterType)
{
    return {};
}

void ViewDataManager::InitSqlAndViewList()
{
    m_viewList = SqliteOperator::getInstance().queryTable(m_userId);
}
