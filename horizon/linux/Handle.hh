#ifndef HORIZON_LINUX_HANDLE_HH
# define HORIZON_LINUX_HANDLE_HH

# include <elle/types.hh>

# include <etoile/gear/Identifier.hh>

# include <nucleus/neutron/Permissions.hh>

namespace horizon
{
  namespace linux
  {

    ///
    /// this class represents a handle which is passed between the FUSE
    /// upcalls through the _info->fh_ attribute.
    ///
    class Handle
    {
    public:
      //
      // enumerations
      //
      enum Operation
        {
          OperationGetattr,
          OperationOpendir,
          OperationCreate,
          OperationOpen,
          OperationTruncate
        };

      //
      // constructors & destructors
      //
      Handle(const Operation,
             const etoile::gear::Identifier&);
      Handle(const Operation,
             const etoile::gear::Identifier&,
             const nucleus::neutron::Permissions&);

      //
      // attributes
      //
      Operation                 operation;
      etoile::gear::Identifier  identifier;

      union
      {
        nucleus::neutron::Permissions    permissions;
      };
    };

  }
}

#endif
