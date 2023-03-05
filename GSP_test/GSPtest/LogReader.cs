using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace GSPtest
{
  class Action
  {
    public Action(int _time, string _command)
    {
      this.time = _time;
      this.command = _command;
    }
    public int time { get; set; }
    public string command { get; set; }
  }
  class LogReader
  {
    public List<string> commands = new List<string>();
    public List<List<Action>> sessions = new List<List<Action>>();
    public void ReadLog(string file_name)
    {
      using (StreamReader stream = new StreamReader(file_name))
      {
        List<Action> log = new List<Action>();
        string line = "";
        while ((line = stream.ReadLine()) != null)
        {
          int time = GetTimeFromRecord(line);
          if (time > 0)
          {
            if (GetCommandFromRecord(line, out string cmd))
            {
              bool cmd_exists = false;
              foreach (string c in commands)
                if (c == cmd)
                {
                  cmd_exists = true;
                  break;
                }
              if (!cmd_exists)
                commands.Add(cmd);
              log.Add(new Action(time, cmd));
            }
          }
        }
        sessions.Add(log);
      }
    }
    public void ReadLog2(string file_name)
    {
      using (StreamReader stream = new StreamReader(file_name))
      {
        List<Action> log = new List<Action>();
        string line = "";
        while ((line = stream.ReadLine()) != null)
        {
          int time = GetTimeFromRecord(line);
          if (time > 0)
          {
            if (GetCommandFromRecord(line, out string cmd))
            {
              bool cmd_exists = false;
              foreach (string c in commands)
                if (c == cmd)
                {
                  cmd_exists = true;
                  break;
                }
              if (!cmd_exists)
                commands.Add(cmd);
              log.Add(new Action(time, cmd));
              if (cmd == "Exit")
              {
                sessions.Add(log);
                log = new List<Action>();
              }
            }
          }
        }
        sessions.Add(log);
      }
    }
    public void ReadDir(string path)
    {
      string[] files = Directory.GetFiles(path, "*.log");
      foreach (string file in files)
        this.ReadLog2(file);
    }
    public void GetDataForGPS(out List<Client> clients, out Dictionary<int, string> item_map)
    {
      clients = new List<Client>();
      item_map = new Dictionary<int, string>();
      int id = 1;
      foreach (string cmd in commands)
        item_map.Add(id++, cmd);
      for (int i = 0; i < sessions.Count; i++)
      {
        List<Transaction> trans = new List<Transaction>();
        foreach (Action act in sessions[i])
          trans.Add(new Transaction(i, act.time, new ItemGroup(item_map.FirstOrDefault(x => x.Value == act.command).Key)));
        Client client = new Client(i);
        client.SetTransactions(trans, item_map.Count);
        clients.Add(client);
      }
    }
    private int GetTimeFromRecord(string r)
    {
      if (r.Length < 20)
        return -1;
      if (!Int32.TryParse(r.Substring(0, 4), out int year))
        return -1;
      if (!Int32.TryParse(r.Substring(5, 2), out int month))
        return -1;
      if (!Int32.TryParse(r.Substring(8, 2), out int day))
        return -1;
      if (!Int32.TryParse(r.Substring(11, 2), out int hours))
        return -1;
      if (!Int32.TryParse(r.Substring(14, 2), out int minutes))
        return -1;
      if (!Int32.TryParse(r.Substring(17, 2), out int seconds))
        return -1;

      int res = seconds + minutes * 60 + hours * 60 * 60 + day * 24 * 60 * 60;
      return res;
    }
    private bool GetCommandFromRecord(string r, out string res)
    {
      res = "";
      int start = r.IndexOf('<')+1;
      int end = r.IndexOf('>');
      if (start == 0 || end == 0)
        return false;
      res = r.Substring(start, end - start);
      return true;
    }
  }
}
