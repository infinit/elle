#ifndef ETOILE_JOURNAL_JOURNAL_HH
# define ETOILE_JOURNAL_JOURNAL_HH

# include <elle/types.hh>

# include <etoile/gear/fwd.hh>

# include <nucleus/proton/Address.hh>
# include <nucleus/proton/Revision.hh>
# include <nucleus/proton/Block.hh>

namespace etoile
{
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
                              nucleus::proton::Revision const& revision,
                              nucleus::proton::Block& out_block);

    private:
      static elle::Status _Record(gear::Scope* scope);
    };

  }
}

#endif
