#ifndef ELLE_STANDALONE_MORGUE_HXX
#define ELLE_STANDALONE_MORGUE_HXX

namespace elle
{
  namespace standalone
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method registers an address to delete.
    ///
    /// note that should have the timer not yet been allocated and set up,
    /// this is done in this method.
    ///
    template <typename T>
    Status
    Morgue::Register(T*         instance)
    {
      this->container.push_back(static_cast<Meta*>(instance));
      _corpses_available.signal();
      return Status::Ok;
    }

  }
}

#endif
