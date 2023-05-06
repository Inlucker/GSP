#ifndef LOGREADER_H
#define LOGREADER_H

#include <QDir>

class Action
{
public:
  Action(int _time, QString _command)
  {
    this->time = _time;
    this->command = _command;
  }
  int time;
  QString command;
};

class LogReader
{
public:
  LogReader() = delete;

  static void readLogs(QString dir_name = "./");
  static void readLogsWithoutTime(QString dir_name = "./");
  static void includeEndCmds(bool val = true);

private:
  static void readFile(const QFileInfo& file_info, QList<QString> &commands, int &session_id);
  static void readDir(const QString& abs_path, QList<QString> &commands, int &session_id);
  static void readFileWithoutTime(const QFileInfo& file_info, QList<QString> &commands, int &session_id);
  static void readDirWithoutTime(const QString& abs_path, QList<QString> &commands, int &session_id);
  static int getTimeFromRecord(QString r);
  static bool getCommandFromRecord(QString r, QString& res);
  static QStringList getAllCommandsFromRecord(QString r);
  static QStringList getTwoCommandsFromRecord(QString r);

private:
  //QStringList name_filters;
  static QDir::Filters dir_filters;
  static QStringList ignore_commands;
  static QStringList new_session_commands;
  static bool end_cmds;
};

#endif // LOGREADER_H
