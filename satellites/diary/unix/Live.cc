//
// ---------- header ----------------------------------------------------------
//
// project       diary
//
// license       infinit
//
// author        julien quintard   [fri jul  1 11:42:31 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <satellites/diary/unix/Live.hh>

namespace satellite
{
  namespace unix
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the list of mappings.
    ///
    Live::Container                     Live::Items;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor making the mapping between a recorded _passive_
    /// _fh_ and the _active_ one.
    ///
    Live::Item::Item(const elle::Natural64                      passive,
                     const elle::Natural64                      active):
      passive(passive),
      active(active)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the live system.
    ///
    elle::Status        Live::Initialize()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// this method cleans the live system.
    ///
    elle::Status        Live::Clean()
    {
      Live::Iterator    iterator;

      enter();

      // go through the items.
      for (iterator = Live::Items.begin();
           iterator != Live::Items.end();
           iterator++)
        {
          Live::Item*   item = *iterator;

          // delete the item.
          delete item;
        }

      // clear the container.
      Live::Items.clear();

      leave();
    }

    ///
    /// this method records an item.
    ///
    elle::Status        Live::Add(const elle::Natural64         passive,
                                  const elle::Natural64         active)
    {
      Live::Item*       item;

      enterx(instance(item));

      // allocate the item.
      item = new Live::Item(passive, active);

      // push the item.
      Live::Items.push_front(item);

      // waive.
      waive(item);

      leave();
    }

    ///
    /// this method returns the active _fh_ given the passive one.
    ///
    /// in other words, this method performs the transcoding.
    ///
    elle::Status        Live::Retrieve(const elle::Natural64    passive,
                                       elle::Natural64&         active)
    {
      Live::Scoutor     scoutor;

      enter();

      // go through the items.
      for (scoutor = Live::Items.begin();
           scoutor != Live::Items.end();
           scoutor++)
        {
          Live::Item*   item = *scoutor;

          // check if this is the item we are looking for.
          if (item->passive == passive)
            {
              active = item->active;

              leave();
            }
        }

      escape("unable to locate this live item");
    }

    ///
    /// this method removes an item.
    ///
    elle::Status        Live::Remove(const elle::Natural64      passive)
    {
      Live::Iterator    iterator;

      enter();

      // go through the items.
      for (iterator = Live::Items.begin();
           iterator != Live::Items.end();
           iterator++)
        {
          Live::Item*   item = *iterator;

          // check if this is the item we are looking for.
          if (item->passive == passive)
            {
              // delete the item.
              delete item;

              // remove it from the container.
              Live::Items.erase(iterator);

              leave();
            }
        }

      escape("unable to locate this live item");
    }

    ///
    /// this method shows all the recorded items.
    ///
    elle::Status        Live::Show(const elle::Natural32        margin)
    {
      elle::String      alignment(margin, ' ');
      Live::Scoutor     scoutor;

      enter();

      std::cout << alignment << "[Live]" << std::endl;

      std::cout << alignment << elle::Dumpable::Shift
                << "[Items]" << std::endl;

      // go through the items.
      for (scoutor = Live::Items.begin();
           scoutor != Live::Items.end();
           scoutor++)
        {
          Live::Item*   item = *scoutor;

          std::cout << alignment << elle::Dumpable::Shift
                    << elle::Dumpable::Shift
                    << "[Item] "
                    << std::hex << item->passive
                    << " :: "
                    << std::hex << item->active << std::endl;
        }

      leave();
    }

  }
}
