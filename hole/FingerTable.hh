#ifndef HOLE_FINGERTABLE_HH
# define HOLE_FINGERTABLE_HH

namespace hole
{
  class Node;
  class LocalNode;

  class FingerTable
  {
  public:
    FingerTable(LocalNode & localNode);
    ~FingerTable();

    /** initialize the finger table by filling it with the local node */
    void init();
    /** return you predecessor, it be you if you're alone */
    inline Node & predecessor();
    /** return you predecessor, it be you if you're alone */
    inline Node & successor();
    /** return at index. -1 returns you predecessor, and 0 is the local node */
    inline Node & operator[] (int index);

  protected:
    LocalNode & localNode_;
    Node *      successors_[160];
    Node *      predecessor_;
  };
}

#endif /* !HOLE_FINGERTABLE_HH */
