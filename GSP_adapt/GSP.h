#ifndef GSP_H
#define GSP_H

#include "LogReader.h"
#include "Sequence.h"
#include "Session.h"
#include "DataBase.h"
#include <QList>
#include <QMap>

class GSP
{
public:
  GSP(QString db_name = "db_name");

  QList<Sequence> getFrequentSequences();
  void printFrequentSequences();

  void test1();
  void test5();
  void test6();

private:
  void prepareGSP();
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

  LogReader log_reader;

  double min_support = 1;
  int min_gap = 0;
  int max_gap = INT_MAX;
};

#endif // GSP_H
