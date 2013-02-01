#include <etoile/gear/Transcript.hh>

#include <elle/finally.hh>

namespace etoile
{
  namespace gear
  {
    /*-------------.
    | Construction |
    `-------------*/

    Transcript::Transcript(Transcript&& other)
    {
      for (auto action: other._container)
        this->_container.push_back(action);

      other._container.clear();
    }

    Transcript::~Transcript()
    {
      for (auto action: this->_container)
        delete action;

      this->_container.clear();
    }

    /*--------.
    | Methods |
    `--------*/

    void
    Transcript::record(Action const* action)
    {
      ELLE_FINALLY_ACTION_DELETE(action);

      this->_container.push_back(action);

      ELLE_FINALLY_ABORT(action);
    }

    elle::Size
    Transcript::size() const
    {
      return (static_cast<elle::Size>(this->_container.size()));
    }

    elle::Boolean
    Transcript::empty() const
    {
      return (this->_container.empty());
    }

    void
    Transcript::merge(Transcript&& other)
    {
      // Insert the _other_'s actions.
      for (auto action: other._container)
        this->_container.push_back(action);

      // Clear the container without deleting the actions.
      other._container.clear();
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status        Transcript::Dump(const elle::Natural32  margin) const
    {
      elle::String              alignment(margin, ' ');

      std::cout << alignment << "[Transcript]" << std::endl;

      for (auto action: this->_container)
        if (action->Dump(margin + 2) == elle::Status::Error)
          throw elle::Exception("unable to dump the action");

      return elle::Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Transcript::print(std::ostream& stream) const
    {
      elle::Size i{0};

      stream << "transcript{";

      for (auto action: this->_container)
        {
          stream << *action;

          if (++i != this->size())
            stream << " ";
        }

      stream << "}";
    }

    /*---------.
    | Iterable |
    `---------*/

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
