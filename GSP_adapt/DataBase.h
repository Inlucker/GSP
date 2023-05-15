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
  DataBase();

  static shared_ptr<DataBase> instance();

  Status setSQLiteDataBase(QString db_name = "db_name");
  Status resetSQLiteDataBase();
  bool databaseExists(QString db_name);
  Status getRowsInLogs(QString db_name, int &rows_number);
  Status getSessionsInLogs(int &sessions_n);
  Status addCommand(int session_id, const QString& datetime, const QString &cmd, int &id);
  Status addCommand(int session_id, int int_time, const QString &cmd, int &id);
  Status getCmdsMap(QMap<int, QString>& cmds_map);
  Status getSessionsNum(int& sessions_num);
  Status getAllLogs(int commands_num, QList<Session>& sessions);

  QString lastError();

private:
  inline Status execQuery(QString query);

private:
  QString m_last_error;
  QString cur_db_name;
  QSqlQuery m_query;
};

#endif // DATABASE_H
