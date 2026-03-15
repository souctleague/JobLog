#ifndef SQLITEOPERATOR_H
#define SQLITEOPERATOR_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>
#include <vector>
#include "commonConst.h"

class SqliteOperator
{
public:
    static SqliteOperator& getInstance()
    {
        static SqliteOperator instance;
        return instance;
    }

    SqliteOperator(const SqliteOperator&) = delete;
    SqliteOperator(SqliteOperator&&) = delete;
    SqliteOperator& operator=(const SqliteOperator&) = delete;
    SqliteOperator& operator=(SqliteOperator&&) = delete;

public:
    // 打开数据库
    bool openAndInitDb(void);
    // 创建工作事务数据表
    void createUserAdministrationTable();
    void createWorkListsTable();
    // 判断数据表是否存在
    bool isTableExist(QString&& tableName);

public:
    // 创建新的用户
    bool addNewAccount(const char (&userName)[64], const char (&password)[64]) const;
    // 查询用户是否存在并返回userId
    bool IsUserExist(const char (&userName)[64]) const;
    // 校验用户名与密码是否存在
    std::pair<bool, int> IsAccountExist(const char (&userName)[64], const char (&password)[64]) const;

public: // 操作工作事务表相关操作
    // 查询全部数据
    std::vector<WorkData> queryTable(int userId);
    // 插入事务数据
    int singleInsertData(int userId, WorkData &singleData); // 插入单条数据
    // 修改数据
    bool modifyData(int workId, EventTableHeader e, const QVariant& newValue);
    // 删除数据
    bool deleteData(const std::vector<int>& workId);
    //删除数据表
    void deleteTable(QString& tableName);
    // 关闭数据库
    void closeDb(void);

private:
    SqliteOperator();

private:
    QSqlDatabase m_database;// 用于建立和数据库的连接
};

#endif //  SQLITEOPERATOR_H
