#ifndef GSPWORKER_H
#define GSPWORKER_H

#include "GSP.h"
#include <QObject>
#include <chrono>

typedef std::chrono::high_resolution_clock Clock;

class GSPWorker : public QObject
{
  Q_OBJECT
public:
  explicit GSPWorker(GSP &_gsp, double _min_sup, int _min_gap, int _max_gap);

public slots:
  void run();

signals:
  void resultReady(QList<Sequence>);
  void error(QString);

private:
  GSP &gsp;
  double min_sup = 0.5;
  int min_gap = 0;
  int max_gap = INT_MAX;
};

#endif // GSPWORKER_H
