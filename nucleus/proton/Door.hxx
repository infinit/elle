#ifndef NUCLEUS_PROTON_DOOR_HXX
# define NUCLEUS_PROTON_DOOR_HXX

# include <nucleus/Exception.hh>

# include <elle/assert.hh>

namespace nucleus
{
  namespace proton
  {
//
// ---------- Door ------------------------------------------------------------
//

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
    Door<T>::Door(Handle const& handle,
                  Nest& nest):
      _location(Location::nest),
      _block(new Block{handle, nest})
    {
    }

    template <typename T>
    Door<T>::Door(Door&& other):
      _location(other._location)
    {
      switch (this->_location)
        {
        case Location::memory:
          {
            ELLE_ASSERT(other._value != nullptr);

            this->_value = other._value;
            other._value = nullptr;

            break;
          }
        case Location::nest:
          {
            ELLE_ASSERT(other._block != nullptr);

            this->_block = other._block;
            other._block = nullptr;

            break;
          }
        default:
          throw Exception("unknown location '%s'", this->_location);
        }
    }

    template <typename T>
    Door<T>::~Door()
    {
      switch (this->_location)
        {
        case Location::memory:
          {
            break;
          }
        case Location::nest:
          {
            delete this->_block;

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
            ELLE_ASSERT(this->_block != nullptr);

            this->_block->open();

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
            ELLE_ASSERT(this->_block != nullptr);

            this->_block->close();

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
    elle::Boolean
    Door<T>::operator ==(Door<T> const& other) const
    {
      if (this == &other)
        return (true);

      if (this->_location != other._location)
        return (false);

      switch (this->_location)
        {
        case Location::memory:
          {
            ELLE_ASSERT(this->_value != nullptr);
            ELLE_ASSERT(other._value != nullptr);

            // Compare the value pointers.
            return (this->_value == other._value);
          }
        case Location::nest:
          {
            ELLE_ASSERT(this->_block != nullptr);
            ELLE_ASSERT(other._block != nullptr);

            // Compare the handles.
            return (this->_block->handle() == other._block->handle());
          }
        default:
          throw Exception("unknown location '%s'", this->_location);
        }

      elle::unreachable();
    }

    template <typename T>
    T const&
    Door<T>::operator ()() const
    {
      switch (this->_location)
        {
        case Location::memory:
          {
            ELLE_ASSERT(this->_value != nullptr);

            return (*this->_value);
          }
        case Location::nest:
          {
            ELLE_ASSERT(this->_block != nullptr);

            return (this->_block->access());
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
            ELLE_ASSERT(this->_value != nullptr);

            return (*this->_value);
          }
        case Location::nest:
          {
            ELLE_ASSERT(this->_block != nullptr);

            return (this->_block->access());
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

      switch (this->_location)
        {
        case Location::memory:
          {
            std::cout << alignment << elle::io::Dumpable::Shift
                      << "[Value]" << std::endl;

            this->_value->Dump(margin + 4);

            break;
          }
        case Location::nest:
          {
            ELLE_ASSERT(this->_block != nullptr);

            std::cout << alignment << elle::io::Dumpable::Shift
                      << "[Block]" << std::endl;

            this->_block->handle().Dump(margin + 4);

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
            ELLE_ASSERT(this->_value != nullptr);

            stream << *this->_value;

            break;
          }
        case Location::nest:
          {
            ELLE_ASSERT(this->_block != nullptr);

            stream << *this->_block;

            break;
          }
        default:
          throw Exception("unknown location '%s'", this->_location);
        }

      stream << ")";
    }

//
// ---------- Block -----------------------------------------------------------
//

    /*-------------.
    | Construction |
    `-------------*/

    template <typename T>
    Door<T>::Block::Block(Handle const& handle,
                          Nest& nest):
      _handle(handle),
      _ambit(nest, this->_handle, Ambit<T>::Mode::automatic)
    {
    }

    template <typename T>
    Door<T>::Block::~Block()
    {
      if (this->_ambit.state() == Ambit<T>::State::loaded)
        this->_ambit.unload();
    }

    /*--------.
    | Methods |
    `--------*/

    template <typename T>
    void
    Door<T>::Block::open()
    {
      this->_ambit.load();
    }

    template <typename T>
    void
    Door<T>::Block::close()
    {
      this->_ambit.unload();
    }

    template <typename T>
    T const&
    Door<T>::Block::access() const
    {
      if (this->_ambit.state() != Ambit<T>::State::loaded)
        this->open();

      return (this->_ambit());
    }

    template <typename T>
    T&
    Door<T>::Block::access()
    {
      if (this->_ambit.state() != Ambit<T>::State::loaded)
        this->open();

      return (this->_ambit());
    }

    /*----------.
    | Printable |
    `----------*/

    template <typename T>
    void
    Door<T>::Block::print(std::ostream& stream) const
    {
      stream << this->_ambit;
    }
  }
}

#endif
