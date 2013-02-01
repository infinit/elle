#include <nucleus/neutron/Ensemble.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Index.hh>
#include <nucleus/neutron/Size.hh>
#include <nucleus/neutron/Subject.hh>
#include <nucleus/proton/Ambit.hh>
#include <nucleus/proton/Nest.hh>
#include <nucleus/proton/Contents.hh>
#include <nucleus/proton/Limits.hh>
#include <nucleus/proton/Porcupine.hh>
#include <nucleus/Exception.hh>

#include <elle/log.hh>
#include <elle/assert.hh>
#include <elle/finally.hh>

#include <cryptography/Digest.hh>
#include <cryptography/oneway.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

ELLE_LOG_COMPONENT("infinit.nucleus.neutron.Ensemble");

namespace nucleus
{
  namespace neutron
  {
    /*----------.
    | Constants |
    `----------*/

    proton::Nature const Ensemble::Constants::seam =
      proton::Nature::ensemble_seam;
    proton::Nature const Ensemble::Constants::quill =
      proton::Nature::ensemble_quill;
    proton::Nature const Ensemble::Constants::value =
      proton::Nature::ensemble_value;
    proton::Nature const Ensemble::Constants::nature =
      Ensemble::Constants::value;

    /*---------------.
    | Static Methods |
    `---------------*/

    void
    Ensemble::transfer_right(Ensemble& left,
                             Ensemble& right,
                             proton::Extent const size)
    {
      static proton::Footprint const initial =
        elle::serialize::footprint<Ensemble>();

      ELLE_TRACE_FUNCTION(left, right, size);

      proton::Extent const extent = left.nest().limits().extent();
      proton::Footprint footprint(initial);

      ELLE_ASSERT(left.nest().limits().extent() ==
                  right.nest().limits().extent());

      auto end = left._container.end();
      auto i = left._container.begin();

      // Go through the left ensemble's fellows until the future size is
      // reached after which all the remaining fellows will be moved to
      // the right ensemble.
      for (; i != end; ++i)
        {
          auto& fellow = i->second;

          // Check whether the container's contention size has been reached.
          if (footprint >= size)
            break;

          // Otherwise, leave this item in the left fellow.
          //
          // Note however that another check is performed in order to make
          // sure that adding this fellow will not make the container too large.
          if ((footprint + fellow->footprint()) > extent)
            break;

          // Add the fellow's footprint to the local _footprint_ variable.
          footprint += fellow->footprint();
        }

      // Go through the remaining fellows in order to move them to
      // the right ensemble.
      for (auto j = i; j != end; ++j)
        {
          auto& fellow = j->second;

          // Substract the fellow's footprint from the left ensemble since
          // it is getting moved to the right one.
          left.footprint(left.footprint() - fellow->footprint());

          // Insert the fellow into the right ensemble.
          right.insert(fellow);
        }

      // Remove the moved fellows from the the current ensemble.
      left._container.erase(i, end);
    }

    void
    Ensemble::transfer_left(Ensemble& left,
                            Ensemble& right,
                            proton::Extent const size)
    {
      static proton::Footprint const initial =
        elle::serialize::footprint<Ensemble>();

      ELLE_TRACE_FUNCTION(left, right, size);

      proton::Extent const extent = left.nest().limits().extent();
      proton::Footprint footprint(initial);

      ELLE_ASSERT(left.nest().limits().extent() ==
                  right.nest().limits().extent());

      auto rend = right._container.rend();
      auto i = right._container.rbegin();

      // Go through the right ensemble's fellows until the future size is
      // reached after which all the remaining fellows will be moved to
      // the left ensemble.
      for (; i != rend; ++i)
        {
          auto& fellow = i->second;

          // Check whether the container's contention size has been reached.
          if (footprint >= size)
            break;

          // Otherwise, leave this fellow in the ensemble.
          //
          // Note however that another check is performed in order to make
          // sure that adding this fellow will not make the container too large.
          if ((footprint + fellow->footprint()) > extent)
            break;

          // Add the fellow's footprint to the footprint.
          footprint += fellow->footprint();
        }

      // Go through the remaining fellows in order to move them to
      // the left ensemble.
      for (auto j = i; j != rend; ++j)
        {
          auto& fellow = j->second;

          // Substract the fellow's footprint from the right ensemble since
          // it is getting moved to the left one.
          right.footprint(right.footprint() - fellow->footprint());

          // Insert the fellow into the left ensemble.
          left.insert(fellow);
        }

      // Remove the moved fellows from the right ensemble.
      right._container.erase(right._container.begin(), i.base());
    }

    /*-------------.
    | Construction |
    `-------------*/

    Ensemble::Ensemble():
      Value::Value()
    {
      static proton::Footprint const initial =
        elle::serialize::footprint(*this);

      this->footprint(initial);
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(Ensemble, proton::Value)
    {
    }

    /*--------.
    | Methods |
    `--------*/

    void
    Ensemble::insert(Fellow* fellow)
    {
      ELLE_TRACE_METHOD(fellow);

      ELLE_ASSERT(fellow != nullptr);

      std::shared_ptr<Fellow> pointer{fellow};

      this->insert(pointer);
    }

    void
    Ensemble::insert(std::shared_ptr<Fellow> const& fellow)
    {
      ELLE_TRACE_METHOD(fellow);

      // Inject the fellow in the container.
      this->_inject(fellow);

      // Update the state.
      this->state(proton::State::dirty);
    }

    elle::Boolean
    Ensemble::exist(Subject const& subject) const
    {
      ELLE_TRACE_METHOD(subject);

      return (this->_container.find(subject) != this->_container.end());
    }

    void
    Ensemble::update(Subject const& subject,
                     Token const& token)
    {
      ELLE_TRACE_METHOD(subject, token);

      // Locate the fellow.
      auto iterator = this->_iterator(subject);

      // Retrieve the fellow.
      auto& fellow = iterator->second;

      // Substract the fellow's current footprint.
      ELLE_ASSERT(fellow->footprint() != 0);
      ELLE_ASSERT(this->footprint() >= fellow->footprint());
      this->footprint(this->footprint() - fellow->footprint());

      // Update the fellow.
      fellow->token(token);

      // Add the fellow's new footprint.
      ELLE_ASSERT(fellow->footprint() != 0);
      this->footprint(this->footprint() + fellow->footprint());

      // Update the state.
      this->state(proton::State::dirty);
    }

    Fellow const&
    Ensemble::locate(Subject const& subject) const
    {
      ELLE_TRACE_METHOD(subject);

      auto scoutor = this->_iterator(subject);
      auto& fellow = scoutor->second;

      return (*fellow);
    }

    Index
    Ensemble::seek(Subject const& subject) const
    {
      ELLE_TRACE_METHOD(subject);

      Index index{0};

      for (auto& pair: this->_container)
        {
          auto& fellow = pair.second;

          if (fellow->subject() == subject)
            return (index);

          index++;
        }

      throw Exception("unable to locate the given subject '%s'", subject);
    }

    Fellow const&
    Ensemble::select(Index const& index) const
    {
      ELLE_TRACE_METHOD(index);

      Index i{0};

      for (auto& pair: this->_container)
        {
          auto& fellow = pair.second;

          if (i == index)
            return (*fellow);
        }

      throw Exception("unable to locate the fellow at the given index '%s'",
                      index);
    }

    Range<Fellow>
    Ensemble::consult(Index const& index,
                      Size const& size) const
    {
      ELLE_TRACE_METHOD(index);

      Range<Fellow> range;
      Index i{0};

      for (auto& pair: this->_container)
        {
          auto& fellow = pair.second;

          // If this fellow lies in the selected range [index, index + size[.
          if ((i >= index) && (i < (index + size)))
            range.insert(fellow);

          i++;
        }

      return (range);
    }

    void
    Ensemble::erase(Subject const& subject)
    {
      ELLE_TRACE_METHOD(subject);

      // Call take out and forget about the returned fellow, hence (possibly)
      // deleting it.
      this->takeout(subject);
    }

    std::shared_ptr<Fellow>
    Ensemble::takeout(Subject const& subject)
    {
      // Locate the fellow for the given subject.
      auto iterator = this->_iterator(subject);

      // Retrieve the fellow.
      std::shared_ptr<Fellow> fellow = iterator->second;

      // Substract the fellow footprint from the ensemble's.
      ELLE_ASSERT(fellow->footprint() != 0);
      ELLE_ASSERT(this->footprint() >= fellow->footprint());
      this->footprint(this->footprint() - fellow->footprint());

      // Remove the fellow from the container.
      this->_container.erase(iterator);

      // Update the state.
      this->state(proton::State::dirty);

      // And finally return the pointer.
      return (fellow);
    }

    Size
    Ensemble::size() const
    {
      ELLE_TRACE_METHOD("");

      return (static_cast<Size>(this->_container.size()));
    }

    void
    Ensemble::_inject(std::shared_ptr<Fellow> const& fellow)
    {
      ELLE_DEBUG_METHOD(fellow);

      // Check that the fellow's subject does not already exist.
      if (this->_container.find(fellow->subject()) != this->_container.end())
        throw Exception("the subject '%s' seems to already exist",
                        fellow->subject());

      // Insert the fellow to the container.
      auto result =
        this->_container.insert(
          std::pair<Subject const, std::shared_ptr<Fellow>>{
            fellow->subject(), fellow});

      // Check if the insertion was successful.
      if (result.second == false)
        throw Exception("unable to insert the fellow the container");

      // Update the footprint.
      ELLE_ASSERT(fellow->footprint() != 0);
      this->footprint(this->footprint() + fellow->footprint());
    }

    typename Ensemble::Scoutor
    Ensemble::_iterator(Subject const& subject) const
    {
      Scoutor scoutor;

      if ((scoutor = this->_container.find(subject)) == this->_container.end())
        throw Exception("unable to locate the given subject: '%s'", subject);

      return (scoutor);
    }

    typename Ensemble::Iterator
    Ensemble::_iterator(Subject const& subject)
    {
      Iterator iterator;

      if ((iterator = this->_container.find(subject)) == this->_container.end())
        throw Exception("unable to locate the given subject: '%s'", subject);

      return (iterator);
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Ensemble::Dump(const elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[Ensemble] #"
                << this->_container.size() << std::endl;

      if (Value::Dump(margin + 2) == elle::Status::Error)
        throw Exception("unable to dump the underlying value");

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Fellows]" << std::endl;

      for (auto& pair : this->_container)
        {
          auto& fellow = pair.second;

          if (fellow->Dump(margin + 4) == elle::Status::Error)
            throw Exception("unable to dump the fellow");
        }

      return (elle::Status::Ok);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Ensemble::print(std::ostream& stream) const
    {
      stream << "ensemble("
             << "#" << this->_container.size()
             << ")";
    }

    /*------.
    | Value |
    `------*/

    elle::Boolean
    Ensemble::empty() const
    {
      return (this->_container.empty());
    }

    Subject
    Ensemble::mayor() const
    {
      ELLE_ASSERT(this->_container.empty() == false);

      return (this->_container.rbegin()->first);
    }

    proton::Capacity
    Ensemble::capacity() const
    {
      return (static_cast<proton::Capacity>(this->_container.size()));
    }

    proton::Handle
    Ensemble::split()
    {
      ELLE_TRACE_METHOD("");

      // Allocate a new right ensemble.
      proton::Contents* contents =
        new proton::Contents{this->nest().network(),
                             this->nest().agent_K(),
                             new Ensemble};
      proton::Handle orphan{this->nest().attach(contents)};
      proton::Ambit<Ensemble> right{this->nest(), orphan};

      // Load the new right ensemble.
      right.load();

      // Export the records from the current ensemble into the new ensemble.
      Ensemble::transfer_right(*this,
                               right(),
                               this->nest().limits().extent() *
                               this->nest().limits().contention());

      // Set both values' state as dirty.
      this->state(proton::State::dirty);
      right().state(proton::State::dirty);

      // Unload the new right ensemble.
      right.unload();

      return (orphan);
    }

    void
    Ensemble::merge(proton::Handle& other)
    {
      ELLE_TRACE_METHOD(other);

      proton::Ambit<Ensemble> ensemble(this->nest(), other);

      // Load the other ensemble.
      ensemble.load();

      // Check which value has the lowest keys.
      if (ensemble().mayor() < this->mayor())
        {
          // In this case, export the records from the given ensemble
          // into the current's since these records happen to have
          // lower keys.
          //
          // Note that the footprint-based number of records to keep in
          // the left ensemble is zero i.e transfer all records.
          Ensemble::transfer_right(ensemble(), *this, 0);
        }
      else
        {
          // Otherwise, import the higher records from the given ensemble
          // into the current's.
          //
          // Note that the footprint-based number of records to keep in
          // the right ensemble is zero i.e transfer all records.
          Ensemble::transfer_left(*this, ensemble(), 0);
        }

      // Set both values' state as dirty.
      this->state(proton::State::dirty);
      ensemble().state(proton::State::dirty);

      ELLE_ASSERT(ensemble()._container.size() == 0);

      // Unload the given ensemble.
      ensemble.unload();
    }

    /*---------.
    | Iterable |
    `---------*/

    typename Ensemble::Scoutor
    Ensemble::begin() const
    {
      return (this->_container.begin());
    }

    typename Ensemble::Scoutor
    Ensemble::end() const
    {
      return (this->_container.end());
    }

    typename Ensemble::Iterator
    Ensemble::begin()
    {
      return (this->_container.begin());
    }

    typename Ensemble::Iterator
    Ensemble::end()
    {
      return (this->_container.end());
    }

    namespace ensemble
    {
      /*----------.
      | Functions |
      `----------*/

      void
      upgrade(proton::Porcupine<Ensemble>& porcupine,
              cryptography::PrivateKey const& pass_k)
      {
        ELLE_TRACE_FUNCTION(porcupine, pass_k);

        for (proton::Capacity index = 0; index < porcupine.size(); )
          {
            auto pair = porcupine.seek(index);
            auto& door = pair.first;

            ELLE_ASSERT(index == pair.second);

            door.open();

            for (auto _pair: door())
              {
                auto& fellow = _pair.second;

                switch (fellow->subject().type())
                  {
                  case Subject::TypeUser:
                    {
                      ELLE_TRACE("update fellow user '%s'", fellow->subject());

                      // Update the fellow's token with the freshly constructed
                      // token which embeds the new private key encrypted with
                      // the user's public key so that only he can decrypt it.
                      door().update(fellow->subject(),
                                    Token{pass_k, fellow->subject().user()});

                      break;
                    }
                  case Subject::TypeGroup:
                    {
                      // XXX
                      ELLE_ASSERT(false);

                      break;
                    }
                  default:
                    throw Exception("unknown subject type '%s'",
                                    fellow->subject().type());
                  }
              }

            index += door().size();

            door.close();
          }
      }
    }
  }
}
