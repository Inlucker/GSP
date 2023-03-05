#ifndef COMMAND_H
#define COMMAND_H

class Command
{
public:
  Command(int _session_id, int _time, int _command_id);

  bool operator<(const Command& c) const;
  bool operator>(const Command& c) const;

//private:
  int session_id;
  int time;
  int command_id;
};

#endif // COMMAND_H
