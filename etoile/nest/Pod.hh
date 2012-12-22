#ifndef ETOILE_NEST_POD_HH
# define ETOILE_NEST_POD_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Contents.hh>
# include <nucleus/proton/Placement.hh>
# include <nucleus/proton/Address.hh>

# include <elle/idiom/Open.hh>

namespace etoile
{
  namespace nest
  {

    ///
    /// XXX
    ///
    class Pod:
      public elle::radix::Object
    {
    public:
      //
      // enumerations
      //
      enum Nature
        {
          NatureVolatile,
          NaturePersistent
        };

      enum Link
        {
          LinkAttached,
          LinkDetached
        };

      enum State
        {
          StateUnloaded,
          StateLoaded
        };

      //
      // constructors & destructors
      //
      Pod();
      // XXX[ownership transferred]
      Pod(const nucleus::proton::Placement&,
          nucleus::proton::Contents*);
      Pod(const nucleus::proton::Placement&,
          const nucleus::proton::Address&,
          nucleus::proton::Contents*);
      Pod(const Pod&);

      //
      // methods
      //
      /// XXX
      std::shared_ptr<nucleus::proton::Contents>
      load(nucleus::proton::Handle&);
      /// XXX
      void
      unload(nucleus::proton::Handle&);
      /// XXX
      void
      release();

      //
      // interfaces
      //

      // object
      declare(Pod);

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Nature nature;
      Link link;
      State state;

      nucleus::proton::Placement placement;
      nucleus::proton::Address address;

      std::shared_ptr<nucleus::proton::Contents> block;
      elle::Natural32 counter;
    };

  }
}

#endif
