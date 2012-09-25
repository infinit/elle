#ifndef NUCLEUS_PROTON_TRANSCRIPT_HH
# define NUCLEUS_PROTON_TRANSCRIPT_HH

# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Action.hh>

# include <elle/idiom/Close.hh>
#  include <vector>
# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class represents a set of actions to be performed on the
    /// storage layer such as pushing this block, removing this one and so
    /// on.
    ///
    class Transcript
    {
    public:
      //
      // types
      //
      typedef std::vector<Action*>              Container;
      typedef Container::iterator               Iterator;
      typedef Container::const_iterator         Scoutor;

      //
      // constructors & destructors
      //
      ~Transcript();

      //
      // methods
      //
      elle::Status      Push(const Address&,
                             const Block*);
      elle::Status      Wipe(const Address&);

      elle::Status      Clear(const Action::Type);
      elle::Status      Flush();

      //
      // interfaces
      //

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Container         container;
    };

  }
}

#endif
