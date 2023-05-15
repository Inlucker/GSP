#ifndef READLOGSWORKER_H
#define READLOGSWORKER_H

#include <QObject>
#include <chrono>

typedef std::chrono::high_resolution_clock Clock;

class ReadLogsWorker : public QObject
{
  Q_OBJECT
public:
  explicit ReadLogsWorker(const QString &_dir = "./", const QString &_db_name = "./", bool _logs_with_time = true);

public slots:
  void run();

signals:
  void resultReady(QString db_name);

private:
  QString dir = "./";
  QString db_name = "./";
  bool logs_with_time = true;
};

#endif // READLOGSWORKER_H
