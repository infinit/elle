#ifndef NUCLEUS_PROTON_DOOR_HXX
# define NUCLEUS_PROTON_DOOR_HXX

# include <nucleus/Exception.hh>

# include <elle/assert.hh>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Construction |
    `-------------*/

    template <typename T>
    Door<T>::Door(T* value):
      _location(Location::memory),
      _value(value)
    {
    }

    template <typename T>
    Door<T>::Door(Handle& handle,
                  Nest& nest)
      _location(Location::nest),
      _ambit(new Ambit<T>(nest, handle, Ambit<T>::Mode::automatic))
    {
    }

    template <typename T>
    Door<T>::~Door()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Door");

      switch (this->_location)
        {
        case Location::memory:
          {
            break;
          }
        case Location::nest:
          {
            if (this->_ambit->state() == Ambit<T>::State::loaded)
              this->_ambit->unload();

            delete this->_ambit;

            break;
          }
        default:
          throw Exception("unknown location '%s'", this->_location);
        }
    }

    /*--------.
    | Methods |
    `--------*/

    template <typename T>
    void
    Door<T>::open()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Door");
      ELLE_TRACE_METHOD("");

      switch (this->_location)
        {
        case Location::memory:
          {
            break;
          }
        case Location::nest:
          {
            this->_ambit->load();

            break;
          }
        default:
          throw Exception("unknown location '%s'", this->_location);
        }
    }

    template <typename T>
    void
    Door<T>::close()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Door");
      ELLE_TRACE_METHOD("");

      switch (this->_location)
        {
        case Location::memory:
          {
            break;
          }
        case Location::nest:
          {
            this->_ambit->unload();

            break;
          }
        default:
          throw Exception("unknown location '%s'", this->_location);
        }
    }

    /*----------.
    | Operators |
    `----------*/

    template <typename T>
    T const&
    Door<T>::operator ()() const
    {
      switch (this->_location)
        {
        case Location::memory:
          {
            return (*this->_value);
          }
        case Location::nest:
          {
            if (this->_ambit->state != Ambit<T>::State::loaded)
              this->open();

            return (this->_ambit());
          }
        default:
          throw Exception("unknown location '%s'", this->_location);
        }

      elle::unreachable();
    }

    template <typename T>
    T&
    Door<T>::operator ()()
    {
      switch (this->_location)
        {
        case Location::memory:
          {
            return (*this->_value);
          }
        case Location::nest:
          {
            if (this->_ambit->state != Ambit<T>::State::loaded)
              this->open();

            return (this->_ambit());
          }
        default:
          throw Exception("unknown location '%s'", this->_location);
        }

      elle::unreachable();
    }

    /*---------.
    | Dumpable |
    `---------*/

    template <typename T>
    elle::Status
    Door<T>::Dump(const elle::Natural32 margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Door]" << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Location] " << this->_location << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Value]" << std::endl;

      switch (this->_location)
        {
        case Location::memory:
          {
            this->_value->Dump(margin + 4);

            break;
          }
        case Location::nest:
          {
            this->_ambit->Dump(margin + 4);

            break;
          }
        default:
          throw Exception("unknown location '%s'", this->_location);
        }

      return elle::Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    template <typename T>
    void
    Door<T>::print(std::ostream& stream) const
    {
      stream << "door("
             << this->_location
             << ", ";

      switch (this->_location)
        {
        case Location::memory:
          {
            stream << *this->_value;

            break;
          }
        case Location::nest:
          {
            stream << *this->_ambit;

            break;
          }
        default:
          throw Exception("unknown location '%s'", this->_location);
        }

      stream << ")";
    }
  }
}

#endif
