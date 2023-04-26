#include "GSP.h"
#include <chrono>

typedef chrono::high_resolution_clock Clock;

GSP::GSP(QString db_name)
{
  if (DataBase::createSQLiteDataBase(db_name) != OK) //ToDO где лучше создавть БД?
    throw DataBase::lastError();
  if (DataBase::resetSQLiteDataBase() != OK)
    throw DataBase::lastError();
}

QList<Sequence> GSP::getFrequentSequences()
{
  prepareGSP();
  QList<Session> sessions;
  DataBase::getAllLogs(cmds_map.size(), sessions);

  this->freq_seqs.clear();
  QList<Sequence> candidates = generateCandidates1();
  int added_seqs_num = countSupport(candidates, sessions);
  if (freq_seqs.size() > 0)
  {
    while (added_seqs_num > 0)
    {
      candidates = generateCandidates();
      qDebug() << "generateCandidates() candidates.size() = " << candidates.size();
      if (candidates.size() > 0)
        added_seqs_num = countSupport(candidates, sessions);
      else
        break;
      qDebug() << "countSupport() added_seqs_num = " << added_seqs_num;
    }
  }
  sortFrequentSequences();
  return this->freq_seqs;
}

/*QList<Sequence> GSP::getFrequentSequences2(const QList<Session>& sessions)
{
  this->freq_seqs.clear();
  QList<Sequence> candidates = generateCandidates1();
  int added_seqs_num = countSupport2(candidates, sessions);
  if (freq_seqs.size() > 0)
  {
    while (added_seqs_num > 0)
    {
      candidates = generateCandidates();
      if (candidates.size() > 0)
        added_seqs_num = countSupport2(candidates, sessions);
      else
        break;
    }
  }
  return this->freq_seqs;
}*/

void GSP::printFrequentSequences()
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

void GSP::test1()
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

/*void GSP::test2()
{
  Session s1(0);
  QList<Command> cmds;
  cmds.push_back(Command(0, 10, 0));
  cmds.push_back(Command(0, 20, 2));
  cmds.push_back(Command(0, 26, 1));
  s1.setCommands(cmds, 3);

  QList<Session> sessions;
  sessions.push_back(s1);
  sessions_count = 1;

  cmds_map[1] = "11";
  cmds_map[2] = "22";
  cmds_map[3] = "33";

  min_gap = 1;
  max_gap = 15;
  min_support = 1;

  QList<Sequence> res = getFrequentSequences2(sessions);
  printFrequentSequences();
}*/

void GSP::test5()
{
  chrono::time_point<Clock> start = Clock::now();
  log_reader.readLogs(".\\logs");
  chrono::time_point<Clock> end = Clock::now();
  chrono::nanoseconds diff = chrono::duration_cast<chrono::nanoseconds>(end - start);
  qDebug() << "readLogs() time: " << diff.count() / 1000000000. << " s";
  min_gap = 0; // 0 1
  max_gap = INT_MAX; // 15 INT_MAX
  min_support = 0.1; // 0.5 0.01

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

void GSP::test6()
{
  log_reader.readLogsWithoutTime(".\\logs_without_time");
  min_gap = 1;
  max_gap = 15;
  min_support = 0.5;

  QList<Sequence> res = getFrequentSequences();
  printFrequentSequences();
}

void GSP::prepareGSP()
{
  if (DataBase::getCmdsMap(cmds_map) != OK)
    throw DataBase::lastError();

  if (DataBase::getSessionsNum(sessions_count) != OK)
    throw DataBase::lastError();
}

QList<Sequence> GSP::generateCandidates1()
{
  QList<Sequence> candidates;
  for (int i = 0; i < this->cmds_map.size(); i++)
    candidates.push_back(Sequence(i));
  return candidates;
}

QList<Sequence> GSP::generateCandidates()
{
  QList<Sequence> candidates;
  //Join
  for (const Sequence &seq1 : cur_freq_seqs)
    for (const Sequence &seq2 : cur_freq_seqs)
      //if (seq1 != seq2 && Sequence::isJoinable(seq1, seq2)) // excludes <(x),(x)>
      if (Sequence::isJoinable(seq1, seq2))
        candidates.push_back(Sequence::join(seq1, seq2)); // <(x),(y)>
  return candidates;
}

bool GSP::findCommand(int cmd, const Session &session, int min_time, int counter, int& time) const
{
  const QList<forward_list<int>> nodes_list = session.getRepresintationNodesList();
  int item_id = cmd;
  forward_list<int>::const_iterator it = nodes_list[item_id].cbegin();
  if (it._M_node == NULL)
    return false;
  time = -1;
  while (it._M_node != NULL && *it < min_time)
    it++;

  while (it._M_node != NULL && counter > 0)
  {
    counter--;
    it++;
  }

  if (it._M_node == NULL)
    return false;
  else
  {
    time = *it;
    return true;
  }
}

bool GSP::sessionSupportsSequence(const Session& session, const Sequence& seq)
{
  bool res = false;
  int phase = 1;
  int cur_id = 0;
  int times[seq.size()];
  int new_time;
  int cmds[seq.size()];

  while (phase != 0)
  {
    if (cur_id == seq.size())
    {
      res = true;
      break;
    }
    if (phase == 1) //Forward phase
    {
      int counter = 0;
      int min_time = cur_id == 0 ? 0 : times[cur_id - 1] + min_gap; //todo optimize

      //if (cur_id > 0 && min_time == times[cur_id - 1])
      if (cur_id > 0)
        for (int i = 0; i < cur_id; i++)
          if (cmds[i] == seq[cur_id] && times[i] >= min_time)
            counter++;

      bool is_founded = findCommand(seq[cur_id], session, min_time, counter, new_time);
      if (is_founded)
      {
        times[cur_id] = new_time;
        cmds[cur_id] = seq[cur_id];
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
        int counter = 0;
        int min_time = times[cur_id] - max_gap;

        //if (cur_id > 1 && min_time == times[cur_id - 2])
        if (cur_id > 0)
          for (int i = 0; i < cur_id + 1; i++)
            if (cmds[i] == seq[cur_id - 1] && times[i] >= min_time)
              counter++;

        bool is_founded = findCommand(seq[cur_id - 1], session, min_time, counter, new_time);
        if (is_founded)
        {
          times[cur_id - 1] = new_time;
          cmds[cur_id - 1] = seq[cur_id - 1];
        }
        else // no command in session
        {
          res = false;
          phase = 0;
          break;
        }
        if (cur_id - 1 == 0 || new_time - times[cur_id - 2] <= max_gap) //ok
        {
          if (times[cur_id] - new_time >= min_gap)
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

Sequence GSP::findFreqSequenceByCommand(int cmd)
{
  for (int i = 0; i < freq_seqs.size(); i++)
  {
    if (freq_seqs[i] == cmd)
      return freq_seqs[i];
  }
  return Sequence();
}

double GSP::calcLift(Sequence seq)
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

int GSP::countSupport(QList<Sequence> &candidates, const QList<Session>& sessions)
{
  //QList<Session> sessions;
  //DataBase::getAllLogs(cmds_map.size(), sessions);

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
  return res;

  /*QList<Session> sessions;
  int cur_session_id = 0;
  sessions.push_back(Session(cur_session_id));

  QList<Command> commands;

  int max_n = 1000;
  QString query_str = "SELECT * FROM logs ORDER BY session_id ASC;";
  QSqlQuery m_query;
  if (!m_query.exec(query_str))
  {
    qDebug() << m_query.lastError().text();
    throw m_query.lastError().text();
  }

  while (m_query.isActive())
  {
    int counter = 0;
    while (counter < max_n && m_query.next())
    {
      int session_id = m_query.value(1).toInt();
      QDateTime time = m_query.value(2).toDateTime();
      int name = m_query.value(3).toString().toInt();

      if (session_id > cur_session_id)
      {
        sessions[cur_session_id].setCommands(commands, cmds_map.size()); //add items_num
        cur_session_id = session_id;
        commands.clear();
      }

      commands.append(Command(session_id, time.toSecsSinceEpoch(), name));

      counter++;
    }
    if (!m_query.isActive())
    {
      sessions[cur_session_id].setCommands(commands); //add items_num
      commands.clear();
    }

    for (Sequence& candidate : candidates)
    {
      for (const Session& session : sessions)
      {
        if (sessionSupportsSequence(session, candidate))
          candidate.support++;
      }
    }

  }

  int res = 0;
  cur_freq_seqs.clear();
  for (Sequence& candidate : candidates)
  {
    candidate.support = candidate.support / sessions_count;
    //candidate.confidence = calcConfidence(candidate);
    //candidate.lift = calcLift(candidate);
    if (candidate.support >= this->min_support)
    {
      freq_seqs.push_back(candidate);
      cur_freq_seqs.push_back(candidate);
      res++;
    }
  }

  return res;*/
}

/*int GSP::countSupport2(QList<Sequence>& candidates, const QList<Session> &sessions)
{
  int res = 0;
  cur_freq_seqs.clear();
  for (Sequence& candidate : candidates)
  {
    for (const Session& session : sessions)
    {
      if (sessionSupportsSequence(session, candidate))
        candidate.support++;
    }
    candidate.support = candidate.support / sessions.size();
    //candidate.confidence = calcConfidence(candidate);
    //candidate.lift = calcLift(candidate);
    if (candidate.support >= this->min_support)
    {
      freq_seqs.push_back(candidate);
      cur_freq_seqs.push_back(candidate);
      res++;
    }
  }
  return res;
}*/

void GSP::sortFrequentSequences()
{
  //sort(freq_seqs.begin(), freq_seqs.end(), sequenceGreaterThan);
  sort(freq_seqs.begin(), freq_seqs.end(), greater<>());
}
