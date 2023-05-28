#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "LogReader.h"
#include "Sequence.h"
#include "Session.h"
#include "DataBase.h"
#include <QList>
#include <QMap>

class Calculator
{
public:
  Calculator();

  int prepareGSP();
  QList<Sequence> getFrequentSequences(double _min_sup = -1, int _min_gap = -1, int _max_gap = -1);
  void printFrequentSequences();
  QString getSeqStr(const Sequence &seq);
  void setSameCmds(bool val);

  void test1();
  void test5();
  void test6();
  void test7();

private:
  QList<Sequence> generateCandidates1();
  QList<Sequence> generateCandidates();
  bool findCommand(int cmd, const Session& session, int min_time, int prev_cmd_id, int &time, int &id) const;
  bool sessionSupportsSequence(const Session& session, const Sequence& seq);
  Sequence findFreqSequenceByCommand(int cmd);
  double calcLift(Sequence seq);
  int countSupport(QList<Sequence> &candidates, const QList<Session> &sessions);
  void sortFrequentSequences();

private:
  QList<Sequence> freq_seqs;
  QList<Sequence> cur_freq_seqs;
  QMap<int, QString> cmds_map;
  int sessions_count = 0;
  QString db_file_path;
  QList<Session> sessions;
  int records_num = 0;

  //LogReader log_reader;

  double min_support = 0.5;
  int min_gap = 0;
  int max_gap = INT_MAX;
  bool same_cmds = true;

  chrono::nanoseconds gen_time = std::chrono::nanoseconds::zero();;
  chrono::nanoseconds count_time = std::chrono::nanoseconds::zero();;
};

#endif // CALCULATOR_H
