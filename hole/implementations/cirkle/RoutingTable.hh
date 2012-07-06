#ifndef HOLE_IMPLEMENTATIONS_CIRKLE_ROUTINGTABLE_HH
# define HOLE_IMPLEMENTATIONS_CIRKLE_ROUTINGTABLE_HH

# include <elle/types.hh>

# include <hole/Label.hh>
# include <hole/implementations/cirkle/Neighbour.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      class RoutingTable:
        public elle::radix::Entity
      {
      public:
        //
        // types
        //
        typedef std::map<const Label, Neighbour*>       Container;
        typedef typename Container::iterator            Iterator;
        typedef typename Container::const_iterator      Scoutor;

        //
        // constructors & destructors
        //
        ~RoutingTable();

        //
        // methods
        //
        elle::Status            Add(const Label&,
                                    Neighbour*);
        elle::Status            Exist(const Label&);
        elle::Status            Retrieve(const Label&,
                                         Neighbour*&);
        elle::Status            Remove(const Label&);
        elle::Status            Locate(const Label&,
                                       Iterator* = nullptr);

        //
        // interfaces
        //

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;

        //
        // attributes
        //
        Container               container;
      };

    }
  }
}

#endif
