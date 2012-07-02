#ifndef ELLE_CONTAINER_TIMELINE_TIMELINE_HH
# define ELLE_CONTAINER_TIMELINE_TIMELINE_HH

# include <elle/types.hh>

# include <elle/radix/Entity.hh>

# include <elle/utility/Time.hh>

# include <elle/container/timeline/Bucket.hh>

# include <elle/idiom/Close.hh>
#  include <map>
# include <elle/idiom/Open.hh>

namespace elle
{
  namespace container
  {
    ///
    /// this namespace contains all the classes related to the timeline
    /// container.
    ///
    namespace timeline
    {

      ///
      /// this container makes it easy to store objects according to a
      /// timestamp.
      ///
      /// this construct is therefore heavily used when it comes to
      /// implementing cache policies for instance.
      ///
      template <typename T>
      class Timeline:
        public radix::Entity
      {
      public:
        //
        // types
        //
        typedef std::pair<const utility::Time, Bucket<T>*>       Value;
        typedef std::map<const utility::Time, Bucket<T>*>        Container;
        typedef typename Container::iterator            Iterator;
        typedef typename Container::const_iterator      Scoutor;

        //
        // constructors & destructors
        //
        Timeline();
        ~Timeline();

        //
        // methods
        //
        Status          Insert(utility::Time const&, T const&);
        Status          Delete(utility::Time const&, T const&);

        Status          Clear();

        //
        // interfaces
        //

        // dumpable
        Status          Dump(const Natural32 = 0) const;

        //
        // attributes
        //
        Container       container;
      };

    }
  }
}

#include <elle/container/timeline/Timeline.hxx>

#endif
