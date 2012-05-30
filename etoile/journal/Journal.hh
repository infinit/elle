#ifndef ETOILE_JOURNAL_JOURNAL_HH
# define ETOILE_JOURNAL_JOURNAL_HH

# include <elle/types.hh>

# include <nucleus/proton/Address.hh>
# include <nucleus/proton/Version.hh>
# include <nucleus/proton/Block.hh>

namespace etoile
{
  namespace gear
  {

    ///
    /// this class needs to be forward-declared in order to prevent
    /// conflicts.
    ///
    class Scope;

  }

  ///
  /// this namespace contains everything related to the journal which
  /// is responsible for recording and triggering the storage layer operations.
  ///
  namespace journal
  {

    ///
    /// this class represents the journal manager.
    ///
    class Journal
    {
    private:
      static std::set<gear::Scope*>  _scopes;
    public:
      static elle::Status Record(gear::Scope* scope);

      /// Retreive a block from the Journal. returns true if the block is found,
      /// or throws an exception.
      elle::Boolean get_block(nucleus::proton::Address const& address,
                              nucleus::proton::Version const& version,
                              nucleus::proton::Block& out_block);

    private:
      static elle::Status _Record(gear::Scope* scope);
    };

  }
}

#include <etoile/gear/Scope.hh>

#endif
