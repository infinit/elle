#include <nucleus/proton/Porcupine.hh>
#include <nucleus/Exception.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constants -------------------------------------------------------
//

    const elle::Natural32 Porcupine::secret_length = 256;

//
/// ---------- static methods --------------------------------------------------
//

    cryptography::SecretKey
    Porcupine::secret()
    {
      cryptography::SecretKey secret;

      if (secret.Generate(Porcupine::secret_length) == elle::Status::Error)
        throw Exception("unable to generate the default key");

      return (secret);
    }

//
// ---------- definitions -----------------------------------------------------
//

    const typename Porcupine::Flags Porcupine::FlagNone = 0;
    const typename Porcupine::Flags Porcupine::FlagRecursive = (1 << 1);
    const typename Porcupine::Flags Porcupine::FlagKey = (1 << 2);
    const typename Porcupine::Flags Porcupine::FlagAddress = (1 << 3);
    const typename Porcupine::Flags Porcupine::FlagCapacity = (1 << 4);
    const typename Porcupine::Flags Porcupine::FlagFootprint = (1 << 5);
    const typename Porcupine::Flags Porcupine::FlagState = (1 << 6);
    const typename Porcupine::Flags Porcupine::FlagAll =
      Porcupine::FlagRecursive |
      Porcupine::FlagKey |
      Porcupine::FlagAddress |
      Porcupine::FlagCapacity |
      Porcupine::FlagFootprint |
      Porcupine::FlagState;

//
// ---------- constructors & destructors --------------------------------------
//

    Porcupine::Porcupine():
      _mode(Mode::empty),
      _height(0),
      _capacity(0),
      _nest(nullptr),
      _state(StateClean)
    {
    }

    Porcupine::Porcupine(/* XXX Network const& network,
                            cryptography::PublicKey const& agent_K,*/
                         Nest& nest):
      _mode(Mode::empty),
      _height(0),
      _capacity(0),
      /* XXX
      _network(network),
      _agent_K(agent_K),
      */
      _nest(&nest),
      _state(StateClean)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    elle::Boolean
    Porcupine::empty() const
    {
      return (this->_mode == Mode::empty);
    }

    void
    Porcupine::unseal(cryptography::SecretKey const& secret)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      ELLE_TRACE_SCOPE("unseal(%s)", secret);

      this->_root.secret(secret);
    }

    Height
    Porcupine::height() const
    {
      return (this->_height);
    }

    Capacity
    Porcupine::capacity() const
    {
      return (this->_capacity);
    }

    State
    Porcupine::state() const
    {
      return (this->_state);
    }

    Nest&
    Porcupine::nest()
    {
      assert(this->_nest != nullptr);

      return (*this->_nest);
    }

    void
    Porcupine::nest(Nest& nest)
    {
      this->_nest = &nest;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the porcupine.
    ///
    elle::Status
    Porcupine::Dump(const elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[Porcupine]" << std::endl;

      // dump the mode.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Mode] " << this->_mode << std::endl;

      // dump the height.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Height] " << this->_height << std::endl;

      // dump the capacity.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Capacity] " << this->_capacity << std::endl;

      // dump the root.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Root]" << std::endl;

      switch (this->_mode)
        {
        case Mode::empty:
          {
            break;
          }
        case Mode::value:
        case Mode::hierarchy:
          {
            if (this->_root.Dump(margin + 4) == elle::Status::Error)
              escape("unable to dump the handle");

            break;
          }
        default:
          {
            throw Exception("unknown mode '%s'", this->_mode);
          }
        }

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[State] " << std::dec << this->_state << std::endl;

      return elle::Status::Ok;
    }

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Porcupine::Mode const mode)
    {
      switch (mode)
        {
        case Porcupine::Mode::empty:
          {
            stream << "empty";
            break;
          }
        case Porcupine::Mode::value:
          {
            stream << "value";
            break;
          }
        case Porcupine::Mode::hierarchy:
          {
            stream << "hierarchy";
            break;
          }
        default:
          {
            throw Exception("unknown mode: '%s'", static_cast<int>(mode));
          }
        }

      return (stream);
    }
  }
}
