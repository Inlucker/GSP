#include "Command.h"

Command::Command(int _session_id, int _time, int _command_id)
{
  this->session_id = _session_id;
  this->time = _time;
  this->command_id = _command_id;
}

bool Command::operator<(const Command &c) const
{
  return this->time < c.time;
}

bool Command::operator>(const Command &c) const
{
  return this->time > c.time;
}
