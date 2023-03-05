#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <iostream>
#include <string>
#include <QList>

using namespace std;

class Sequence
{
public:
  Sequence();
  Sequence(int item);
  Sequence(initializer_list<int> args);
  Sequence& operator =(initializer_list<int> args);

  bool operator==(const Sequence& seq2) const;
  bool operator!=(const Sequence& seq2) const;
  int operator [](int id) const;
  bool operator<(const Sequence& seq2) const;
  bool operator>(const Sequence& seq2) const;

  int size() const;

  Sequence dropFirstItem() const;
  Sequence dropLastItem() const;
  int getLastItem() const;
  void addLastGroupFrom(Sequence s2);

  static bool isJoinable(Sequence s1, Sequence s2);
  static Sequence join(const Sequence& s1, const Sequence& s2);

public:
  double support = 0;
  double confidence = -1;
  double lift = -1;

private:
  QList<int> cmds;
};

ostream& operator <<(ostream& os, const Sequence& seq);

#endif // SEQUENCE_H
