#ifndef LOGREADER_H
#define LOGREADER_H

#include <QDir>
#include <memory>

using namespace std;

class LogReader
{
public:
  LogReader();

  static shared_ptr<LogReader> instance();

  void readLogs(QString dir_name = "./");
  void readLogsWithoutTime(QString dir_name = "./");
  void includeEndCmds(bool val = true);
  QStringList getIgnoreList() const;
  QStringList getNewSessionCmdsList() const;
  void setIgnoreList(QStringList list);
  void setNewSessionCmdsList(QStringList list);

private:
  void readFile(const QFileInfo& file_info, QList<QString> &commands, int &session_id);
  void readDir(const QString& abs_path, QList<QString> &commands, int &session_id);
  void readFileWithoutTime(const QFileInfo& file_info, QList<QString> &commands, int &session_id);
  void readDirWithoutTime(const QString& abs_path, QList<QString> &commands, int &session_id);
  int getTimeFromRecord(QString r);
  bool getCommandFromRecord(QString r, QString& res);
  QStringList getAllCommandsFromRecord(QString r);
  QStringList getTwoCommandsFromRecord(QString r);

private:
  QDir::Filters dir_filters;
  QStringList ignore_commands;
  QStringList new_session_commands;
  bool end_cmds;
};

#endif // LOGREADER_H
