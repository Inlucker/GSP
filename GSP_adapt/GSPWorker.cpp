#include "GSPWorker.h"

GSPWorker::GSPWorker(GSP &_gsp, double _min_sup, int _min_gap, int _max_gap)
  : gsp(_gsp),
    min_sup(_min_sup),
    min_gap(_min_gap),
    max_gap(_max_gap)
{

}

void GSPWorker::run()
{
  try
  {
    chrono::time_point<Clock> start = Clock::now();
    QList<Sequence> res = gsp.getFrequentSequences(min_sup, min_gap, max_gap);
    chrono::time_point<Clock> end = Clock::now();
    chrono::nanoseconds diff = chrono::duration_cast<chrono::nanoseconds>(end - start);
    qDebug() << "getFrequentSequences() time: " << diff.count() / 1000000000. << " s";

    emit resultReady(res);

    start = Clock::now();
    gsp.printFrequentSequences();
    end = Clock::now();
    diff = chrono::duration_cast<chrono::nanoseconds>(end - start);
    qDebug() << "printFrequentSequences() time: " << diff.count() / 1000000000. << " s";
  }
  catch (QString err)
  {
    emit error(err);
  }
  catch (...)
  {
    emit error("Неизвестаня ошибка");
  }
}
