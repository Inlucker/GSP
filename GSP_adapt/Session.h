#ifndef SESSION_H
#define SESSION_H

#include "Command.h"
#include <QList>
#include <forward_list>

using namespace std;

class Session
{
public:
  Session(int _id);

  void setCommands(QList<Command> _commands, int items_num = 0);
  const QList<forward_list<pair<int,int>>>& getRepresintationNodesList() const;

private:
  void makeRepresintaion(int item_num = 0);

private:
  int id;
  QList<Command> commands;
  QList<forward_list<pair<int,int>>> repr;
};

#endif // SESSION_H
