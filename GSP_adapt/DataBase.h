#ifndef DATABASE_H
#define DATABASE_H

#include "Session.h"
#include <QtSql>
#include <memory>

using namespace std;

enum Status
{
  OK = 0,
  EXEC_ERROR,
  EMPTY_RES,
  DATABASE_OPEN_ERROR
};

class DataBase
{
public:
  DataBase() = delete;

  static Status createSQLiteDataBase(QString db_name = "db_name");
  static Status resetSQLiteDataBase();
  static Status addCommand(int session_id, const QString& datetime, const QString &cmd);
  static Status addCommand(int session_id, int int_time, const QString &cmd);
  static Status getCmdsMap(QMap<int, QString>& cmds_map);
  static Status getSessionsNum(int& sessions_num);
  static Status getAllLogs(int commands_num, QList<Session>& sessions);

  static QString lastError();

private:
    static inline Status execQuery(QString query);

private:
  static unique_ptr<QSqlDatabase> cur_db;
  static QSqlQuery m_query;
  static QString m_last_error;
};

#endif // DATABASE_H
