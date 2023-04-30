#include "LogReader.h"
#include "DataBase.h"

QDir::Filters LogReader::dir_filters = QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot;
//name_filters << "*.log";
QStringList LogReader::ignore_commands = {"Inspector", "StartupVPerfTest", "TipOfDay"};

/*LogReader::LogReader()
{
}*/

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

void LogReader::readFile(const QFileInfo &file_info, QList<QString> &commands, int &session_id)
{
  QFile file (file_info.absoluteFilePath());
  //qDebug() << "File:" << "\t" << file.fileName();

  if (!file.open(QIODevice::ReadOnly))
    return; // не получилось открыть файл

  bool empty_session = true;
  while (!file.atEnd())
  {
    QString line = QString(file.readLine());
    int time = getTimeFromRecord(line);
    if (time > 0)
    {
      QString datetime = line.left(19);
      QString cmd;
      if (getCommandFromRecord(line, cmd))
      {
        if (cmd == "Exit" && !empty_session)
        {
          session_id++;
          empty_session = true;
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

          if (DataBase::addCommand(session_id, datetime, cmd) != OK)
            qDebug() << DataBase::lastError(); //Make throw instead qDebug()?
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

        if (DataBase::addCommand(session_id, int_time++, cmd) != OK)
          qDebug() << DataBase::lastError(); //Make throw instead qDebug()?

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

int LogReader::getCommandFromRecord(QString r, QString &res)
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
