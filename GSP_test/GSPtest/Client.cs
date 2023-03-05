using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GSPtest
{
  class Client
  {
    int id { get; }
    List<Transaction> transactions = null;
    LinkedList<int>[] repr = null;
    public Client(int _id)
    {
      this.id = _id;
      transactions = new List<Transaction>();
    }
    private void MakeRepresintaion(int item_num = 0)
    {
      if (item_num == 0)
        foreach (Transaction trans in transactions)
          foreach (int item in trans.items)
            if (item_num < item)
              item_num = item;
      repr = new LinkedList<int>[item_num];
      for (int i = 0; i < repr.Length; i++)
        repr[i] = new LinkedList<int>();
      foreach (Transaction trans in transactions)
        foreach (int item in trans.items)
        {
          if (repr[item - 1].Last != null && repr[item - 1].Last.Value > trans.time)
            throw new Exception("Error in transactions: not sorted");
          repr[item - 1].AddLast(trans.time);
        }
    }
    public void SetTransactions(List<Transaction> _transactions, int items_num = 0)
    {
      this.transactions = _transactions;
      transactions.Sort((x, y) => x.time - y.time);
      this.MakeRepresintaion(items_num);
    }
    public LinkedListNode<int>[] GetRepresintationNodesList()
    {
      LinkedListNode<int>[] nodes_list = new LinkedListNode<int>[this.repr.Length];
      for (int i = 0; i < this.repr.Length; i++)
        nodes_list[i] = this.repr[i].First;
      return nodes_list;
    }
  }
}
