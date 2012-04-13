//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [sat aug  8 16:26:41 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Path.hh>
#include <etoile/path/Route.hh>

#include <agent/Agent.hh>
#include <hole/Hole.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this route represents a null route.
    ///
    const Route                         Route::Null;

    ///
    /// this route represents the root directory.
    ///
    Route                               Route::Root;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the path system.
    ///
    elle::Status        Route::Initialize()
    {
      Way               root(elle::System::Path::Separator);

      // create the reference root route.
      if (Route::Root.Create(root) == elle::Status::Error)
        escape("unable to create the route");

      return elle::Status::Ok;
    }

    ///
    /// this method cleans the path system.
    ///
    elle::Status        Route::Clean()
    {
      // nothing to do.

      return elle::Status::Ok;
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Route::Route()
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a route from a string by splitting it according
    /// to the path separator.
    ///
    /// note that the first slab is always empty in order to represent
    /// the root directory.
    ///
    /// the following assumes the root version indicator is '@' while the
    /// slab version indicator is '%'.
    ///
    /// note that the ways can embed version numbers as shown next:
    ///
    ///   /suce%42/avale/leche.txt%3
    ///
    /// however, the format '%[0-9]+' cannot be used with the root directory
    /// since the way always starts with '/...'.
    ///
    /// in order to provide this functionality, the following check is made:
    /// if the first non-empty slab starts with '@[0-9]+', then this slab is
    /// used as the root one with the appropriate version number.
    ///
    elle::Status        Route::Create(const Way&                way)
    {
      elle::String::size_type   start;
      elle::String::size_type   end;
      Slab                      slab;

      // check that the way starts with a leading '/'
      if (way.path[0] != elle::System::Path::Separator)
        escape("the path must contain the leading path separator '%c'",
               elle::System::Path::Separator);

      // clear the elements.
      this->elements.clear();

      // compute the next offsets.
      start =
        way.path.find_first_not_of(elle::System::Path::Separator);
      end =
        way.path.find_first_of(elle::System::Path::Separator, start);

      // check if at least one slab is present.
      if (start < way.path.length())
        {
          // create the slab.
          slab = way.path.substr(start, end - start);

          // check if the slab represents the root directory i.e starts
          // with '@' and follows with a possible version number, should
          // the network support history though.
          if ((hole::Hole::Descriptor.history == true) &&
              (Infinit::Configuration.etoile.history.status == true) &&
              (slab[0] ==
               Infinit::Configuration.etoile.history.indicator.root))
            {
              // modify the '@' character with the version indicator '%'.
              slab[0] = Infinit::Configuration.etoile.history.indicator.slab;

              // record the slab.
              this->elements.push_back(slab);

              // compute the next offsets.
              start =
                way.path.find_first_not_of(elle::System::Path::Separator, end);
              end =
                way.path.find_first_of(elle::System::Path::Separator, start);
            }
        }

      // if no slab is present or the first slab does not represent the
      // root directory---i.e the elements container is empty---register
      // an empty root slab.
      if (this->elements.empty() == true)
        {
          // record an empty root slab.
          this->elements.push_back("");
        }

      // then, go through the string.
      while (start < way.path.length())
        {
          // create the slab.
          slab = way.path.substr(start, end - start);

          // add the section to the container.
          this->elements.push_back(slab);

          // compute the next offsets.
          start =
            way.path.find_first_not_of(elle::System::Path::Separator, end);
          end =
            way.path.find_first_of(elle::System::Path::Separator, start);
        }

      return elle::Status::Ok;
    }

    ///
    /// this method creates a route by appending a name to an existing route.
    ///
    elle::Status        Route::Create(const Route&              route,
                                      const Slab&               slab)
    {
      // copy the elements.
      this->elements = route.elements;

      // add the slab.
      this->elements.push_back(slab);

      return elle::Status::Ok;
    }

    ///
    /// this method returns true if the current route derives the
    /// given base i.e the base's elements appear in the route.
    ///
    elle::Status        Route::Derives(const Route&             base) const
    {
      auto              i = base.elements.begin();
      auto              j = this->elements.begin();
      auto              end = base.elements.end();

      if (base.elements.size() > this->elements.size())
        return (elle::Status::False);

      for(; i != end; ++i, ++j)
        {
          if (*i != *j)
            return (elle::Status::False);
        }

      return (elle::Status::True);
    }

    ///
    /// this method clears the route's content.
    ///
    elle::Status        Route::Clear()
    {
      // clear the container.
      this->elements.clear();

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Route::operator==(const Route&          element) const
    {
      Route::Scoutor    s;
      Route::Scoutor    t;

      // check the address as this may actually be the same object.
      if (this == &element)
        return elle::Status::True;

      // compare the size.
      if (this->elements.size() != element.elements.size())
        return elle::Status::False;

      // for every element.
      for (s = this->elements.begin(), t = element.elements.begin();
           s != this->elements.end();
           s++, t++)
        if (*s != *t)
          return elle::Status::False;

      return elle::Status::True;
    }

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Route::operator<(const Route&           element) const
    {
      Route::Scoutor    s;
      Route::Scoutor    t;

      // check the address as this may actually be the same object.
      if (this == &element)
        return elle::Status::False;

      // compare the size.
      if (this->elements.size() < element.elements.size())
        return elle::Status::True;
      else if (this->elements.size() > element.elements.size())
        return elle::Status::False;

      // for every element.
      for (s = this->elements.begin(), t = element.elements.begin();
           s != this->elements.end();
           s++, t++)
        {
          if (*s < *t)
            return elle::Status::True;
          else if (*s > *t)
            return elle::Status::False;
        }

      // at this point, both routes seem identical.
      return elle::Status::False;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Route, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a route.
    ///
    elle::Status        Route::Dump(const elle::Natural32       margin) const
    {
      elle::String      alignment(margin, ' ');
      Route::Scoutor    scoutor;

      std::cout << alignment << "[Route] " << this
                << " #" << std::dec
                << this->elements.size() << std::endl;

      // for every element.
      for (scoutor = this->elements.begin();
           scoutor != this->elements.end();
           scoutor++)
        {
          // dump the slab.
          std::cout << alignment << elle::Dumpable::Shift
                    << *scoutor << std::endl;
        }

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the object.
    ///
    elle::Status        Route::Serialize(elle::Archive&         archive) const
    {
      Route::Scoutor    scoutor;
      elle::Natural32   size;

      // retrieve the container size.
      size = this->elements.size();

      // serialize the size.
      if (archive.Serialize(size) == elle::Status::Error)
        escape("unable to serialize the size");

      // for every element.
      for (scoutor = this->elements.begin();
           scoutor != this->elements.end();
           scoutor++)
        {
          // serialize the slab.
          if (archive.Serialize(*scoutor) == elle::Status::Error)
            escape("unable to serialize the slab");
        }

      return elle::Status::Ok;
    }

    ///
    /// this method extracts the object.
    ///
    elle::Status        Route::Extract(elle::Archive&           archive)
    {
      elle::Natural32   size;
      elle::Natural32   i;

      // extract the size.
      if (archive.Extract(size) == elle::Status::Error)
        escape("unable to extract the size");

      // for every element.
      for (i = 0; i < size; i++)
        {
          Slab          slab;

          // extract the slab.
          if (archive.Extract(slab) == elle::Status::Error)
            escape("unable to extract the slab");

          // add the slab.
          this->elements.push_back(slab);
        }

      return elle::Status::Ok;
    }

  }
}
