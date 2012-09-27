#include <nucleus/proton/Transcript.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- construction ----------------------------------------------------
//

    Transcript::~Transcript()
    {
      for (auto action: this->_container)
        delete action;

      this->_container.clear();
    }

//
// ---------- methods ---------------------------------------------------------
//

    void
    Transcript::push(Address const& address,
                     Block const* block)
    {
      this->_container.push_back(new Action(address, block));
    }

    void
    Transcript::wipe(Address const& address)
    {
      this->_container.push_back(new Action(address));
    }


//
// ---------- dumpable --------------------------------------------------------
//

    elle::Status        Transcript::Dump(const elle::Natural32  margin) const
    {
      elle::String              alignment(margin, ' ');

      std::cout << alignment << "[Transcript]" << std::endl;

      for (auto action: this->_container)
        {
          if (action->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the action");
        }

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Transcript::print(std::ostream& stream) const
    {
      stream << "transcript(";

      for (auto action: this->_container)
        stream << action->type() << " " << action->address();

      stream << ")";
    }

//
// ---------- iterable --------------------------------------------------------
//

    typename Transcript::Scoutor
    Transcript::begin() const
    {
      return (this->_container.begin());
    }

    typename Transcript::Scoutor
    Transcript::end() const
    {
      return (this->_container.end());
    }

  }
}
