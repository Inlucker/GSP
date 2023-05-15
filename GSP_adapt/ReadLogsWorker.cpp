#include "ReadLogsWorker.h"
#include "LogReader.h"

ReadLogsWorker::ReadLogsWorker(const QString& _dir, const QString &_db_name, bool _logs_with_time)
  : dir(_dir),
    db_name(_db_name),
    logs_with_time(_logs_with_time)
{
}

void ReadLogsWorker::run()
{
  shared_ptr<LogReader> log_reader = LogReader::instance();

  chrono::time_point<Clock> start = Clock::now();
  if (logs_with_time)
    log_reader->readLogs(dir);
  else
    log_reader->readLogsWithoutTime(dir);
  chrono::time_point<Clock> end = Clock::now();
  chrono::nanoseconds diff = chrono::duration_cast<chrono::nanoseconds>(end - start);
  qDebug() << "readLogs() time: " << diff.count() / 1000000000. << " s";

  emit resultReady(db_name);
}
