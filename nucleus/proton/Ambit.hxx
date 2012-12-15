#ifndef NUCLEUS_PROTON_AMBIT_HXX
# define NUCLEUS_PROTON_AMBIT_HXX

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
    Ambit<T>::Ambit(Nest& nest,
                    Handle& handle,
                    Ambit<T>::Mode const mode):
      _nest(nest),
      _mode(mode),
      _state(State::unloaded),
      _handle(handle),
      _block(nullptr)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Ambit");

      switch (this->_mode)
        {
        case Mode::transparent:
          {
            // XXX
            // ELLE_ASSERT(false && "should not be called: in an ideal world");

            ELLE_TRACE("loading on construction");

            this->load();

            break;
          }
        default:
          {
            break;
          }
        }
    }

    template <typename T>
    Ambit<T>::~Ambit()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Ambit");

      switch (this->_mode)
        {
        case Mode::automatic:
        case Mode::transparent:
          {
            if (this->_state == State::loaded)
              {
                // XXX
                // ELLE_ASSERT(false && "should not be called: in an ideal world");

                ELLE_TRACE("unloading on destruction");

                this->unload();
              }

            break;
          }
        default:
          {
            break;
          }
        }
    }

    /*--------.
    | Methods |
    `--------*/

    template <typename T>
    void
    Ambit<T>::load()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Ambit");
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT(this->_state == State::unloaded);
      ELLE_ASSERT(this->_block == nullptr);

      // Load the block from the nest.
      this->_block = this->_nest.load(this->_handle);

      // Make sure the retrieved block is compatible with the type given
      // at the ambit's construction.
      if (dynamic_cast<T*>(&this->_block->node()) == nullptr)
        throw Exception("the referenced node block's type does not match the "
                        "given type");

      this->_state = State::loaded;
    }

    template <typename T>
    void
    Ambit<T>::unload()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Ambit");
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT(this->_state == State::loaded);
      ELLE_ASSERT(this->_block != nullptr);

      // Unload the block to the nest.
      this->_nest.unload(this->_handle);

      // Release the pointer.
      this->_block.reset();

      this->_state = State::unloaded;
    }

    template <typename T>
    Contents const&
    Ambit<T>::contents() const
    {
      return (*this->_block.get());
    }

    template <typename T>
    Contents&
    Ambit<T>::contents()
    {
      return (*this->_block.get());
    }

    /*----------.
    | Operators |
    `----------*/

    template <typename T>
    T const&
    Ambit<T>::operator ()() const
    {
      return (static_cast<T const&>(this->_block->node()));
    }

    template <typename T>
    T&
    Ambit<T>::operator ()()
    {
      return (static_cast<T&>(this->_block->node()));
    }

    /*---------.
    | Dumpable |
    `---------*/

    template <typename T>
    elle::Status
    Ambit<T>::Dump(const elle::Natural32 margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Ambit]" << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Mode] " << this->_mode << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[State] " << this->_state << std::endl;

      if (this->_handle.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the handle");

      if (this->_block != nullptr)
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Block]" << std::endl;

          if (this->_block->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the nodule");
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Block] " << elle::none << std::endl;
        }

      return elle::Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    template <typename T>
    void
    Ambit<T>::print(std::ostream& stream) const
    {
      stream << "ambit("
             << this->_handle
             << ", "
             << this->_mode
             << ", "
             << this->_state
             << ")";
    }
  }
}

#endif
