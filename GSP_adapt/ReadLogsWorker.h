#ifndef READLOGSWORKER_H
#define READLOGSWORKER_H

#include <QThread>
#include <QObject>
#include <chrono>

typedef std::chrono::high_resolution_clock Clock;

class ReadLogsWorker : public QObject
{
  Q_OBJECT
public:
  explicit ReadLogsWorker(const QString &_dir = "./", const QString &_db_name = "./");

public slots:
  void run();

signals:
  void resultReady(QString db_name);

private:
  QString dir = "./";
  QString db_name = "./";
};

#endif // READLOGSWORKER_H
