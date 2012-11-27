#include <nucleus/proton/Value.hh>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Construction |
    `-------------*/

    Value::Value():
      Node(Node::Type::value)
    {
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Value::Dump(elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[Value]" << std::endl;

      if (Node::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the node");

      return (elle::Status::Ok);
    }
  }
}
