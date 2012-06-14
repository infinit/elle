#include <boost/unordered_map.hpp>

#include <elle/concurrency/Event.hh>

#include <elle/network/Parcel.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>
#include <elle/standalone/Log.hh>

#include <elle/cryptography/Random.hh>

namespace elle
{
  using namespace cryptography;

  namespace concurrency
  {
    /// this variable defines an unused hence null Event.
    const Event                         Event::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Event::Event()
      : waited(false)
      , parcel(0)
      , _identifier(0)
      , _signal(0)
    {}

//
// ---------- methods ---------------------------------------------------------
//


    // XXX workaround until concurrency is entirely rewritten.
    typedef std::pair<Event::SignalType*, network::Parcel*> Signal;
    typedef boost::unordered_map <Natural64, Signal> Signals;
    static Signals _signals;

    ///
    /// this method generates a new unique event.
    ///
    Status              Event::Generate()
    {
      // try until the generated event is different from Null.
      do
        {
          // generate the identifier.
          if (Random::Generate(this->_identifier) == Status::Error)
            escape("unable to generate the identifier");
        } while (*this == Event::Null);

      return Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two events match.
    ///
    Boolean             Event::operator==(const Event&          element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      if (this->_identifier != element._identifier)
        return false;

      return true;
    }

    ///
    /// this operator compares two events.
    ///
    Boolean             Event::operator<(const Event&           element) const
    {
      return (this->_identifier < element._identifier);
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Event, _());

//
// ---------- archivable ------------------------------------------------------
//
    void
    Event::XXX_OLD_Extract()
    {
      // XXX
      Signals::iterator it = _signals.find(this->_identifier);
      if (it != _signals.end())
        {
          this->_signal = it->second.first;
          this->parcel = it->second.second;
          this->waited = true;
        }
    };

    void
    Event::Cleanup()
    {
      Signals::iterator it = _signals.find(this->_identifier);
      if (it != _signals.end())
        {
          delete it->second.first;
          _signals.erase(it);
        }
    }

    Natural64
    Event::Identifier() const
    {
      return _identifier;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps an event.
    ///
    Status              Event::Dump(const Natural32             margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Event] " << this->_identifier
                << " XXX(" << this->waited << ")" << std::endl;

      return Status::Ok;
    }

    Event::SignalType&
    Event::Signal()
    {
      if (!_signal)
        {
          this->_signal = new SignalType();
          _signals[this->_identifier] = concurrency::Signal(this->_signal, 0);
          this->waited = true;
        }
      return *_signal;
    }

    elle::network::Parcel*
    Event::Parcel()
    {
      Signals::iterator it(_signals.find(this->_identifier));
      assert(it != _signals.end());
      assert(it->second.second);
      return it->second.second;
    }

    void
    Event::Parcel(elle::network::Parcel* parcel)
    {
      Signals::iterator it(_signals.find(this->_identifier));
      assert(it != _signals.end());
      it->second.second = parcel;
    }
  }
}
