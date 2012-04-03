//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [mon sep 12 05:23:24 2011]
//

#ifndef NUCLEUS_PROTON_PORCUPINE_HH
#define NUCLEUS_PROTON_PORCUPINE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Block.hh>
#include <XXX/Handle.hh>
#include <XXX/Pins.hh>
#include <nucleus/proton/Transcript.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// this class needs forward declaration to prevent conflicts.
    ///
    template <typename... T>
    class Seam;

    ///
    /// this class needs forward declaration to prevent conflicts.
    ///
    template <typename... T>
    class Quill;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this generic declaration enables the definition of an empty
    /// Porcupine<> class which contains some generic stuff.
    ///
    template <typename... T>
    class Porcupine;

    ///
    /// this class makes it easy to build a tree-based data structure of
    /// blocks which does not require all the blocks to remain in main
    /// memory.
    ///
    /// every data item is referenced in an inlet. inlets a grouped together
    /// in a quill which represents a leaf of the given tree. then, every
    /// nodule being a seam or quill is referenced by a seam which encapsulates
    /// several of these references.
    ///
    /// therefore, the tree looks a bit like this with _k_ a key, _@_
    /// the address of a block and _d_ the actual data item:
    ///
    ///                             root seam nodule
    ///                          [ k@ | k@ | ... | k@ ]
    ///                             '               `
    ///                            '                 `
    ///              internal seam nodule         internal seam nodule
    ///             [ k@ | k@ | ... | k@ ]       [ k@ | k@ | ... | k@ ]
    ///                     '
    ///                      '
    ///                leaf quill nodule
    ///              [ k@ | k@ | ... | k@ ]
    ///                                 '
    ///                                  '
    ///                               data item
    ///                                 [ d ]
    ///
    /// note that every nodule is stored in a block, very much like data
    /// items. since the size of such blocks is limited by the network
    /// descriptor's extent attribute, nodules are split when full.
    /// likewise, should a nodule become too small, a balancing or merging
    /// procedure is triggered in order to `simplify' the tree.
    ///
    /// since such blocks can be quite large, the seam and quill entries,
    /// known as inlets, are organised in a tree-based data structure such
    /// as a map.
    ///
    /// note that a mayor key refers to the key with the highest value. this
    /// is the key which is propagated through the tree in order to route
    /// the request from the top. likewise, a key is said to be the maiden
    /// key if it is the only one remaining in a nodule.
    ///
    /// XXX attention V doit etre un type qui peut se mettre dans Contents.
    /// XXX et les handles Add()ed doivent representer des Contents aussi.
    ///
    template <typename V>
    class Porcupine<V>:
      public elle::Object
    {
    public:
      //
      // constructors & destructors
      //
      Porcupine();

      //
      // methods
      //
      elle::Status              Add(const typename V::K&,
                                    Handle&);
      elle::Status              Exist(const typename V::K&);
      elle::Status              Locate(const typename V::K&,
                                       Handle&);
      elle::Status              Remove(const typename V::K&);

      template <typename N>
      elle::Status              Insert(Handle&,
                                       const typename V::K&,
                                       Handle&);
      template <typename N>
      elle::Status              Delete(Handle&,
                                       const typename V::K&);

      elle::Status              Grow();
      elle::Status              Shrink();

      elle::Status              Search(const typename V::K&,
                                       Handle&);

      elle::Status              Check(const Pins = PinAll);
      elle::Status              Traverse(const elle::Natural32 = 0);

      elle::Status              Seal(const elle::SecretKey&);

      //
      // interfaces
      //

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      elle::Natural32           height;

      Handle                    root;

      Transcript                transcript;
    };

    ///
    /// this class contains generic information and methods for controlling
    /// the porcupine data structure system.
    ///
    template <>
    class Porcupine<>
    {
    public:
      //
      // constants
      //
      struct                                    Default
      {
        static const elle::Natural32            Length;

        static elle::SecretKey                  Secret;
      };

      //
      // static methods
      //
      static elle::Status       Initialize(
        const elle::Callback<
          elle::Status,
          elle::Parameters<
            Block*,
            Handle&
            >
          >&,
        const elle::Callback<
          elle::Status,
          elle::Parameters<
            Handle&
            >
          >&,
        const elle::Callback<
          elle::Status,
          elle::Parameters<
            Handle&
            >
          >&,
        const elle::Callback<
          elle::Status,
          elle::Parameters<
            Handle&
            >
          >&);
      static elle::Status       Clean();

      //
      // static attributes
      //
      static
      elle::Callback<
        elle::Status,
        elle::Parameters<
          Block*,
          Handle&
          >
        >                       Attach;
      static
      elle::Callback<
        elle::Status,
        elle::Parameters<
          Handle&
          >
        >                       Detach;
      static
      elle::Callback<
        elle::Status,
        elle::Parameters<
          Handle&
          >
        >                       Load;
      static
      elle::Callback<
        elle::Status,
        elle::Parameters<
          Handle&
          >
        >                       Unload;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <XXX/Porcupine.hxx>

//
// ---------- includes --------------------------------------------------------
//

#include <XXX/Seam.hh>
#include <XXX/Quill.hh>

#endif
