#include "DataBase.h"

DataBase::DataBase()
{
  cur_db_name = "db_name";
  m_query = QSqlQuery();
  m_last_error = "No errors";
}

shared_ptr<DataBase> DataBase::instance()
{
  static shared_ptr<DataBase> my_instance(new DataBase());

  return my_instance;
}

Status DataBase::setSQLiteDataBase(QString db_name)
{
  cur_db_name = db_name;

  QSqlDatabase cur_db;

  if (QSqlDatabase::contains())
    cur_db = QSqlDatabase::database(QLatin1String(QSqlDatabase::defaultConnection), false);
  else
    cur_db = QSqlDatabase::addDatabase("QSQLITE");

  cur_db.setDatabaseName(db_name + ".sqlite");

  if (!cur_db.isValid())
  {
    m_last_error = cur_db.lastError().text().toStdString().c_str();
    return DATABASE_IS_NOT_VALID;
  }
  else if (!cur_db.open())
  {
    m_last_error = cur_db.lastError().text().toStdString().c_str();
    return DATABASE_OPEN_ERROR;
  }
  else
  {
    m_query = QSqlQuery(QSqlDatabase::database(cur_db_name));
  }

  return OK;
}

Status DataBase::resetSQLiteDataBase()
{
  execQuery("DROP TABLE logs;");

  return execQuery("CREATE TABLE IF NOT EXISTS logs\
                   (\
                     id INTEGER PRIMARY KEY AUTOINCREMENT,\
                     session_id INTEGER,\
                     date_time DATETIME,\
                     int_time INTEGER,\
                     command TEXT\
                     );");
}

bool DataBase::databaseExists(QString db_name)
{
  return QFile::exists(db_name + ".sqlite");
}

Status DataBase::getRowsInLogs(QString db_name, int &rows_number)
{
  if (!DataBase::databaseExists(db_name))
    return DATABASE_DOES_NOT_EXISTS;
  QSqlDatabase db;
  if (QSqlDatabase::contains())
    db = QSqlDatabase::database(QLatin1String(QSqlDatabase::defaultConnection), false);
  else
    db = QSqlDatabase::addDatabase("QSQLITE");

  db.setDatabaseName(db_name + ".sqlite");

  if (!db.isValid())
  {
    m_last_error = db.lastError().text().toStdString().c_str();
    return DATABASE_IS_NOT_VALID;
  }
  else if (!db.open())
  {
    m_last_error = db.lastError().text().toStdString().c_str();
    return DATABASE_OPEN_ERROR;
  }
  else
  {
    QSqlQuery query = QSqlQuery(db);

    if (!query.exec("SELECT * FROM logs;"))
    {
      m_last_error = m_query.lastError().text();
      return EXEC_ERROR;
    }

    rows_number = 0;
    while (query.next())
      rows_number++;
  }
  return OK;
}

Status DataBase::getSessionsInLogs(int &sessions_n)
{
  QString q_str = "select count(session_id) from (select distinct session_id from logs);";
  Status s = execQuery(q_str);
  if (s != OK)
    return s;

  sessions_n = -1;
  if (m_query.next())
    sessions_n = m_query.value(0).toInt();

  return s;
}

Status DataBase::addCommand(int session_id, const QString &datetime, const QString& cmd, int &id)
{
  id = -1;
  QString q_str = "insert into logs(session_id, date_time, int_time, command) values(";
  q_str += QString::number(session_id) + ", '";
  q_str += datetime + "', ";
  q_str += QString::number(QDateTime().fromString(datetime,"yyyy-MM-dd hh:mm:ss").toSecsSinceEpoch()) + ", '";
  q_str += cmd + "');";

  Status status = execQuery(q_str);
  if (status == OK)
    id = m_query.lastInsertId().toInt();
  return status;
}

Status DataBase::addCommand(int session_id, int int_time, const QString &cmd)
{
  QString q_str = "insert into logs(session_id, int_time, command) values(";
  q_str += QString::number(session_id) + ", ";
  //q_str += datetime + "', ";
  q_str += QString::number(int_time) + ", '";
  q_str += cmd + "');";

  return execQuery(q_str);
}

Status DataBase::getCmdsMap(QMap<int, QString> &cmds_map)
{
  QString q_str = "SELECT command FROM logs GROUP BY command;";
  Status s = execQuery(q_str);
  if (s != OK)
    return s;

  int id = 0;
  cmds_map.clear();
  while (m_query.next())
    cmds_map[id++] = m_query.value(0).toString();

  return s;
}

Status DataBase::getSessionsNum(int &sessions_num)
{
  QString q_str = "SELECT MAX(session_id) FROM logs;";
  Status s = execQuery(q_str);
  if (s != OK)
    return s;

  if (m_query.next())
    sessions_num = m_query.value(0).toInt()+1;

  return s;
}

Status DataBase::getAllLogs(int commands_num, QList<Session> &sessions)
{
  QString query_str = "with cmds_ids(command, cmd_id) as\
                      (\
                        select command, (row_number() over (partition by same))-1 as cmd_id\
                        from\
                        (\
                          select 1 as same, command\
                          from logs\
                          group by command\
                        )\
                      )\
                      select id, session_id, int_time, cmd_id\
                      from cmds_ids join logs on (cmds_ids.command = logs.command);";

  Status s = execQuery(query_str);
  if (s != OK)
    return s;

  sessions.clear();
  int cur_session_id = 0;
  sessions.push_back(Session(cur_session_id));
  QList<Command> commands;
  while (m_query.next())
  {
    int id = m_query.value(0).toInt();
    int session_id = m_query.value(1).toInt();
    int time = m_query.value(2).toInt();
    int cmd_id = m_query.value(3).toString().toInt();

    if (session_id > cur_session_id)
    {
      sessions[cur_session_id].setCommands(commands, commands_num);
      cur_session_id = session_id;
      sessions.push_back(Session(cur_session_id));
      commands.clear();
    }

    //commands.append(Command(session_id, time.toSecsSinceEpoch(), cmd_id));
    commands.append(Command(id, session_id, time, cmd_id));
  }
  sessions[cur_session_id].setCommands(commands, commands_num);
  commands.clear();

  return s;
}

QString DataBase::lastError()
{
  return m_last_error;
}

Status DataBase::execQuery(QString query)
{
  if (!m_query.exec(query))
  {
    m_last_error = m_query.lastError().text();
    return EXEC_ERROR;
  }
  return OK;
}
