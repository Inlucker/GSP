#include "Sequence.h"

Sequence::Sequence()
{

}

Sequence::Sequence(int item)
{
  cmds.push_back(item);
}

Sequence::Sequence(initializer_list<int> args)
{
  for (auto &elem : args)
    this->cmds.push_back(elem);
}

Sequence &Sequence::operator =(initializer_list<int> args)
{
  cmds.clear();
  for (auto &elem : args)
    this->cmds.push_back(elem);
  return *this;
}

bool Sequence::operator==(const Sequence &seq2) const
{
  if (this->cmds.size() != seq2.cmds.size())
    return false;
  else
    for (int i = 0; i < this->cmds.size(); i++)
      if (this->cmds[i] != seq2.cmds[i])
        return false;
  return true;
}

bool Sequence::operator!=(const Sequence &seq2) const
{
  return !(*this == seq2);
}

int Sequence::operator[](int id) const
{
  return cmds[id];
}

bool Sequence::operator<(const Sequence &seq2) const
{
  if (this->lift != seq2.lift)
    return this->lift < seq2.lift;
  else
    return this->support < seq2.support;
}

bool Sequence::operator>(const Sequence &seq2) const
{
  if (this->lift != seq2.lift)
    return this->lift > seq2.lift;
  else
    return this->support > seq2.support;
}

int Sequence::size() const
{
  return cmds.size();
}

Sequence Sequence::dropFirstItem() const
{
  Sequence res = *this;
  res.cmds.removeAt(0);
  return res;
}

Sequence Sequence::dropLastItem() const
{
  Sequence res = *this;
  res.cmds.removeAt(res.cmds.size() - 1);
  return res;
}

int Sequence::getLastItem() const
{
  return this->cmds[this->cmds.size() - 1];
}

void Sequence::addLastGroupFrom(Sequence s2)
{
  this->cmds.push_back(s2.getLastItem());
}

bool Sequence::isJoinable(Sequence s1, Sequence s2)
{
  Sequence subs1 = s1.dropFirstItem();
  Sequence subs2 = s2.dropLastItem();
  return subs1 == subs2;
}

Sequence Sequence::join(const Sequence &s1, const Sequence &s2)
{
  Sequence res(s1);
  res.support = 0;
  res.confidence = -1;
  res.lift = -1;
  res.addLastGroupFrom(s2);
  return res;
}

ostream& operator <<(ostream& os, const Sequence& seq)
{
  if (seq.size() == 0)
  {
    os << "<>";
    return os;
  }

  os << '<' << to_string(seq[0]);
  for (int i = 1; i < seq.size(); i++)
    os << ", " << to_string(seq[i]);
  os << '>';
  os << " supp = " << seq.support;
  os << endl;

  return os;
}
