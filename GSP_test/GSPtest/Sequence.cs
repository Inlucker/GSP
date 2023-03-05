using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GSPtest
{
  class Sequence
  {
    private List<ItemGroup> list = new List<ItemGroup>(0);
    public double support { get; set; }
    public double confidence { get; set; }
    public double lift { get; set; }
    public Sequence()
    {
    }
    public Sequence(List<List<int>> list) //For Testing
    {
      foreach (List<int> item_group in list)
        this.list.Add(new ItemGroup(item_group));
    }
    public Sequence(List<ItemGroup> list) //For Testing
    {
      foreach (ItemGroup item_group in list)
        this.list.Add(new ItemGroup(item_group));
    }
    public Sequence(int _item)
    {
      this.list.Add(new ItemGroup(_item));
    }
    public Sequence(Sequence _seq)
    {
      foreach (ItemGroup item_group in _seq.list)
        this.list.Add(new ItemGroup(item_group));
    }
    public static bool operator ==(Sequence seq1, Sequence seq2)
    {
      if (seq1.list.Count != seq2.list.Count)
        return false;
      else
        for (int i = 0; i < seq1.list.Count; i++)
          if (seq1.list[i] != seq2.list[i])
            return false;
      return true;
    }
    public static bool operator !=(Sequence seq1, Sequence seq2)
    {
      return !(seq1 == seq2);
    }
    public int CompareTo(Sequence seq2)
    {
      if (this.lift != seq2.lift)
        return -this.lift.CompareTo(seq2.lift);
      else
        return -this.support.CompareTo(seq2.support);
    }

    public ItemGroup this[int i] //operator []
    {
      get { return list[i]; }
    }
    public class Enumerator
    {
      private List<ItemGroup> list { get; }
      int cur_id = -1;
      public Enumerator(List<ItemGroup> _list)
      {
        this.list = _list;
      }
      public bool MoveNext()
      {
        if (this.cur_id < list.Count - 1)
        {
          this.cur_id++;
          return true;
        }
        else
          return false;
      }
      public object Current
      {
        get { return this.list[cur_id]; }
      }
    }
    public Enumerator GetEnumerator()
    {
      return new Enumerator(this.list);
    }
    public override string ToString()
    {
      string res = "<";
      foreach (ItemGroup gr in this)
      {
        res += "(";
        foreach (int item in gr)
          res += item + ",";
        res = res.Remove(res.Length-1, 1);
        res += "),";
      }
      res = res.Remove(res.Length-1, 1);
      res += ">";
      return res;
    }
    public int Size()
    {
      return this.list.Count;
    }
    public Sequence DropFirstItem()
    {
      Sequence res = new Sequence(this.list);
      res.list[0] = res.list[0].DropFirst();
      if (res.list[0].Size() == 0)
        res.list.RemoveAt(0);
      return res;
    }
    public Sequence DropLastItem()
    {
      Sequence res = new Sequence(this.list);
      res.list[res.list.Count - 1] = res.list[res.list.Count - 1].DropLast();
      if (res.list[res.list.Count - 1].Size() == 0)
        res.list.RemoveAt(res.list.Count - 1);
      return res;
    }
    int GetLastItem()
    {
      return this.list[this.list.Count - 1].GetLast();
    }
    void AddLastGroupFrom(Sequence s2)
    {
      int s1_last = this.list.Count - 1;
      int s2_last = s2.list.Count - 1;
      if (s2.list[s2_last].Size() > 1)
      {
        this.list[s1_last].Add(s2.GetLastItem());
      }
      else if (s2.list[s2_last].Size() == 1)
      {
        this.list.Add(new ItemGroup(s2.GetLastItem()));
      }
      else
      {
        //Error
      }
    }
    public static bool IsJoinable(Sequence s1, Sequence s2)
    {
      Sequence subs1 = s1.DropFirstItem();
      Sequence subs2 = s2.DropLastItem();
      return subs1 == subs2;
    }
    public static Sequence Join(Sequence s1, Sequence s2)
    {
      Sequence res = new Sequence(s1);
      res.AddLastGroupFrom(s2);
      return res;
    }
    public static Sequence Join1(Sequence s1, Sequence s2)
    {
      if (s1.list.Count != 1 || s2.list.Count != 1)
        return new Sequence();
      Sequence res = new Sequence(s1);
      res.list[0].Add(s2.list[0].GetFirst());
      return res;
    }
    public bool IsSubSequenceOf(Sequence s2)
    {
      return this.DropFirstItem() == s2 || this.DropLastItem() == s2;
    }
    public List<Sequence> GetSubSequences()
    {
      List<Sequence> sub_seqs = new List<Sequence>(0);
      for (int i = 0; i < this.Size(); i++)
      {
        if (this.list[i].Size() > 1)
        {
          for (int j = 0; j < this.list[i].Size(); j++)
          {
            Sequence sub_seq = new Sequence(this);
            sub_seq.list[i] = sub_seq.list[i].DropAt(j);
            sub_seqs.Add(sub_seq);
          }
        }
      }
      return sub_seqs;
    }
  }
}
