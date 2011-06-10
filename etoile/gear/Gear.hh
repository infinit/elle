//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Gear.hh
//
// created       julien quintard   [fri jun  3 10:58:40 2011]
// updated       julien quintard   [fri jun  3 11:25:51 2011]
//

#ifndef ETOILE_GEAR_GEAR_HH
#define ETOILE_GEAR_GEAR_HH

namespace etoile
{
  ///
  /// this namespace contains everything for recording operations on
  /// the file system so that such operations could be replayed for instance.
  ///
  namespace gear
  {

    // nothing

  }
}

//
// ---------- includes --------------------------------------------------------
//

// XXX

#endif

    class Entrepot
    {
    public:
      //
      // enumerations
      //
      enum Type
	{
	  TypeInternal,
	  TypeExternal
	};

      //
      // types
      //
      typedef std::map<const Identifier, Context*>	Container;
      typedef Container::iterator			Iterator;
      typedef Container::const_iterator			Scoutor;

      //
      // static methods
      //
      template <typename T>
      static elle::Status	Add(const Identifier&,
			    T*);
      template <typename T>
      static elle::Status	Retrieve(const Identifier&,
				 T*&);
      static elle::Status	Remove(const Identifier&);

      //
      // static attributes
      //
      //
      // static attributes
      //
      static Container		Contexts;
    };

---

    ///
    /// this method adds the context to the container.
    ///
    template <typename T>
    elle::Status	Context::Add(const Identifier&	identifier,
				     T*			context)
    {
      std::pair<Context::Iterator, elle::Boolean>	result;

      enter();

      // insert the context in the container.
      result = Context::Contexts.insert(
	         std::pair<const Identifier, Context*>(identifier, context));

      // check the result.
      if (result.second == false)
	escape("unable to insert the context in the container");

      leave();
    }

    ///
    /// this method retrieves a recorded context.
    ///
    template <typename T>
    elle::Status	Context::Retrieve(const Identifier&	identifier,
					  T*&			context)
    {
      Context::Scoutor	scoutor;

      enter();

      // find the entry.
      if ((scoutor = Context::Contexts.find(identifier)) ==
	  Context::Contexts.end())
	escape("unable to locate the context associated with the "
	       "given identifier");

      // retrieve the context.
      context = (T*)scoutor->second;

      // update the fiber. indeed, multiple fibers may access the same
      // context. in order to alwasy track which fiber is using the
      // context, this method sets the current fiber in the context so that,
      // should the application die while the fiber is blocked, the system
      // can retrieve and delete both the application and fiber.
      context->fiber = elle::Fiber::Current;

      leave();
    }

    ///
    /// this method removes a recorded context.
    ///
    elle::Status	Context::Remove(const Identifier&	identifier)
    {
      Context::Iterator	iterator;
      Context*		context;

      enter();

      // find the entry.
      if ((iterator = Context::Contexts.find(identifier)) ==
	  Context::Contexts.end())
	escape("unable to locate the context associated with the "
	       "given identifier");

      // retrieve the context.
      context = iterator->second;

      // erase the entry.
      Context::Contexts.erase(iterator);

      leave();
    }
