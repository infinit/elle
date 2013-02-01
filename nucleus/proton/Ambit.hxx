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
      _handle(handle)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Ambit");

      switch (this->_mode)
        {
        case Mode::transparent:
          {
            ELLE_DEBUG("loading on construction");

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
                ELLE_DEBUG("unloading on destruction");

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
      ELLE_DEBUG_METHOD("");

      ELLE_ASSERT(this->_state == State::unloaded);

      // Load the handle from the nest.
      //
      // This operation ensures that the handle's block gets loaded in main
      // memory and referenced by the handle's egg.
      this->_nest.load(this->_handle);

      // Make sure the retrieved block is compatible with the ambit's type.
      if (dynamic_cast<T*>(&this->_handle.egg()->block()->node()) == nullptr)
        throw Exception("the referenced node block's type does not match the "
                        "given type");

      // Lock the egg so as to make sure nobody unloads the block on the
      // storage layer since it is being used right here.
      // XXX[automatic locking when loading is better] this->_handle.egg()->lock();

      this->_state = State::loaded;
    }

    template <typename T>
    void
    Ambit<T>::unload()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Ambit");
      ELLE_DEBUG_METHOD("");

      ELLE_ASSERT(this->_state == State::loaded);

      // Unlock the egg now that we no longer need it here.
      // XXX[unlock when unloading] this->_handle.egg()->unlock();

      // Unload the handle for the nest to be able to manage its eggs.
      this->_nest.unload(this->_handle);

      this->_state = State::unloaded;
    }

    template <typename T>
    Contents const&
    Ambit<T>::contents() const
    {
      return (*this->_handle.egg()->block());
    }

    template <typename T>
    Contents&
    Ambit<T>::contents()
    {
      return (*this->_handle.egg()->block());
    }

    /*----------.
    | Operators |
    `----------*/

    template <typename T>
    T const&
    Ambit<T>::operator ()() const
    {
      return (static_cast<T const&>(this->contents().node()));
    }

    template <typename T>
    T&
    Ambit<T>::operator ()()
    {
      return (static_cast<T&>(this->contents().node()));
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
        throw Exception("unable to dump the handle");

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
