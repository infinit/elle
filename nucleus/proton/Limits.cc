#include <nucleus/proton/Limits.hh>

#include <iostream>

namespace nucleus
{
  namespace proton
  {
    namespace limits
    {
//
// ---------- porcupine -------------------------------------------------------
//

      /*----------.
      | Printable |
      `----------*/

      void
      Porcupine::print(std::ostream& stream) const
      {
        // XXX
      }

//
// ---------- node ------------------------------------------------------------
//

      /*-------------.
      | Construction |
      `-------------*/

      Node::Node(Extent const extent,
                 Contention const contention,
                 Balancing const balancing):
        _extent(extent),
        _contention(contention),
        _balancing(balancing)
      {
      }

      /*----------.
      | Printable |
      `----------*/

      void
      Node::print(std::ostream& stream) const
      {
        stream << "{"
               << this->_extent
               << ", "
               << this->_contention
               << ", "
               << this->_balancing
               << "}";
      }
    }

//
// ---------- limits ----------------------------------------------------------
//

    /*-------------.
    | Construction |
    `-------------*/

    Limits::Limits(limits::Porcupine const& porcupine,
                   limits::Node const& nodule,
                   limits::Node const& value):
      _porcupine(porcupine),
      _nodule(nodule),
      _value(value)
    {
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Limits::print(std::ostream& stream) const
    {
      stream << "("
             << this->_porcupine
             << ", "
             << this->_nodule
             << ", "
             << this->_value
             << ")";
    }
  }
}
