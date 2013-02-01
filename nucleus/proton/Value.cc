#include <nucleus/proton/Value.hh>
#include <nucleus/Exception.hh>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Construction |
    `-------------*/

    Value::Value():
      Node()
    {
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(Value, Node)
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
        throw Exception("unable to dump the node");

      return (elle::Status::Ok);
    }
  }
}
