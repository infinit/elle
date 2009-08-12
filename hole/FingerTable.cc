#include "hole/LocalNode.hh"
#include "hole/FingerTable.hh"

namespace hole
{
  FingerTable::FingerTable(LocalNode & localNode)
    : localNode_(localNode),
      successors_(),
      predecessor_(&localNode)
  {
    init();
  }

  void FingerTable::init()
  {
    for (int i = 0; i < 160; i++)
      successors_[i] = &localNode_;
    predecessor_ = & localNode_;
  }

  FingerTable::~FingerTable()
  {
  }
}
