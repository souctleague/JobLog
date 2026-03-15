#include "sqliteOperator.h"
#include <string.h>

// 构造函数中初始化数据库对象，并建立数据库
SqliteOperator::SqliteOperator()
{
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        m_database = QSqlDatabase::database("qt_sql_default_connection");
        qInfo() << "use default connect data base!";
        return;
    }

    // 建立和SQlite数据库的连接
    m_database = QSqlDatabase::addDatabase(SQL_NAME);
    // 设置数据库文件的名字
    m_database.setDatabaseName(DATA_BASE_NAME);
    qInfo() << "use sqlite connect data base!";
}

// 打开数据库
bool SqliteOperator::openAndInitDb()
{
    // 数据库打开失败
    if (!m_database.open()) {
        qDebug() << "Error: Failed to connect database." << m_database.lastError();
        return false;
    }

    // 创建用户表（主表）
    if (!isTableExist(USER_TABLE)) {
        createUserAdministrationTable();
    }

    // 创建工作任务数据表
    if (!isTableExist(HISTORY_TABLE)) {
        createWorkListsTable();
    }

    return true;
}

// 创建用户数据表
void SqliteOperator::createUserAdministrationTable()
{
    // 用于执行sql语句的对象
    QSqlQuery sqlQuery;
    // 构建创建数据库的sql语句字符串,SQLite中自增字段必须是INTEGER且填写AUTOINCREMENT（MySQL是AUTO_INCREMENT）
    QString createSql = QString("CREATE TABLE user_table (\
                          userId INTEGER PRIMARY KEY AUTOINCREMENT,\
                          userName VARCHAR(64) NOT NULL,\
                          password VARCHAR(64) NOT NULL)");
    sqlQuery.prepare(createSql);

    // 执行sql语句
    if(!sqlQuery.exec()) {
        qDebug() << "Error: Fail to create User Administration table. " << sqlQuery.lastError();
        return;
    }

    qDebug() << "User Administration Table created!";
}

// 创建工作任务数据表
void SqliteOperator::createWorkListsTable() {
    // 用于执行sql语句的对象
    QSqlQuery sqlQuery;
    // 构建创建数据库的sql语句字符串
    QString createSql = QString("CREATE TABLE history_table (\
                          workId INTEGER PRIMARY KEY AUTOINCREMENT,\
                          userId INTEGER,\
                          isWorkComplete BOOLEAN,\
                          workEvent INTEGER,\
                          workMsg TEXT,\
                          workDateTime DATE,\
                          workPriority INTEGER)");
    sqlQuery.prepare(createSql);

    // 执行sql语句
    if (!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to create Work Lists table. " << sqlQuery.lastError();
        return;
    }

    qDebug() << "Work Lists Table created!";
}

// 判断数据库中某个数据表是否存在
bool SqliteOperator::isTableExist(QString&& tableName)
{
    QSqlDatabase database = QSqlDatabase::database();
    if(database.tables().contains(tableName))
    {
        return true;
    }

    return false;
}

// 创建新的用户,注册成功时返回true
bool SqliteOperator::addNewAccount(const char (&userName)[64], const char (&password)[64]) const {
    if ((strlen(userName) == 0) || (strlen(password) == 0)) {
        qDebug() << "Can't new account, userName strlen=" <<  strlen(userName) << ", password strlen=" << strlen(password);
        return false;
    }

    if (IsUserExist(userName)) {
        qDebug() << "用户已存在不用重复注册, userName=" << userName;
        return false;
    }

    QSqlQuery sqlQuery;
    sqlQuery.prepare("\
            INSERT INTO user_table (userName, password)\
            VALUES (:userName, :password)\
        ");

    sqlQuery.bindValue(0, userName); // 绑定查询参数
    sqlQuery.bindValue(1, password); // 绑定查询参数

    if (!sqlQuery.exec()) {
        qDebug() << "注册失败：" << sqlQuery.lastError().text();
        return false;
    }

    qDebug() << "用户注册成功！";
    return true;
}

// 查询用户是否存在并返回userId
bool SqliteOperator::IsUserExist(const char (&userName)[64]) const {
    if (strlen(userName) == 0) {
        qDebug() << "Can't new account, userName strlen=" <<  strlen(userName);
        return false;
    }

    QSqlQuery sqlQuery;
    // 准备查询语句（参数绑定避免SQL注入）
    sqlQuery.prepare("SELECT * FROM user_table WHERE userName = :username");
    sqlQuery.bindValue(0, userName); // 绑定查询参数

    // 执行查询
    if (!sqlQuery.exec()) {
        qDebug() << "查询失败：" << sqlQuery.lastError().text();
        return false;
    }

    // 判断是否存在记录：query.next()返回true表示有结果
    return sqlQuery.next();
}

// 校验用户名与密码是否存在
std::pair<bool, int> SqliteOperator::IsAccountExist(const char (&userName)[64], const char (&password)[64]) const {
    // 1. 查询用户的哈希密码和盐
    QSqlQuery query;
    query.prepare("SELECT userId, password FROM user_table WHERE userName = :username");
    query.bindValue(0, userName); // 绑定查询参数

    if (!query.exec() || !query.next()) {
        qDebug() << "用户不存在或查询失败";
        qDebug() << "原始SQL语句：" << query.lastQuery();
        return std::make_pair(false, -1);
    }

    // 2. 读取密码
    QString  sqlPassword = query.value(1).toString();

    // 3. 比较数据库中密码与输入密码是否一致
    bool isValid = strncmp(sqlPassword.toUtf8().constData(), password, 64) == 0;

    return std::make_pair(isValid, query.value(0).toInt());
}

// 查询全部数据
std::vector<WorkData> SqliteOperator::queryTable(int userId)
{
    QSqlQuery sqlQuery;
    sqlQuery.exec("SELECT * FROM history_table WHERE userId = :userId");
    sqlQuery.bindValue(0, userId); // 绑定查询参数

    // 执行查询
    if (!sqlQuery.exec()) {
        qDebug() << "查询失败：" << sqlQuery.lastError().text();
        return {};
    }

    // 创建返回值
    std::vector<WorkData> result;
    // 遍历结果，封装到结构体中
    while (sqlQuery.next()) {
        WorkData data;
        // 将数据库字段值赋值到结构体成员（一一对应）
        data.workId = sqlQuery.value("workId").toInt();    // 按字段名取值（比下标更易读）
        data.isWorkComplete = sqlQuery.value("isWorkComplete").toBool();
        data.workEvent = sqlQuery.value("workEvent").toInt();
        data.workMsg = sqlQuery.value("workMsg").toString();
        QString timeStr = sqlQuery.value("workDateTime").toString();
        data.workDateTime = QDateTime::fromString(timeStr, "yyyy-MM-dd");;
        data.workPriority = sqlQuery.value("workPriority").toInt();  // 数据库字段是type，结构体用workType避免关键字冲突

        result.emplace_back(std::move(data)); // 将结构体添加到数组中
    }

    qDebug() << "筛选完成，共找到" << result.size() << "条数据";
    return result;
}

// 插入单条数据
int SqliteOperator::singleInsertData(int userId, WorkData &singledb)
{
    QSqlQuery sqlQuery;
    sqlQuery.prepare("INSERT INTO history_table \
                     (userId, isWorkComplete, workEvent, workMsg, workDateTime, workPriority)\
                      VALUES(:userId, :isWorkComplete, :workEvent, :workMsg, :workDateTime, :workPriority)");
    sqlQuery.bindValue(0, userId);
    sqlQuery.bindValue(1, singledb.isWorkComplete);
    sqlQuery.bindValue(2, singledb.workEvent);
    sqlQuery.bindValue(3, singledb.workMsg);
    sqlQuery.bindValue(4, singledb.workDateTime);
    sqlQuery.bindValue(5, singledb.workPriority);

    if(!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to insert data. " << sqlQuery.lastError();
        return -1;
    }

    // 获取最后插入的自增ID（task_id）
    QVariant lastId = sqlQuery.lastInsertId();
    if (!lastId.isValid()) {
        qDebug() << "获取自增ID失败！";
        return -1;
    }

    qDebug() << "插入任务成功，自增task_id：" << lastId.toInt();
    return lastId.toInt();
}

// 修改数据
bool SqliteOperator::modifyData(int workId, EventTableHeader e, const QVariant& newValue)
{
    static const std::map<EventTableHeader, QString> enumToSQL = {
        {EventTableHeader::CompletionStatus, "isWorkComplete"}, {EventTableHeader::EventType, "workEvent"}, {EventTableHeader::Msg, "workMsg"},
        {EventTableHeader::PlannedCompletionTime, "workDateTime"}, {EventTableHeader::Priority, "workPriority"}
    };

    // 确认是否为异常workId
    if (workId <= 0) {
        qWarning() << "无效的任务ID：" << workId;
        return false;
    }

    // 确认是否为可刷新的事务信息
    if (!enumToSQL.count(e)) {
        qWarning() << "无效的任务枚举：" << e;
        return false;
    }

    // 构造sql语句
    QString sqlStr = QString("UPDATE history_table SET %1 = :new_value WHERE workId = :workId").arg(enumToSQL.at(e));
    QSqlQuery sqlQuery;
    sqlQuery.prepare(sqlStr);
    // 注入语句内容
    switch (e) {
        case EventTableHeader::CompletionStatus:
            sqlQuery.bindValue(0, newValue.toBool());
            break;
        case EventTableHeader::EventType:
        case EventTableHeader::Priority:
            sqlQuery.bindValue(0, newValue.toInt());
            break;
        case EventTableHeader::Msg:
            sqlQuery.bindValue(0, newValue.toString());
            break;
        case EventTableHeader::PlannedCompletionTime:
            sqlQuery.bindValue(0, newValue.toDate());
            break;
    }

    sqlQuery.bindValue(1, workId);

    // 执行更新并校验结果
    if (!sqlQuery.exec()) {
        qCritical() << "更新失败：" << sqlQuery.lastError().text() << " SQL：" << sqlStr;
        return false;
    }

    // 检查是否有行被更新（防止任务ID不存在）
    if (sqlQuery.numRowsAffected() == 0) {
        qWarning() << "未找到任务ID：" << workId << "，更新无效果";
        return false;
    }

    qDebug() << "任务ID" << workId << "的字段[" << e << "]更新为：" << newValue;
    return true;
}

// 删除数据
bool SqliteOperator::deleteData(const std::vector<int>& workId)
{
    // 删除数据为空，返回失败
    if (workId.empty()) {
        return false;
    }

    // 1. 构造占位符：?, ?, ? ...
    QStringList placeholders;
    for (int i = 0; i < workId.size(); ++i) {
        placeholders << "?";
    }

    // 2. 构造 DELETE 语句
    QString sql = QString("DELETE FROM history_table WHERE workId IN (%1)")
                      .arg(placeholders.join(","));
    QSqlQuery sqlQuery;
    sqlQuery.prepare(sql);

    // 3. 绑定所有 id（防 SQL 注入，最安全）
    for (int id : workId) {
        sqlQuery.addBindValue(id);
    }

    // 4. 执行
    if(!sqlQuery.exec())
    {
        qDebug()<<sqlQuery.lastError();
        return false;
    }

    qDebug()<<"deleted data success!";
    return true;
}

//删除数据表
void SqliteOperator::deleteTable(QString& tableName)
{
    QSqlQuery sqlQuery;

    sqlQuery.exec(QString("DROP TABLE %1").arg(tableName));
    if(sqlQuery.exec())
    {
        qDebug() << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "deleted table success";
    }
}

void SqliteOperator::closeDb(void)
{
    m_database.close();
}
