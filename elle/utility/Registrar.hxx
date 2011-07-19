//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/utility/Registrar.hxx
//
// created       julien quintard   [mon jun  6 12:13:38 2011]
// updated       julien quintard   [mon jul 18 21:07:38 2011]
//

#ifndef ELLE_UTILITY_REGISTRAR_HXX
#define ELLE_UTILITY_REGISTRAR_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/radix/Trait.hh>

#include <elle/concurrency/Callback.hh>

namespace elle
{
  using namespace standalone;
  using namespace radix;
  using namespace concurrency;

  namespace utility
  {

//
// ---------- constructors & destructors --------------------------------------
//

    template <typename T>
    Registrar<T>::~Registrar()
    {
      Registrar::Scoutor	scoutor;

      // go through the functionoids.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  // delete the functionoid.
	  delete scoutor->second;
	}

      // clear the container.
      this->container.clear();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method records an association identifier and callback/closure/etc.
    /// so that, later, the associated callback/closure/etc. could be
    /// triggered with an archive containing its arguments.
    ///
    /// such a construct is very useful for network messaging or event
    /// in order to serializing method calls on a hard disk for instance.
    ///
    template <typename T>
    template <typename C>
    Status		Registrar<T>::Register(const T		identifier,
					       const C&		object)
    {
      Selectionoid<C>*				selectionoid;
      std::pair<Registrar::Iterator, Boolean>	result;

      enter(instance(selectionoid));

      // check if this identifier has already been recorded.
      if (this->container.find(identifier) != this->container.end())
	escape("this identifier seems to have already been recorded");

      // create a new selectionoid.
      selectionoid = new Selectionoid<C>(object);

      // insert the selectionoid in the container.
      result = this->container.insert(
	         std::pair<T,
			   Registrar::Functionoid*>(identifier,
						    selectionoid));

      // check if the insertion was successful.
      if (result.second == false)
	escape("unable to insert the selectoinoid in the container");

      // waive the selectionoid tracking.
      waive(selectionoid);

      leave();
    }

    ///
    /// this function takes the archive and calls the associated
    /// selectionoid whose job is to extract the necessary information
    /// and trigger an object.
    ///
    template <typename T>
    Status		Registrar<T>::Dispatch(const T		identifier,
					       Archive&		archive)
    {
      Registrar<T>::Scoutor	scoutor;

      enter();

      // retrieve the functionoid associated to the identifier.
      if ((scoutor = this->container.find(identifier)) ==
	  this->container.end())
	leave();

      // call the functionoid.
      if (scoutor->second->Call(archive) == StatusError)
	escape("an error occured in the functionoid");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// these are generated automatically.
    ///
    embed(Registrar<T>, _(template <typename T>));

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the whole registrar.
    ///
    template <typename T>
    Status		Registrar<T>::Dump(const Natural32	margin) const
    {
      String			alignment(margin, ' ');
      Registrar::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Registrar]" << std::endl;

      // dump the container.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  // dump the identifier.
	  std::cout << alignment << Dumpable::Shift
		    << "[Identifier] " << scoutor->first << std::endl;

	  // dump the functionoid.
	  if (scoutor->second->Dump(margin + 2) == StatusError)
	    escape("unable to dump the functionoid");
	}

      leave();
    }

//
// ---------- selectonoid -----------------------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename T>
    template <typename C>
    Registrar<T>::Selectionoid<C>::Selectionoid(const C&	object):
      object(object)
    {
    }

    ///
    /// this method forwards the request by calling the callback,
    /// closure or else with the archive.
    ///
    template <typename T>
    template <typename C>
    Status	Registrar<T>::Selectionoid<C>::Call(Archive&	archive) const
    {
      enter();

      // call the callback/closure/etc.
      if (this->object.Call(archive) == StatusError)
	escape("unable to call the object");

      leave();
    }

    ///
    /// this method dumps the selectionoid.
    ///
    template <typename T>
    template <typename C>
    Status	Registrar<T>::Selectionoid<C>::Dump(const Natural32 margin)
      const
    {
      enter();

      // dump the object.
      if (this->object.Dump(margin) == StatusError)
	escape("unable to dump the object");

      leave();
    }

  }
}

#endif
