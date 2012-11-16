#ifndef NUCLEUS_PROTON_AMBIT_HXX
# define NUCLEUS_PROTON_AMBIT_HXX

# include <nucleus/Exception.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

    /// XXX: manual(manual load/unload), automatic(unload automatic),
    ///      transparent(both load/unload are automatic)
    template <typename T>
    Ambit<T>::Ambit(Nest& nest,
                    Handle& handle,
                    Mode const mode):
      _nest(nest),
      _mode(mode),
      _state(StateUnloaded),
      _handle(handle),
      _block(nullptr)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Ambit");

      switch (this->_mode)
        {
        case Ambit<T>::ModeTransparent:
          {
            ELLE_TRACE("Loading on construction")
              this->load();

            break;
          }
        default:
          {
            break;
          }
        }
    }

    ///
    /// XXX
    ///
    template <typename T>
    Ambit<T>::~Ambit()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Ambit");

      switch (this->_mode)
        {
        case Ambit<T>::ModeAutomatic:
        case Ambit<T>::ModeTransparent:
          {
            if (this->_state == Ambit<T>::StateLoaded)
              {
                ELLE_TRACE("Unloading on destruction")
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

//
// ---------- methods ---------------------------------------------------------
//

    template <typename T>
    void
    Ambit<T>::load()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Ambit");

      ELLE_TRACE_SCOPE("load() this(%s)", this);

      if (this->_state == Ambit<T>::StateLoaded)
        throw Exception("unable to load an already loaded block");

      assert(this->_block == nullptr);

      this->_block = this->_nest.load(this->_handle);

      if (dynamic_cast<T*>(this->_block->node()) == nullptr)
        throw Exception("the retrieved block does not match the type");

      this->_state = Ambit<T>::StateLoaded;
    }

    template <typename T>
    void
    Ambit<T>::unload()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Ambit");

      ELLE_TRACE_SCOPE("unload() this(%s)", this);

      if (this->_state == Ambit<T>::StateUnloaded)
        throw Exception("unable to unload an already unloaded block");

      assert(this->_block != nullptr);

      this->_nest.unload(this->_handle);

      this->_block.reset();
      this->_state = Ambit<T>::StateUnloaded;
    }

    template <typename T>
    Handle&
    Ambit<T>::handle()
    {
      return (this->_handle);
    }

    template <typename T>
    Contents*
    Ambit<T>::contents()
    {
      return (this->_block.get());
    }

    template <typename T>
    T*
    Ambit<T>::operator ()()
    {
      return (static_cast<T*>(this->_block->node()));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    template <typename T>
    elle::Status
    Ambit<T>::Dump(const elle::Natural32 margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Ambit]" << std::endl;

      // dump the mode.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Mode] " << std::dec << this->_mode << std::endl;

      // dump the state.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[State] " << std::dec << this->_state << std::endl;

      // dump the handle.
      if (this->_handle.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the handle");

      // dump the block.
      if (this->_block != nullptr)
        {
          // dump the hierarchy, if present.
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

  }
}

#endif
