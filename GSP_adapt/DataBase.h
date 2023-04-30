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
  DATABASE_OPEN_ERROR,
  DATABASE_DOES_NOT_EXISTS,
  DATABASE_IS_NOT_VALID
};

class DataBase
{
public:
  DataBase() = delete;

  static Status setSQLiteDataBase(QString db_name = "db_name");
  static Status resetSQLiteDataBase();
  static bool databaseExists(QString db_name);
  static Status getRowsInLogs(QString db_name, int &rows_number);
  static Status addCommand(int session_id, const QString& datetime, const QString &cmd);
  static Status addCommand(int session_id, int int_time, const QString &cmd);
  static Status getCmdsMap(QMap<int, QString>& cmds_map);
  static Status getSessionsNum(int& sessions_num);
  static Status getAllLogs(int commands_num, QList<Session>& sessions);

  static QString lastError();

private:
  static inline Status execQuery(QString query);

private:
  //static unique_ptr<QSqlDatabase> cur_db;
  static QString m_last_error;
  static QString cur_db_name;
  static QSqlQuery m_query;
};

#endif // DATABASE_H
