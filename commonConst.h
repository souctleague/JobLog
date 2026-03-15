#ifndef COMMONCONST_H
#define COMMONCONST_H

#include <cstdint>
#include <QString>
#include <QDateTime>

// 事务表中各列信息，数值代表对应列
enum EventTableHeader {
    WorkEventId = 0,
    CompletionStatus = 1,
    EventType = 2,
    Msg = 3,
    PlannedCompletionTime = 4,
    Priority = 5,
    MaxEventTableCount
};

enum WorkEventType {
    Req = 0, // 需求
    SDTS = 1, // 问题单
    SP = 2, // 专项
    Train = 3, // 培训
    MaxWorkEventCount
};

enum WorkPriorityEnum {
    Lower = 0,
    Middle = 1,
    High = 2,
    MaxWorkPriorityCount
};

struct UserAdministrationData {
    int userId;
    char user[64];
    char password[64];
};

struct WorkData {
    bool isWorkComplete = false;
    int workEvent = 0;
    QString workMsg;
    QDateTime workDateTime;
    int workPriority = 1;
    int workId = 0;
};

const char* const DATA_BASE_NAME = "MyDataBase.db";
const char* const SQL_NAME = "QSQLITE";
const char* const USER_TABLE = "user_table";
const char* const HISTORY_TABLE = "history_table";

static int g_userId = -1;

#endif // COMMONCONST_H
