#ifndef ATHENA_PAXOS_PAXOS_HXX
# define ATHENA_PAXOS_PAXOS_HXX

namespace athena
{
  namespace paxos
  {
    template <typename T, typename ServerId>
    Paxos<T, ServerId>::Paxos(ServerId id, Peers peers)
      : _id(std::move(id))
      , _peers(std::move(peers))
      , _round(0)
      , _accepted()
      , _has_accepted()
      , _chosen(false)
    {}

    template <typename T, typename ServerId>
    T
    Paxos<T, ServerId>::choose(T const& value)
    {
      ELLE_LOG_COMPONENT("athena.paxos.Paxos");
      ELLE_TRACE_SCOPE("%s: choose %s", *this, value);
      ++this->_round;
      this->propose(this->_id, this->_round);
      for (auto& peer: this->_peers)
        peer->propose(this->_id, this->_round);
      this->accept(this->_id, this->_round, value);
      for (auto& peer: this->_peers)
        peer->accept(this->_id, this->_round, value);
      this->_chosen = true;
      ELLE_TRACE("%s: chose %s", *this, this->_accepted->value);
      return this->_accepted->value;
    }

    template <typename T, typename ServerId>
    T
    Paxos<T, ServerId>::choose()
    {
      ELLE_LOG_COMPONENT("athena.paxos.Paxos");
      ELLE_TRACE_SCOPE("%s: choose", *this);
      reactor::wait(this->_has_accepted);
      if (!this->_chosen)
        return this->choose(this->_accepted->value);
      else
        return this->_accepted->value;
    }

    template <typename T, typename ServerId>
    boost::optional<std::pair<int, T>>
    Paxos<T, ServerId>::propose(ServerId const& sender, int round)
    {
      return {};
    }

    template <typename T, typename ServerId>
    void
    Paxos<T, ServerId>::accept(ServerId const& sender, int round,
                               T const& value)
    {
      ELLE_LOG_COMPONENT("athena.paxos.Paxos");
      ELLE_TRACE_SCOPE("%s: accept %s at round %s from %s",
                       *this, value, round, sender);
      if (!this->_accepted)
        this->_accepted.emplace();
      this->_accepted->sender = sender;
      this->_accepted->round = round;
      this->_accepted->value = value;
      this->_has_accepted.open();
    }

    /*----------.
    | Printable |
    `----------*/

    template <typename T, typename ServerId>
    void
    Paxos<T, ServerId>::print(std::ostream& output) const
    {
      elle::fprintf(output, "Paxos(%s)", this->_id);
    }
  }
}

#endif
