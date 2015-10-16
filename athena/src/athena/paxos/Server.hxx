#ifndef ATHENA_PAXOS_SERVER_HXX
# define ATHENA_PAXOS_SERVER_HXX

# include <elle/With.hh>
# include <elle/serialization/Serializer.hh>

# include <reactor/Scope.hh>

namespace athena
{
  namespace paxos
  {
    /*------.
    | Types |
    `------*/

    template <typename T, typename ClientId>
    bool
    Server<T, ClientId>::Proposal::operator <(Proposal const& rhs) const
    {
      if (this->round < rhs.round)
        return true;
      return this->sender < rhs.sender;
    }

    template <typename T, typename ClientId>
    void
    Server<T, ClientId>::Proposal::serialize(elle::serialization::Serializer& s)
    {
      s.serialize("round", this->round);
      s.serialize("sender", this->sender);
    }

    template <typename T, typename ClientId>
    void
    Server<T, ClientId>::Accepted::serialize(elle::serialization::Serializer& s)
    {
      s.serialize("proposal", this->proposal);
      s.serialize("value", this->value);
    }

    /*--------.
    | Printer |
    `--------*/

    template <typename T>
    struct Printer
    {
      Printer(T const& o_)
        : o(o_)
      {}

      T const& o;
    };

    template <typename T>
    Printer<T>
    printer(T const& o)
    {
      return Printer<T>(o);
    }

    template <typename T>
    std::ostream&
    operator <<(std::ostream& output, Printer<std::shared_ptr<T>> const& p)
    {
      output << *p.o;
      return output;
    }

    template <typename T>
    std::ostream&
    operator <<(std::ostream& output, Printer<std::unique_ptr<T>> const& p)
    {
      output << *p.o;
      return output;
    }

    template <typename T>
    std::ostream&
    operator <<(std::ostream& output, Printer<T> const& p)
    {
      output << p.o;
      return output;
    }

    /*-------------.
    | Construction |
    `-------------*/

    template <typename T, typename ClientId>
    Server<T, ClientId>::Server()
      : _accepted()
      , _has_accepted()
      , _chosen(false)
    {}

    template <typename T, typename ClientId>
    boost::optional<typename Server<T, ClientId>::Accepted>
    Server<T, ClientId>::propose(Proposal const& p)
    {
      ELLE_LOG_COMPONENT("athena.paxos.Server");
      ELLE_TRACE_SCOPE("%s: get proposal: %s ", *this, p);
      if (!this->_minimum || this->_minimum < p)
      {
        ELLE_DEBUG("%s: update minimum proposal", *this);
        this->_minimum = p;
      }
      return this->_accepted;
    }

    template <typename T, typename ClientId>
    typename Server<T, ClientId>::Proposal
    Server<T, ClientId>::accept(Proposal const& p,
                                T const& value)
    {
      ELLE_LOG_COMPONENT("athena.paxos.Server");
      ELLE_TRACE_SCOPE("%s: accept for %s: %s",
                       *this, p, printer(value));
      if (!(p < this->_minimum))
      {
        if (!this->_accepted)
          this->_accepted.emplace();
        this->_accepted->proposal = p;
        this->_accepted->value = value;
        this->_has_accepted.open();
      }
      ELLE_ASSERT(this->_minimum);
      return this->_minimum.get();
    }

    /*----------.
    | Printable |
    `----------*/

    template <typename T, typename ClientId>
    void
    Server<T, ClientId>::print(std::ostream& output) const
    {
      elle::fprintf(output, "paxos::Server(%x)", this);
    }
  }
}

#endif
