#include "Command.h"

Command::Command(int _id, int _session_id, int _time, int _command_id)
{
  this->id = _id;
  this->session_id = _session_id;
  this->time = _time;
  this->command_id = _command_id;
}

bool Command::operator<(const Command &c) const
{
  if (this->time == c.time)
    return this->id < c.id;
  else
    return this->time < c.time;
}

bool Command::operator>(const Command &c) const
{
  if (this->time == c.time)
    return this->id > c.id;
  else
    return this->time > c.time;
}
