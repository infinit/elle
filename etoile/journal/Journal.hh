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

      // XXX[temporay: clones the block through serialization]
      static
      std::unique_ptr<nucleus::proton::Block>
      clone(nucleus::neutron::Component const component,
            nucleus::proton::Block const&);
      /// Retrieve a block from the journal.
      ///
      /// This method returns true if the block is found, false otherwise.
      /// Note that this method may throw an exception should an error occur.
      static
      std::unique_ptr<nucleus::proton::Block>
      retrieve(nucleus::proton::Address const& address,
               nucleus::proton::Revision const& revision =
                 nucleus::proton::Revision::Last);

    private:
      /// Process the given scope's transcript so as to push and/or wipe some
      /// blocks.
      ///
      /// Note that this process is run it a specific background thread.
      static
      void
      _process(gear::Scope* scope);
    };
  }
}

#endif
