//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [mon sep 12 06:15:32 2011]
//

#ifndef NUCLEUS_PROTON_SEAM_HH
#define NUCLEUS_PROTON_SEAM_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>
#include <XXX/Nodule.hh>
#include <XXX/Quill.hh>
#include <XXX/Inlet.hh>
#include <XXX/Pins.hh>

#include <elle/idiom/Close.hh>
# include <map>
#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this generic class makes it possible to specialize Seam, especially
    /// for Seam<>.
    ///
    template <typename... T>
    class Seam;

    ///
    /// this class represents an internal tree node. such a node contains
    /// a data structure containing inlets which reference sub-nodules.
    ///
    /// note that since the block size of the nodules can be configured,
    /// the internal data structure is hierachical in order to handle
    /// blocks with thousand entries.
    ///
    template <typename V>
    class Seam<V>:
      public Nodule<V>
    {
    public:
      //
      // static attributes
      //
      static elle::Natural32    Footprint;

      //
      // static methods
      //
      static elle::Status       Initialize();
      static elle::Status       Clean();

      //
      // types
      //
      typedef Inlet< V, Nodule<V> >                     I;

      //
      // types
      //
      typedef std::map<const typename V::K, I*>                 Container;

      struct                                                    Iterator
      {
        typedef typename Container::iterator                    Forward;
        typedef typename Container::reverse_iterator            Backward;
      };

      //
      // constructors & destructors
      //

      // XXX[virer le constructeur vide du coup]
      Contents< Seam<V> >* contents() { return &this->_contents; }
      Contents< Seam<V> >& _contents;
      Seam(Contents< Seam<V> >& contents):
        Nodule<V>(Nodule<V>::TypeSeam),
        _contents(contents)
      {}

      // XXX Seam();
      ~Seam();

      //
      // methods
      //
      elle::Status              Create();

      elle::Status              Insert(I*);
      elle::Status              Insert(const typename V::K&,
                                       Handle&);

      elle::Status              Delete(typename Iterator::Forward&);
      elle::Status              Delete(Handle&);
      elle::Status              Delete(const typename V::K&);

      elle::Status              Exist(const typename V::K&);

      elle::Status              Lookup(const typename V::K&,
                                       typename Iterator::Forward&);
      elle::Status              Lookup(const typename V::K&,
                                       I*&);
      elle::Status              Lookup(const typename V::K&,
                                       Handle&);

      elle::Status              Locate(const typename V::K&,
                                       typename Iterator::Forward&);
      elle::Status              Locate(const typename V::K&,
                                       I*&);
      elle::Status              Locate(const typename V::K&,
                                       Handle&);

      elle::Status              Link(I*,
                                     Handle&);
      elle::Status              Link(Handle&);

      elle::Status              Update(const typename V::K&,
                                       const typename V::K&);
      elle::Status              Propagate(const typename V::K&,
                                          const typename V::K&);

      elle::Status              Merge(Seam<V>*);

      elle::Status              Check(Handle&,
                                      const Pins);

      //
      // interfaces
      //

      // nodule
      elle::Status              Mayor(typename V::K&) const;
      elle::Status              Maiden(typename V::K&) const;
      elle::Status              Search(const typename V::K&,
                                       Handle&);
      elle::Status              Check(Handle&,
                                      Handle&,
                                      const Pins = PinAll);
      elle::Status              Traverse(const elle::Natural32 = 0);
      elle::Status              Seal(const elle::SecretKey&,
                                     Address&);

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status              Serialize(elle::Archive&) const;
      elle::Status              Extract(elle::Archive&);

      //
      // attributes
      //
      Container                 container;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <XXX/Seam.hxx>

#endif
