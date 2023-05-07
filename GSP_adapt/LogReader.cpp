#include "LogReader.h"
#include "DataBase.h"

LogReader::LogReader()
{
  dir_filters = QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot;
  ignore_commands = {"Inspector", "StartupVPerfTest", "TipOfDay",
                     "/Inspector", "/StartupVPerfTest", "/TipOfDay",
                     "/Exit", "/NewDocument", "/OpenDocument"};
  new_session_commands = {"Exit", "NewDocument", "OpenDocument"};
  end_cmds = false;
}

shared_ptr<LogReader> LogReader::instance()
{
  static shared_ptr<LogReader> my_instance(new LogReader());

  return my_instance;
}

void LogReader::readLogs(QString dir_name)
{
  QDir dir(dir_name);
  dir.setFilter(dir_filters);
  //dir.setNameFilters(name_filters);
  QList<QString> commands;
  int session_id = 0;

  for (QFileInfo& file_info : dir.entryInfoList())
    if (file_info.isFile() && file_info.completeSuffix() == "log")
      readFile(file_info, commands, session_id);
    else if (file_info.isDir())
      readDir(file_info.absoluteFilePath(), commands, session_id);
}

void LogReader::readLogsWithoutTime(QString dir_name)
{
  QDir dir(dir_name);
  dir.setFilter(dir_filters);
  //dir.setNameFilters(name_filters);
  QList<QString> commands;
  int session_id = 0;

  for (QFileInfo& file_info : dir.entryInfoList())
  {
    if (file_info.isFile() && file_info.completeSuffix() == "log")
    {
      readFileWithoutTime(file_info, commands, session_id);
    }
    else if (file_info.isDir())
    {
      readDirWithoutTime(file_info.absoluteFilePath(), commands, session_id);
    }
  }
}

void LogReader::includeEndCmds(bool val)
{
  LogReader::end_cmds = val;
}

QStringList LogReader::getIgnoreList() const
{
  return ignore_commands;
}

QStringList LogReader::getNewSessionCmdsList() const
{
  return new_session_commands;
}

void LogReader::setIgnoreList(QStringList list)
{
  ignore_commands = list;
}

void LogReader::setNewSessionCmdsList(QStringList list)
{
  new_session_commands = list;
}

void LogReader::readFile(const QFileInfo &file_info, QList<QString> &commands, int &session_id)
{
  QFile file (file_info.absoluteFilePath());
  //qDebug() << "File:" << "\t" << file.fileName();

  if (!file.open(QIODevice::ReadOnly))
    return; // не получилось открыть файл

  shared_ptr<DataBase> db = DataBase::instance();
  bool empty_session = true;
  while (!file.atEnd())
  {
    QString line = QString(file.readLine());
    int time = getTimeFromRecord(line);
    if (time > 0)
    {
      QString datetime = line.left(19);
      if (end_cmds)
      {
        QStringList cmds = getTwoCommandsFromRecord(line);
        if (cmds.size() > 2)
          qDebug () << "Here: " << cmds;
        for (int i = 0; i < cmds.size(); i++)
        {
          QString cmd = cmds[i];
          if (new_session_commands.contains(cmd))
          {
            if (!empty_session)
            {
              session_id++;
              empty_session = true;
            }
          }
          else
          {
            empty_session = false;
            bool cmd_exists = false;
            for (const QString& c : commands)
              if (c == cmd)
              {
                cmd_exists = true;
                break;
              }
            if (!cmd_exists)
              commands.append(cmd);

            int id = -1;
            if (db->addCommand(session_id, datetime, cmd, id) != OK)
              qDebug() << db->lastError(); //Make throw instead qDebug()?
            if (id % 100 == 0)
              qDebug() << id;
          }
        }
      }
      else
      {
        QString cmd = "";
        if (getCommandFromRecord(line, cmd))
        {
          if (new_session_commands.contains(cmd))
          {
            if (!empty_session)
            {
              session_id++;
              empty_session = true;
            }
          }
          else
          {
            empty_session = false;
            bool cmd_exists = false;
            for (const QString& c : commands)
              if (c == cmd)
              {
                cmd_exists = true;
                break;
              }
            if (!cmd_exists)
              commands.append(cmd);

            int id = -1;
            if (db->addCommand(session_id, datetime, cmd, id) != OK)
              qDebug() << db->lastError(); //Make throw instead qDebug()?
            if (id % 1000 == 0)
              qDebug() << id;
          }
        }
      }
    }
  }

  file.close();
}

void LogReader::readDir(const QString& abs_path, QList<QString> &commands, int &session_id)
{
  QDir dir(abs_path);
  dir.setFilter(dir_filters);
  //dir.setNameFilters(name_filters);

  for (QFileInfo& file_info : dir.entryInfoList())
  {
    if (file_info.isFile() && file_info.completeSuffix() == "log")
    {
      readFile(file_info, commands, session_id);
    }
    else if (file_info.isDir())
    {
      readDir(file_info.absoluteFilePath(), commands, session_id);
    }
  }
}

void LogReader::readFileWithoutTime(const QFileInfo &file_info, QList<QString> &commands, int &session_id)
{
  QFile file (file_info.absoluteFilePath());
  //qDebug() << "File:" << "\t" << file.fileName();

  if (!file.open(QIODevice::ReadOnly))
    return; // не получилось открыть файл

  shared_ptr<DataBase> db = DataBase::instance();
  int int_time = 0;
  while (!file.atEnd())
  {
    QString line = QString(file.readLine());
    if (line[0] == '<')
    {
      QString cmd;
      if (getCommandFromRecord(line, cmd))
      {
        bool cmd_exists = false;
        for (const QString& c : commands)
          if (c == cmd)
          {
            cmd_exists = true;
            break;
          }
        if (!cmd_exists)
          commands.append(cmd);

        if (db->addCommand(session_id, int_time++, cmd) != OK)
          qDebug() << db->lastError(); //Make throw instead qDebug()?

        if (cmd == "Exit")
        {
          session_id++;
          int_time = 0;
        }
      }
    }
  }

  file.close();
}

void LogReader::readDirWithoutTime(const QString &abs_path, QList<QString> &commands, int &session_id)
{
  QDir dir(abs_path);
  dir.setFilter(dir_filters);
  //dir.setNameFilters(name_filters);

  for (QFileInfo& file_info : dir.entryInfoList())
  {
    if (file_info.isFile() && file_info.completeSuffix() == "log")
    {
      readFileWithoutTime(file_info, commands, session_id);
    }
    else if (file_info.isDir())
    {
      readDirWithoutTime(file_info.absoluteFilePath(), commands, session_id);
    }
  }
}

int LogReader::getTimeFromRecord(QString r)
{
  if (r.length() < 20)
    return -1;
  bool ok = false;
  int year = r.mid(0, 4).toInt(&ok);
  if (!ok)
    return -1;
  int month = r.mid(5, 2).toInt(&ok);
  if (!ok)
    return -1;
  int day = r.mid(8, 2).toInt(&ok);
  if (!ok)
    return -1;
  int hours = r.mid(11, 2).toInt(&ok);
  if (!ok)
    return -1;
  int minutes = r.mid(14, 2).toInt(&ok);
  if (!ok)
    return -1;
  int seconds = r.mid(17, 2).toInt(&ok);
  if (!ok)
    return -1;

  int res = seconds + minutes * 60 + hours * 60 * 60 + day * 24 * 60 * 60;
  return res;
}

bool LogReader::getCommandFromRecord(QString r, QString &res)
{
  res = "";
  int start = r.indexOf('<')+1;
  int end = r.indexOf('>');
  if (start == 0 || end == 0)
    return false;
  res = r.mid(start, end - start);

  if (ignore_commands.contains(res))
    return false;

  return true;
}

//Считывает <> также внутри стркои команды
QStringList LogReader::getAllCommandsFromRecord(QString r)
{
  QStringList res;
  int cmds_n = r.count("<");

  int start = -1;
  int end = -1;
  for (int i = 0; i < cmds_n; i++)
  {
    QString s = "";
    start = r.indexOf('<', start + 1)+1;
    if (r[start] == '/')
      end = r.indexOf(':', start + 1);
    else
      end = r.indexOf('>', start + 1);
    if (start == -1 || end == -1)
      continue;
    s = r.mid(start, end - start);
    if (ignore_commands.contains(s))
      continue;
    res.append(s);
  }
  return res;
}

//Считывает только начало <> и конец </> команд
QStringList LogReader::getTwoCommandsFromRecord(QString r)
{
  QStringList res;
  QString s = "";
  int start = r.indexOf('<');
  int end = r.indexOf('>');
  if (start != -1 && end != -1)
  {
    start++;
    s = r.mid(start, end - start);
    if (!ignore_commands.contains(s))
      res.append(s);
  }

  s = "";
  start = r.indexOf("</", start + 1);
  end = r.indexOf(':', start + 1);
  if (start != -1 && end != -1)
  {
    start++;
    s = r.mid(start, end - start);
    if (!ignore_commands.contains(s))
      res.append(s);
  }

  return res;
}
