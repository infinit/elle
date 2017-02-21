#pragma once

#include <unordered_set>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <elle/attribute.hh>
#include <elle/Error.hh>
#include <elle/Option.hh>
#include <elle/Printable.hh>
#include <elle/serialization/Serializer.hh>

#include <reactor/Barrier.hh>

namespace elle
{
  namespace athena
  {
    namespace paxos
    {
      template <typename T,
                typename Version,
                typename ClientId,
                typename ServerId = ClientId>
      class Server
        : public elle::Printable
      {
      /*------.
      | Types |
      `------*/
      public:
        typedef Server<T, Version, ClientId, ServerId> Self;
        typedef std::unordered_set<ServerId> Quorum;

      /*---------.
      | Proposal |
      `---------*/
      public:
        struct Proposal
        {
          Proposal();
          Proposal(Version version, int round, ClientId sender);
          Version version;
          int round;
          ClientId sender;
          bool
          operator ==(Proposal const& rhs) const;
          bool
          operator <(Proposal const& rhs) const;
          void
          serialize(elle::serialization::Serializer& s);
          typedef elle::serialization_tag serialization_tag;
          friend
          std::ostream&
          operator <<(std::ostream& output,
                      typename Server<T, Version, ClientId>::Proposal const& p)
          {
            elle::fprintf(output, "%f:%f:%f", p.version, p.round, p.sender);
            return output;
          }
        };

      /*---------.
      | Accepted |
      `---------*/
      public:
        struct Accepted
        {
          Accepted(elle::serialization::SerializerIn& s, elle::Version const& v);
          Accepted(Proposal proposal, elle::Option<T, Quorum> value,
                   bool confirmed);
          Proposal proposal;
          elle::Option<T, Quorum> value;
          bool confirmed;
          void
          serialize(elle::serialization::Serializer& s, elle::Version const& v);
          typedef elle::serialization_tag serialization_tag;
          friend
          std::ostream&
          operator <<(std::ostream& output, Accepted const& accepted)
          {
            elle::fprintf(output, "Accepted(%f)", accepted.proposal);
            return output;
          }
        };

      /*------------.
      | WrongQuorum |
      `------------*/
      public:
        class WrongQuorum
          : public elle::Error
        {
        public:
          typedef elle::Error Super;
          WrongQuorum(Quorum expected, Quorum effective);
          WrongQuorum(elle::serialization::SerializerIn& input,
                      elle::Version const& version);
          virtual
          void
          serialize(elle::serialization::Serializer& s,
                    elle::Version const& version) override;
          ELLE_ATTRIBUTE_R(Quorum, expected);
          ELLE_ATTRIBUTE_R(Quorum, effective);
        private:
          void
          _serialize(elle::serialization::Serializer& s,
                     elle::Version const& version);
        };
      private:
        static const elle::serialization::Hierarchy<elle::Exception>::
        Register<WrongQuorum> _register_wrong_quorum_serialization;

      /*-------------.
      | PartialState |
      `-------------*/
      public:
        class PartialState
          : public elle::Error
        {
        public:
          typedef elle::Error Super;
          PartialState(Proposal p);
          PartialState(elle::serialization::SerializerIn& input,
                       elle::Version const& version);
          virtual
          void
          serialize(elle::serialization::Serializer& s,
                    elle::Version const& version) override;
          ELLE_ATTRIBUTE_R(Proposal, proposal);
        private:
          void
          _serialize(elle::serialization::Serializer& s,
                     elle::Version const& version);
        };
      private:
        static const elle::serialization::Hierarchy<elle::Exception>::
        Register<PartialState> _register_partial_state_serialization;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        Server(ServerId id, Quorum quorum,
               elle::Version version = elle::serialization_tag::version);
        ELLE_ATTRIBUTE_R(ServerId, id);
        ELLE_ATTRIBUTE_R(Quorum, quorum);
        ELLE_ATTRIBUTE_R(boost::optional<T>, value);
        ELLE_ATTRIBUTE_R(elle::Version, version);
        ELLE_ATTRIBUTE_R(bool, partial);

      /*----------.
      | Consensus |
      `----------*/
      public:
        boost::optional<Accepted>
        propose(Quorum q, Proposal p);
        Proposal
        accept(Quorum q, Proposal p, elle::Option<T, Quorum> value);
        void
        confirm(Quorum q, Proposal p);
        boost::optional<Accepted>
        get(Quorum q);
        Quorum
        current_quorum() const;
        boost::optional<Accepted>
        current_value() const;
        Version
        current_version() const;
        struct VersionState
        {
          VersionState(Proposal p, boost::optional<Accepted> a = {});
          Proposal proposal;
          boost::optional<Accepted> accepted;
          Version
          version() const;
          VersionState(elle::serialization::SerializerIn& s,
                       elle::Version const& v);
          void
          serialize(elle::serialization::Serializer& s, elle::Version const& v);
          typedef elle::serialization_tag serialization_tag;
        };
        ELLE_ATTRIBUTE(boost::optional<VersionState>, state);
      private:
        struct _Details;
        friend struct _Details;

      /*--------------.
      | Serialization |
      `--------------*/
      public:
        Server(elle::serialization::SerializerIn& s, elle::Version const& v);
        void
        serialize(elle::serialization::Serializer& s, elle::Version const& v);
        typedef elle::serialization_tag serialization_tag;

      /*----------.
      | Printable |
      `----------*/
      public:
        void
        print(std::ostream& output) const override;
      };
    }
  }
}

#include <elle/athena/paxos/Server.hxx>
