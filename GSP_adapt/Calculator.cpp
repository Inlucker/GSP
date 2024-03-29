#include "Calculator.h"
#include <chrono>

typedef chrono::high_resolution_clock Clock;

Calculator::Calculator()
{
}

QList<Sequence> Calculator::getFrequentSequences(double _min_sup, int _min_gap, int _max_gap)
{
  if (_min_sup >= 0)
    min_support = _min_sup;
  if (_min_gap >= 0)
    min_gap = _min_gap;
  if (_max_gap >= 0)
    max_gap = _max_gap;

  prepareGSP();
  //ToDo Заменить на постепенную подгрузу сессий по страницам
  QList<Session> sessions;
  shared_ptr<DataBase> db = DataBase::instance();
  int records_num = 0;

  chrono::time_point<Clock> start = Clock::now();
  db->getAllLogs(cmds_map.size(), sessions, records_num);
  chrono::time_point<Clock> end = Clock::now();
  chrono::nanoseconds diff = chrono::duration_cast<chrono::nanoseconds>(end - start);
  qDebug() << "getAllLogs() time: " << diff.count() / 1000000000. << " s";

  qDebug() << "DataBase has" << records_num << "records";

  qDebug() << "Parameters: same_cmds =" << same_cmds << "; min_sup =" << min_support << "; min_gap =" << min_gap << "; max_gap =" << max_gap;

  gen_time = std::chrono::nanoseconds::zero();
  count_time = std::chrono::nanoseconds::zero();

  this->freq_seqs.clear();
  QList<Sequence> candidates = generateCandidates1();
  int added_seqs_num = countSupport(candidates, sessions);
  if (freq_seqs.size() > 0)
  {
    while (added_seqs_num > 0)
    {
      candidates = generateCandidates();
      //qDebug() << "generateCandidates() candidates.size() = " << candidates.size();
      if (candidates.size() > 0)
        added_seqs_num = countSupport(candidates, sessions);
      else
        break;
      //qDebug() << "countSupport() added_seqs_num = " << added_seqs_num;
    }
  }
  qDebug() << "Method time: " << (gen_time.count()+count_time.count()) / 1000000000. << " s";
  qDebug() << "Gen    time: " << gen_time.count() / 1000000000. << " s";
  qDebug() << "Count  time: " << count_time.count() / 1000000000. << " s";
  qDebug();

  sortFrequentSequences();
  return this->freq_seqs;
}

void Calculator::printFrequentSequences()
{
  /*for (int i = 0; i < freq_seqs.size(); i++)
    cout << freq_seqs[i];*/

  //get max_len
  int max_len = 0;
  QString res = "";
  for (int i = 0; i < freq_seqs.size(); i++)
  {
    res = "<";
    for (int j = 0; j < freq_seqs[i].size(); j++)
      res += cmds_map.value(freq_seqs[i][j]) + ", ";
    res.remove(res.size() - 2, 2);
    res += "> ";
    if (res.size() > max_len)
      max_len = res.size();
  }

  //print
  res = "";
  for (int i = 0; i < freq_seqs.size(); i++)
  {
    res = "<";
    for (int j = 0; j < freq_seqs[i].size(); j++)
      res += cmds_map.value(freq_seqs[i][j]) + ", ";
    res.remove(res.size() - 2, 2);
    res += ">";
    while (res.size() < max_len)
      res += " ";
    res+= "supp = " + QString::number(freq_seqs[i].support, 'f', 3);
    res+= " lift = " + (freq_seqs[i].lift < 0 ? "null ": QString::number(freq_seqs[i].lift, 'f', 3));
    qDebug() << res;
  }
}

QString Calculator::getSeqStr(const Sequence &seq)
{
  QString res = "<";
  for (int j = 0; j < seq.size(); j++)
    res += cmds_map.value(seq[j]) + ", ";
  res.remove(res.size() - 2, 2);
  res += ">";
  return res;
}

void Calculator::setSameCmds(bool val)
{
  same_cmds = val;
}

void Calculator::test1()
{
  cur_freq_seqs.clear();
  cur_freq_seqs.push_back({1, 2, 3});
  cur_freq_seqs.push_back({1, 2, 4});
  cur_freq_seqs.push_back({1, 3, 4});
  cur_freq_seqs.push_back({1, 3, 5});
  cur_freq_seqs.push_back({2, 3, 4});
  cur_freq_seqs.push_back({2, 3, 5});

  QList<Sequence> candidates = this->generateCandidates();

  for (int i = 0; i < candidates.size(); i++)
    cout << candidates[i];
}

void Calculator::test5()
{
  shared_ptr<LogReader> log_reader = LogReader::instance();

  shared_ptr<DataBase> db = DataBase::instance();
  db->setSQLiteDataBase();
  db->resetSQLiteDataBase();
  chrono::time_point<Clock> start = Clock::now();
  log_reader->readLogs(".\\logs");
  chrono::time_point<Clock> end = Clock::now();
  chrono::nanoseconds diff = chrono::duration_cast<chrono::nanoseconds>(end - start);
  qDebug() << "readLogs() time: " << diff.count() / 1000000000. << " s";

  min_gap = 0; // 0 1
  max_gap = INT_MAX; // 15 INT_MAX
  min_support = 0.5; // 0.5 0.01

  start = Clock::now();
  QList<Sequence> res = getFrequentSequences();
  end = Clock::now();
  diff = chrono::duration_cast<chrono::nanoseconds>(end - start);
  qDebug() << "getFrequentSequences() time: " << diff.count() / 1000000000. << " s";

  start = Clock::now();
  printFrequentSequences();
  end = Clock::now();
  diff = chrono::duration_cast<chrono::nanoseconds>(end - start);
  qDebug() << "printFrequentSequences() time: " << diff.count() / 1000000000. << " s";
}

void Calculator::test6()
{
  shared_ptr<LogReader> log_reader = LogReader::instance();
  log_reader->readLogsWithoutTime(".\\logs_without_time");
  min_gap = 1;
  max_gap = 15;
  min_support = 0.5;

  QList<Sequence> res = getFrequentSequences();
  printFrequentSequences();
}

#define ITERS 100

void Calculator::test7()
{
  min_support = 0.1;
  min_gap = 0;
  max_gap = INT_MAX;

  prepareGSP();
  //ToDo Заменить на постепенную подгрузу сессий по страницам
  QList<Session> sessions;
  shared_ptr<DataBase> db = DataBase::instance();
  int records_num = 0;

  db->getAllLogs(cmds_map.size(), sessions, records_num);
  qDebug() << "DataBase has" << records_num << "records";

  qDebug() << "Parameters: same_cmds =" << same_cmds << "; min_sup =" << min_support << "; min_gap =" << min_gap << "; max_gap =" << max_gap;

  QString res[3];

  for (int i = 0; i < 3; i++)
    res[i] = "";

  for (int i = 0; i < 10; i++)
  {
    gen_time = chrono::nanoseconds::zero();
    count_time = chrono::nanoseconds::zero();

    for (int j = 0; j < ITERS; j++)
    {
      this->freq_seqs.clear();
      QList<Sequence> candidates = generateCandidates1();
      int added_seqs_num = countSupport(candidates, sessions);
      if (freq_seqs.size() > 0)
      {
        while (added_seqs_num > 0)
        {
          candidates = generateCandidates();
          if (candidates.size() > 0)
            added_seqs_num = countSupport(candidates, sessions);
          else
            break;
        }
      }
    }

    gen_time /= ITERS;
    count_time /= ITERS;
    qDebug() << "Method time: " << (gen_time.count()+count_time.count()) / 1000000000. << " s";
    qDebug() << "Gen    time: " << gen_time.count() / 1000000000. << " s";
    qDebug() << "Count  time: " << count_time.count() / 1000000000. << " s";
    res[0] += "(" + QString::number(min_support) + ", " + QString::number((gen_time.count()+count_time.count()) / 1000000000.) + ")";
    res[1] += "(" + QString::number(min_support) + ", " + QString::number(gen_time.count() / 1000000000.) + ")";
    res[2] += "(" + QString::number(min_support) + ", " + QString::number(count_time.count() / 1000000000.) + ")";
    min_support -= 0.01;
  }
  for (int i = 0; i < 3; i++)
    qDebug() << res[i];
}

void Calculator::prepareGSP()
{
  shared_ptr<DataBase> db = DataBase::instance();
  if (db->getCmdsMap(cmds_map) != OK)
    throw db->lastError();

  if (db->getSessionsNum(sessions_count) != OK)
    throw db->lastError();
}

QList<Sequence> Calculator::generateCandidates1()
{
  chrono::time_point<Clock> start = Clock::now();

  //method
  QList<Sequence> candidates;
  for (int i = 0; i < this->cmds_map.size(); i++)
    candidates.push_back(Sequence(i));

  chrono::time_point<Clock> end = Clock::now();
  chrono::nanoseconds diff = chrono::duration_cast<chrono::nanoseconds>(end - start);
  gen_time += diff;
  //qDebug() << "generateCandidates1() time: " << diff.count() / 1000000000. << " s";
  //qDebug() << "gen-d" << candidates.size() << "candidates";

  return candidates;
}

QList<Sequence> Calculator::generateCandidates()
{
  chrono::time_point<Clock> start = Clock::now();

  //method
  QList<Sequence> candidates;
  //Join
  for (const Sequence &seq1 : cur_freq_seqs)
    for (const Sequence &seq2 : cur_freq_seqs)
      if (Sequence::isJoinable(seq1, seq2) && (same_cmds || seq1 != seq2))
        candidates.push_back(Sequence::join(seq1, seq2)); // <(x),(y)>

  chrono::time_point<Clock> end = Clock::now();
  chrono::nanoseconds diff = chrono::duration_cast<chrono::nanoseconds>(end - start);
  gen_time += diff;
  //qDebug() << "generateCandidates() time: " << diff.count() / 1000000000. << " s";
  //qDebug() << "gen-d" << candidates.size() << "candidates";

  return candidates;
}

bool Calculator::findCommand(int cmd, const Session &session, int min_time, int prev_cmd_id, int& time, int &id) const
{
  const QList<forward_list<pair<int,int>>> nodes_list = session.getRepresintationNodesList();
  int item_id = cmd;
  forward_list<pair<int,int>>::const_iterator it = nodes_list[item_id].cbegin();
  if (it._M_node == NULL)
    return false;
  time = -1;
  id = -1;
  while (it._M_node != NULL && it->first < min_time)
    it++;

  //Если id предыдущей команды больше чем id найденной команды, то идем дальше, т.к. не соблюден порядок, хоть и время одинаковое
  if (prev_cmd_id >= 0)
    while (it._M_node != NULL && prev_cmd_id >= it->second)
      it++;

  if (it._M_node == NULL)
    return false;
  else
  {
    time = it->first;
    id = it->second;
    return true;
  }
}

bool Calculator::sessionSupportsSequence(const Session& session, const Sequence& seq)
{
  bool res = false;
  int phase = 1;
  int cur_id = 0;
  int times[seq.size()];
  int new_time;
  int new_id;
  int cmds[seq.size()];
  int ids[seq.size()];

  while (phase != 0)
  {
    if (cur_id == seq.size())
    {
      res = true;
      break;
    }
    if (phase == 1) //Forward phase
    {
      int prev_cmd_id = cur_id == 0 ? -1 : ids[cur_id - 1];
      int min_time = cur_id == 0 ? 0 : times[cur_id - 1] + min_gap;
      bool is_founded = findCommand(seq[cur_id], session, min_time, prev_cmd_id, new_time, new_id);
      if (is_founded)
      {
        times[cur_id] = new_time;
        cmds[cur_id] = seq[cur_id];
        ids[cur_id] = new_id;
      }
      else // no command in session
      {
        res = false;
        phase = 0;
        break;
      }
      if (cur_id == 0 || new_time - times[cur_id - 1] <= max_gap) //ok
      {
        cur_id++;
        phase = 1;
      }
      else // switch to backward phase
        phase = 2;
    }
    else if (phase == 2) //Backward phase
    {
      if (cur_id == 0)
        phase = 1;
      else
      {
        int prev_cmd_id = cur_id < 2 ? -1 : ids[cur_id - 2];
        int min_time = times[cur_id] - max_gap;
        bool is_founded = findCommand(seq[cur_id - 1], session, min_time, prev_cmd_id, new_time, new_id);
        if (is_founded)
        {
          times[cur_id - 1] = new_time;
          cmds[cur_id - 1] = seq[cur_id - 1];
          ids[cur_id - 1] = new_id;
        }
        else // no command in session
        {
          res = false;
          phase = 0;
          break;
        }
        if (cur_id - 1 == 0 || new_time - times[cur_id - 2] <= max_gap) //ok
        {
          if (times[cur_id] - new_time >= min_gap && new_id < ids[cur_id])
            cur_id++;
          phase = 1;
        }
        else // continue backward phase
        {
          cur_id--;
          phase = 2;
        }
      }
    }
    else //eror phase, shouldn't be here
      break;
  }

  return res;
}

Sequence Calculator::findFreqSequenceByCommand(int cmd)
{
  for (int i = 0; i < freq_seqs.size(); i++)
  {
    if (freq_seqs[i] == cmd)
      return freq_seqs[i];
  }
  return Sequence();
}

double Calculator::calcLift(Sequence seq)
{
  double res = seq.support;
  for (int i = 0; i < seq.size(); i++)
  {
    Sequence subseq = findFreqSequenceByCommand(seq[i]);
    if (subseq.size() < 1)
      return -1;
    res /= subseq.support;
  }
  return res;
}

int Calculator::countSupport(QList<Sequence> &candidates, const QList<Session>& sessions)
{
  chrono::time_point<Clock> start = Clock::now();

  //method
  int res = 0;
  cur_freq_seqs.clear();
  for (Sequence& candidate : candidates)
  {
    for (const Session& session : sessions)
    {
      if (sessionSupportsSequence(session, candidate))
        candidate.support++;
    }
    candidate.support = candidate.support / sessions_count;
    //candidate.confidence = calcConfidence(candidate);
    candidate.lift = calcLift(candidate);
    if (candidate.support >= this->min_support)
    {
      freq_seqs.push_back(candidate);
      cur_freq_seqs.push_back(candidate);
      res++;
    }
  }

  chrono::time_point<Clock> end = Clock::now();
  chrono::nanoseconds diff = chrono::duration_cast<chrono::nanoseconds>(end - start);
  count_time += diff;
  //qDebug() << "countSupport() time: " << diff.count() / 1000000000. << " s";
  //qDebug() << "added" << res << "sequences";

  return res;
}

void Calculator::sortFrequentSequences()
{
  sort(freq_seqs.begin(), freq_seqs.end(), greater<>());
}
