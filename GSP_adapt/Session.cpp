#include "Session.h"
#include <iostream>

Session::Session(int _id)
{
  this->id = _id;
}

void Session::setCommands(QList<Command> _commands, int items_num)
{
  this->commands = _commands;
  sort(this->commands.begin(), this->commands.end(), greater<Command>());
  this->makeRepresintaion(items_num);
}

const QList<forward_list<pair<int, int> > > &Session::getRepresintationNodesList() const
{
  return repr;
}

void Session::makeRepresintaion(int item_num)
{
  if (item_num == 0)
    for (const Command& cmd : commands)
      if (item_num < cmd.command_id)
        item_num = cmd.command_id;
  for (int i = 0; i < item_num; i++)
    repr.push_back(forward_list<pair<int,int>>());
  for (const Command& cmd : commands)
    repr[cmd.command_id].push_front(make_pair(cmd.time, cmd.id));

  //Вывод полученной репрезентации сессии
  /*for (int i = 0; i < item_num; i++)
  {
    cout << i << ": ";
    for (const auto & elem : repr[i])
      cout << elem << " ";
    cout << endl;
  }
  cout << endl;*/
}
