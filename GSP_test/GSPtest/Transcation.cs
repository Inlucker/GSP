using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GSPtest
{
  class Transaction
  {
    public int client_id { get; }
    public int time { get; }
    public ItemGroup items { get; }
    public Transaction(int _client_id, int _time, ItemGroup _items)
    {
      this.client_id = _client_id;
      this.time = _time;
      this.items = _items;
    }
    public override string ToString()
    {
      int secs = this.time % 60;
      int minutes = this.time / 60 % 60;
      int hours = this.time / (60 * 60) % 24;
      int days = this.time / (60 * 60 * 24);
      string real_time = "day" + days + " " + (hours < 10 ? "0" : "") + hours + ":" + (minutes < 10 ? "0" : "") + minutes + ":" + (secs < 10 ? "0" : "") + secs;
      return "id " + this.client_id + "; time " + this.time + "; items " + this.items.ToString() + "; " + real_time;
    }
  }
}
