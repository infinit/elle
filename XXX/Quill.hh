//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [mon sep 12 05:53:08 2011]
//

#ifndef NUCLEUS_PROTON_QUILL_HH
#define NUCLEUS_PROTON_QUILL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Block.hh>
#include <XXX/Nodule.hh>
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
    /// this class is declared in order to allow specialization, especially
    /// Quill<>.
    ///
    template <typename... T>
    class Quill;

    ///
    /// this class represents a tree's leaf node. such an element basically
    /// contains a set of inlets referencing the child nodules.
    ///
    /// note that since the block size of the nodules can be configured,
    /// the internal data structure is hierachical in order to handle
    /// blocks with thousand entries.
    ///
    template <typename V>
    class Quill<V>:
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
      typedef Inlet<V, V>                               I;

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
      Contents< Quill<V> >* contents() { return &this->_contents; }
      Contents< Quill<V> >& _contents;
      Quill(Contents< Quill<V> >& contents):
        Nodule<V>(Nodule<V>::TypeQuill),
        _contents(contents)
      {}

      // XXX Quill();
      ~Quill();

      //
      // methods
      //
      elle::Status              Create();

      elle::Status              Insert(I*);
      elle::Status              Insert(const typename V::K&,
                                       Handle&);

      elle::Status              Delete(typename Iterator::Forward&);
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

      elle::Status              Merge(Quill<V>*);

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

#include <XXX/Quill.hxx>

#endif
