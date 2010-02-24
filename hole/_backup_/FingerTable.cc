#include "hole/Node.hh"
#include "hole/FingerTable.hh"

namespace hole
{
  FingerTable::FingerTable(Node & localNode)
    : localNode_(localNode),
      successors_(),
      predecessor_(&localNode)
  {
    Init();
  }

  void FingerTable::Init()
  {
    for (int i = 0; i < 160; i++)
      successors_[i] = &localNode_;
    predecessor_ = & localNode_;
  }

  FingerTable::~FingerTable()
  {
  }
}
