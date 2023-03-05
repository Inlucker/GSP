using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace GSPtest
{
  class GSP
  {
    List<Sequence> freq_seqs = new List<Sequence>();
    List<Sequence> cur_freq_seqs = new List<Sequence>();
    Dictionary<int, string> item_map = new Dictionary<int, string>();
    List<Client> clients = new List<Client>();
    double support = 1;
    int min_gap = 0;
    int max_gap = int.MaxValue;
    int win_size = 0;
    private bool IsFrequentSubSeq(Sequence seq)
    {
      List<Sequence> sub_seqs = seq.GetSubSequences();
      List<bool> flags = new List<bool>(sub_seqs.Count);
      for (int i = 0; i < sub_seqs.Count; i++)
        flags.Add(false);
      for (int i = 0; i < sub_seqs.Count; i++)
        foreach (Sequence freq_seq in cur_freq_seqs)
          if (sub_seqs[i] == freq_seq)
            flags[i] = true;
      bool res = true;
      for (int i = 0; i < flags.Count; i++)
        res = res && flags[i];
      return res;
    }
    private List<Sequence> GenerateCandidates1()
    {
      List<Sequence> candidates = new List<Sequence>(0);
      for (int i = 0; i < this.item_map.Count; i++)
        candidates.Add(new Sequence(i + 1));
      return candidates;
    }
    private List<Sequence> GenerateCandidates()
    {
      List<Sequence> candidates = new List<Sequence>(0);
      //Join
      foreach (Sequence seq1 in cur_freq_seqs)
        foreach (Sequence seq2 in cur_freq_seqs)
          if (Sequence.IsJoinable(seq1, seq2))
            if (seq1 != seq2) // exclude <(x),(x)> sequences
              candidates.Add(Sequence.Join(seq1, seq2)); // <(x),(y)>
      if (cur_freq_seqs[0].Size() == 1) //if prev cur_freq_seqs has 1 elem only
        for (int i = 0; i < cur_freq_seqs.Count; i++)
          for (int j = i + 1; j < cur_freq_seqs.Count; j++)
            if (cur_freq_seqs[i].Size() == 1 && cur_freq_seqs[j].Size() == 1)
              candidates.Add(Sequence.Join1(cur_freq_seqs[i], cur_freq_seqs[j])); // <(x, y)>
      //Prune
      for (int i = 0; i < candidates.Count; i++)
      {
        if (!IsFrequentSubSeq(candidates[i]))
          candidates.Remove(candidates[i--]);
      }
      return candidates;
    }
    private bool FindElement(ItemGroup element, Client client, int time, out int start_time, out int end_time)
    {
      LinkedListNode<int>[] nodes_list = client.GetRepresintationNodesList();

      start_time = int.MaxValue;
      end_time = 0;

      for (int i = 0; i < element.Size(); i++)
      {
        int item_id = element[i] - 1;
        if (nodes_list[item_id] == null)
          return false;
        while (nodes_list[item_id].Value < time)
        {
          if (nodes_list[item_id].Next == null)
            return false;
          nodes_list[item_id] = nodes_list[item_id].Next;
        }
        if (nodes_list[item_id].Value < start_time)
          start_time = nodes_list[item_id].Value;
        if (nodes_list[item_id].Value > end_time)
          end_time = nodes_list[item_id].Value;
      }

      //checking win_size
      while (end_time - start_time > win_size)
      {
        int new_start_time = start_time;
        for (int i = 0; i < element.Size(); i++)
        {
          int item_id = element[i] - 1;

          if (nodes_list[item_id].Value == start_time)
          {
            if (nodes_list[item_id].Next == null)
              return false;
            nodes_list[item_id] = nodes_list[item_id].Next;

            if (nodes_list[item_id].Value > start_time)
              new_start_time = nodes_list[item_id].Value;
            /*if (nodes_list[item_id].Value > end_time)
              end_time = nodes_list[item_id].Value;*/
          }
        }

        start_time = new_start_time;

        if (start_time > end_time)
          return false;
      }

      return true;
    }
    private bool ClientSupportsSequence(Client client, Sequence seq)
    {
      bool res = false;
      int phase = 1;
      int cur_id = 0;
      int[] start_times = new int[seq.Size()];
      int[] end_times = new int[seq.Size()];

      while (phase != 0)
      {
        if (cur_id == seq.Size())
        {
          res = true;
          break;
        }
        if (phase == 1) //Forward phase
        {
          bool is_founded = FindElement(seq[cur_id], client, cur_id == 0 ? 0 : end_times[cur_id - 1] + min_gap, out int new_start_time, out int new_end_time);
          if (is_founded)
          {
            start_times[cur_id] = new_start_time;
            end_times[cur_id] = new_end_time;
          }
          if (!is_founded) // no element in transactions
          {
            res = false;
            phase = 0;
            break;
          }
          else if (cur_id == 0 || new_end_time - start_times[cur_id - 1] < max_gap) //ok
          {
            cur_id++;
            phase = 1;
          }
          else // switch to backward phase
          {
            phase = 2;
          }
        }
        else if (phase == 2) //Backward phase
        {
          if (cur_id == 0)
            phase = 1;
          else
          {
            bool is_founded = FindElement(seq[cur_id - 1], client, end_times[cur_id] - max_gap, out int new_start_time, out int new_end_time);
            if (is_founded)
            {
              start_times[cur_id - 1] = new_start_time;
              end_times[cur_id - 1] = new_end_time;
            }
            if (!is_founded) // no element in transactions
            {
              res = false;
              phase = 0;
              break;
            }
            else if (cur_id - 1 == 0 || new_end_time - start_times[cur_id - 2] < max_gap) //ok
            {
              if (end_times[cur_id] - new_start_time > min_gap)
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
        else //eror phase
        {
          break;
        }
      }

      return res;
    }
    private int CountSupport(List<Sequence> candidates)
    {
      int res = 0;
      cur_freq_seqs = new List<Sequence>();
      foreach (Sequence candidate in candidates)
      {
        int supported_clients = 0;
        foreach (Client client in clients)
        {
          if (ClientSupportsSequence(client, candidate))
            supported_clients++;
        }
        double candidate_support = (double)supported_clients / clients.Count;
        if (candidate_support >= this.support)
        {
          candidate.support = candidate_support;
          candidate.confidence = CalcConfidence(candidate);
          candidate.lift = CalcLift(candidate);
          freq_seqs.Add(candidate);
          cur_freq_seqs.Add(candidate);
          res++;
        }
      }
      return res;
    }
    private Sequence FindFreqSequenceByItemGroup(ItemGroup gr)
    {
      foreach (Sequence seq in freq_seqs)
        if (seq.Size() == 1 && seq[0] == gr)
          return seq;
      return new Sequence();
    }
    private double CalcConfidence(Sequence seq)
    {
      double res = seq.support;
      ItemGroup gr = seq[0];
      Sequence subseq = FindFreqSequenceByItemGroup(gr);
      if (subseq.Size() < 1)
        return -1;
      res /= subseq.support;
      return res;
    }
    private double CalcLift(Sequence seq)
    {
      double res = seq.support;
      foreach (ItemGroup gr in seq)
      {
        Sequence subseq = FindFreqSequenceByItemGroup(gr);
        if (subseq.Size() < 1)
          return -1;
        res /= subseq.support;
      }
      return res;
    }
    public List<Sequence> GetFrequentSequences()
    {
      this.freq_seqs = new List<Sequence>();
      List<Sequence> candidates = GenerateCandidates1();
      int added_seqs_num = CountSupport(candidates);
      if (freq_seqs.Count > 0)
      {
        while (candidates.Count > 0 && added_seqs_num > 0)
        {
          candidates = GenerateCandidates();
          added_seqs_num = CountSupport(candidates);
        }
      }
      SortFrequentSequences();
      return this.freq_seqs;
    }
    public void SortFrequentSequences()
    {
      this.freq_seqs.Sort((x, y) => -x.support.CompareTo(y.support));
      this.freq_seqs.Sort((x, y) => -x.confidence.CompareTo(y.confidence));
      this.freq_seqs.Sort((x, y) => -x.lift.CompareTo(y.lift));
    }
    public int PrintFrequentSequences()
    {
      int max_len = 0;
      for (int i = 0; i < this.freq_seqs.Count; i++)
      {
        string res = "<";
        foreach (ItemGroup gr in this.freq_seqs[i])
        {
          res += "(";
          foreach (int item in gr)
            res += this.item_map[item] + ",";
          res = res.Remove(res.Length - 1, 1);
          res += "),";
        }
        res = res.Remove(res.Length - 1, 1);
        res += ">";
        if (res.Length > max_len)
          max_len = res.Length;

        res += " supp = " + String.Format("{0:F3}", this.freq_seqs[i].support);
        res += " conf = " + (this.freq_seqs[i].confidence != -1 ? String.Format("{0:F3}", this.freq_seqs[i].confidence) : "null ");
        res += " lift = " + (this.freq_seqs[i].lift != -1 ? String.Format("{0:F3}", this.freq_seqs[i].lift) : "null ");

        Console.WriteLine(res);
      }
      return max_len;
    }
    public void WriteFrequentSequencesToFile(string file_name = "gsp_result.txt", int max_len = -1)
    {
      using (StreamWriter file = new StreamWriter(file_name))
      {
        for (int i = 0; i < this.freq_seqs.Count; i++)
        {
          string res = "<";
          foreach (ItemGroup gr in this.freq_seqs[i])
          {
            res += "(";
            foreach (int item in gr)
              res += this.item_map[item] + ",";
            res = res.Remove(res.Length - 1, 1);
            res += "),";
          }
          res = res.Remove(res.Length - 1, 1);
          res += ">";

          while (res.Length < max_len)
            res += " ";

          res += " supp = " + String.Format("{0:F3}", this.freq_seqs[i].support);
          res += " conf = " + (this.freq_seqs[i].confidence != -1 ? String.Format("{0:F3}", this.freq_seqs[i].confidence) : "null ");
          res += " lift = " + (this.freq_seqs[i].lift != -1 ? String.Format("{0:F3}", this.freq_seqs[i].lift) : "null ");

          file.WriteLine(res);
        }
      }
    }
    public void Test1()
    {
      cur_freq_seqs = new List<Sequence>(0);
      cur_freq_seqs.Add(new Sequence(new List<ItemGroup> { new ItemGroup( new List<int> { 1, 2 } ),
                                                           new ItemGroup( new List<int> { 3 } ) }));
      cur_freq_seqs.Add(new Sequence(new List<ItemGroup> { new ItemGroup( new List<int> { 1, 2 } ),
                                                           new ItemGroup( new List<int> { 4 } ) }));
      cur_freq_seqs.Add(new Sequence(new List<ItemGroup> { new ItemGroup( new List<int> { 1 } ),
                                                           new ItemGroup( new List<int> { 3, 4 } ) }));
      cur_freq_seqs.Add(new Sequence(new List<ItemGroup> { new ItemGroup( new List<int> { 1, 3 } ),
                                                           new ItemGroup( new List<int> { 5 } ) }));
      cur_freq_seqs.Add(new Sequence(new List<ItemGroup> { new ItemGroup( new List<int> { 2 } ),
                                                           new ItemGroup( new List<int> { 3, 4 } ) }));
      cur_freq_seqs.Add(new Sequence(new List<ItemGroup> { new ItemGroup( new List<int> { 2 } ),
                                                           new ItemGroup( new List<int> { 3 } ),
                                                           new ItemGroup( new List<int> { 5 } )}));

      List<Sequence> candidates2 = this.GenerateCandidates();
    }
    public void Test2()
    {
      Client c1 = new Client(1);
      List<Transaction> trans1 = new List<Transaction>();
      trans1.Add(new Transaction(1, 10, new ItemGroup(new List<int> { 1, 2 })));
      trans1.Add(new Transaction(1, 25, new ItemGroup(new List<int> { 4, 6 })));
      trans1.Add(new Transaction(1, 45, new ItemGroup(new List<int> { 3 })));
      trans1.Add(new Transaction(1, 50, new ItemGroup(new List<int> { 1, 2 })));
      trans1.Add(new Transaction(1, 65, new ItemGroup(new List<int> { 3 })));
      trans1.Add(new Transaction(1, 90, new ItemGroup(new List<int> { 2, 4 })));
      trans1.Add(new Transaction(1, 95, new ItemGroup(new List<int> { 6 })));
      c1.SetTransactions(trans1, 7);
      min_gap = 5;
      max_gap = 30;
      win_size = 0;

      Sequence seq = new Sequence(new List<ItemGroup> { new ItemGroup( new List<int> { 1, 2 } ),
                                                        new ItemGroup( new List<int> { 3 } ),
                                                        new ItemGroup( new List<int> { 4 } ) });

      bool res = this.ClientSupportsSequence(c1, seq);
    }
    public void Test3()
    {
      Client c1 = new Client(1);
      List<Transaction> trans1 = new List<Transaction>();
      trans1.Add(new Transaction(1, 10, new ItemGroup(new List<int> { 1 })));
      trans1.Add(new Transaction(1, 20, new ItemGroup(new List<int> { 3 })));
      trans1.Add(new Transaction(1, 26, new ItemGroup(new List<int> { 2 })));
      c1.SetTransactions(trans1, 3);
      this.clients = new List<Client>();
      this.clients.Add(c1);
      item_map.Add(1, "11");
      item_map.Add(2, "22");
      item_map.Add(3, "33");
      min_gap = 5; //5 or 10
      max_gap = 15; //15 or 5
      win_size = 0; //0 or 6
      support = 1;

      List<Sequence> res = GetFrequentSequences();
      this.PrintFrequentSequences();
    }
    public void Test4()
    {
      LogReader log_reader = new LogReader();
      log_reader.ReadLog("C:\\Users\\arsen\\AppData\\Roaming\\Nanosoft\\nanoCAD x64 22.0\\nnmsstcs\\2022-09.log");
      log_reader.GetDataForGPS(out this.clients, out this.item_map);
      min_gap = 1;
      max_gap = 15;
      win_size = 0;
      support = 1;

      List<Sequence> res = GetFrequentSequences();
      this.PrintFrequentSequences();
      this.WriteFrequentSequencesToFile();
    }
    public void Test5()
    {
      LogReader log_reader = new LogReader();
      log_reader.ReadDir(".\\logs");
      log_reader.GetDataForGPS(out this.clients, out this.item_map);
      min_gap = 1;
      max_gap = 15;
      win_size = 0;
      support = 0.5;

      List<Sequence> res = GetFrequentSequences();
      int max_len = this.PrintFrequentSequences();
      this.WriteFrequentSequencesToFile("gsp_result.txt", max_len);
    }
  }
}
