using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GSPtest
{
  /*class Item
  {
    int id;
    public Item(int _id)
    {
      this.id = _id;
    }
    public static bool operator ==(Item item1, Item item2)
    {
      return item1.id == item2.id;
    }
    public static bool operator !=(Item item1, Item item2)
    {
      return item1.id != item2.id;
    }
  }*/
  class ItemGroup
  {
    private List<int> items { get; set; }
    /*public ItemGroup()
    {
      this.items = new List<int>(0);
    }*/
    public ItemGroup(int _item)
    {
      this.items = new List<int> { _item };
    }
    public ItemGroup(List<int> _items) //For Testing
    {
      this.items = new List<int>(0);
      foreach (int id in _items)
        this.items.Add(id);
    }
    public ItemGroup(ItemGroup _item_group)
    {
      this.items = new List<int>(0);
      foreach (int id in _item_group.items)
        this.items.Add(id);
    }
    public static bool operator ==(ItemGroup gr1, ItemGroup gr2)
    {
      if (gr1.items.Count != gr2.items.Count)
        return false;
      else
        for (int i = 0; i < gr1.items.Count; i++)
          if (gr1.items[i] != gr2.items[i])
            return false;
      return true;
    }
    public static bool operator !=(ItemGroup gr1, ItemGroup gr2)
    {
      return !(gr1 == gr2);
    }
    public int this[int i] //operator []
    {
      get { return items[i]; }
    }
    public class Enumerator
    {
      private List<int> items { get; }
      int cur_id = -1;
      public Enumerator(List<int> _items)
      {
        this.items = _items;
      }
      public bool MoveNext()
      {
        if (this.cur_id < items.Count-1)
        {
          this.cur_id++;
          return true;
        }
        else
          return false;
      }
      public object Current
      {
        get { return this.items[cur_id]; }
      }
    }
    public Enumerator GetEnumerator()
    {
      return new Enumerator(this.items);
    }
    public override string ToString()
    {
      string res = "(";
      foreach (int item in this)
        res += item + ",";
      res = res.Remove(res.Length - 1, 1);
      res += ")";
      return res;
    }
    public int Size()
    {
      return this.items.Count;
    }
    public void Add(int _item)
    {
      this.items.Add(_item);
    }
    public ItemGroup DropFirst()
    {
      ItemGroup res = new ItemGroup(this.items);
      res.items.RemoveAt(0);
      return res;
    }
    public ItemGroup DropAt(int n)
    {
      ItemGroup res = new ItemGroup(this.items);
      res.items.RemoveAt(n);
      return res;
    }
    public ItemGroup DropLast()
    {
      ItemGroup res = new ItemGroup(this.items);
      res.items.RemoveAt(this.items.Count - 1);
      return res;
    }
    public int GetFirst()
    {
      return this.items[0];
    }
    public int GetLast()
    {
      return this.items[this.items.Count - 1];
    }
  }
}
