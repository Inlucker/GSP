#include "ReadLogsWorker.h"
#include "LogReader.h"

ReadLogsWorker::ReadLogsWorker(const QString& _dir, const QString &_db_name)
  : dir(_dir),
    db_name(_db_name)
{
}

void ReadLogsWorker::run()
{
  shared_ptr<LogReader> log_reader = LogReader::instance();

  chrono::time_point<Clock> start = Clock::now();
  log_reader->readLogs(dir);  chrono::time_point<Clock> end = Clock::now();
  chrono::nanoseconds diff = chrono::duration_cast<chrono::nanoseconds>(end - start);
  qDebug() << "readLogs() time: " << diff.count() / 1000000000. << " s";

  emit resultReady(db_name);
}
