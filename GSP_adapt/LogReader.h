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
  LogReader();

  void readLogs(QString dir_name = "./");
  void readLogsWithoutTime(QString dir_name = "./");

private:
  void readFile(const QFileInfo& file_info, QList<QString> &commands, int &session_id);
  void readDir(const QString& abs_path, QList<QString> &commands, int &session_id);
  void readFileWithoutTime(const QFileInfo& file_info, QList<QString> &commands, int &session_id);
  void readDirWithoutTime(const QString& abs_path, QList<QString> &commands, int &session_id);
  int getTimeFromRecord(QString r);
  int getCommandFromRecord(QString r, QString& res);

private:
  //QStringList name_filters;
  QDir::Filters dir_filters;
};

#endif // LOGREADER_H
