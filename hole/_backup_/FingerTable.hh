#ifndef HOLE_FINGERTABLE_HH
# define HOLE_FINGERTABLE_HH

namespace hole
{
  class Node;
  class LocalNode;

  class FingerTable
  {
  public:
    FingerTable(Node & localNode);
    ~FingerTable();

    /** initialize the finger table by filling it with the local node */
    void Init();
    /** return you predecessor, it be you if you're alone */
    inline Node & Predecessor();
    /** return you predecessor, it be you if you're alone */
    inline Node & Successor();
    /** return the node at index. Starts at 0. */
    inline Node & operator[](int index);

  protected:
    Node & localNode_;
    Node * successors_[160];
    Node * predecessor_;
  };
}

# include "hole/FingerTable.hxx"

#endif /* !HOLE_FINGERTABLE_HH */
