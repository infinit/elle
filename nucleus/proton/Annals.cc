#include <nucleus/proton/Annals.hh>
#include <nucleus/proton/Clef.hh>

namespace nucleus
{
  namespace proton
  {
    /*--------.
    | Methods |
    `--------*/

    void
    Annals::record(std::unique_ptr<Clef const>&& clef)
    {
      this->_container.push_back(std::move(clef));
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Annals::print(std::ostream& stream) const
    {
      stream << "#" << this->_container.size();
    }

    /*---------.
    | Iterable |
    `---------*/

    typename Annals::Scoutor
    Annals::begin() const
    {
      return (this->_container.begin());
    }

    typename Annals::Scoutor
    Annals::end() const
    {
      return (this->_container.end());
    }
  }
}
