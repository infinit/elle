#ifndef ETOILE_PORTAL_MANIFEST_HH
# define ETOILE_PORTAL_MANIFEST_HH

# include <etoile/fwd.hh>
# include <etoile/path/Chemin.hh>
# include <etoile/path/Way.hh>

# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Group.hh>
# include <nucleus/neutron/Range.hh>
# include <nucleus/neutron/Record.hh>

# include <protocol/RPC.hh>

namespace etoile
{
  namespace portal
  {
    struct RPC: public infinit::protocol::RPC<elle::serialize::InputBinaryArchive,
                                              elle::serialize::OutputBinaryArchive>
    {
      RPC(infinit::protocol::ChanneledStream& channels);

      /*--------.
      | General |
      `--------*/

      /// Authenticate to the portal.
      ///
      /// The phrase which is compared to the current instance's; If
      /// it's valid, the application is authorised to trigger
      /// operation on behalf of the current user.
      RemoteProcedure<bool,
                      elle::String const&> authenticate;

      /*-----.
      | Path |
      `-----*/
      RemoteProcedure<etoile::path::Chemin,
                      etoile::path::Way const&> pathresolve;
      RemoteProcedure<void,
                      etoile::path::Way const&> pathlocate;
      RemoteProcedure<void,
                      etoile::path::Way const&> pathway;

      /*-------.
      | Object |
      `-------*/
      RemoteProcedure<etoile::gear::Identifier,
                      etoile::path::Chemin const&> objectload;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> objectinformation;
      RemoteProcedure<void,
                      etoile::abstract::Object&> objectabstract;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> objectdiscard;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> objectstore;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> objectdestroy;

      /*-----.
      | File |
      `-----*/
      RemoteProcedure<void> filecreate;
      RemoteProcedure<void,
                      etoile::path::Chemin&> fileload;
      RemoteProcedure<void> filewrite;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&,
                      nucleus::neutron::Offset&,
                      nucleus::neutron::Size&> fileread;
      RemoteProcedure<void,
                      elle::standalone::Region&> fileregion;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&,
                      nucleus::neutron::Size&> fileadjust;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> filediscard;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> filestore;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> filedestroy;

      /*----------.
      | Directory |
      `----------*/
      RemoteProcedure<void> directorycreate;
      RemoteProcedure<void,
                      etoile::path::Chemin&> directoryload;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&,
                      etoile::path::Slab&,
                      etoile::gear::Identifier const&> directoryadd;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&,
                      etoile::path::Slab&> directorylookup;
      RemoteProcedure<void,
                      nucleus::neutron::Entry&> directoryentry;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&,
                      nucleus::neutron::Index&,
                      nucleus::neutron::Size&> directoryconsult;
      RemoteProcedure<void,
                      nucleus::neutron::Range<nucleus::neutron::Entry>&> directoryrange;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&,
                      etoile::path::Slab&,
                      etoile::path::Slab&> directoryrename;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&,
                      etoile::path::Slab&> directoryremove;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> directorydiscard;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> directorystore;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> directorydestroy;

      /*-----.
      | Link |
      `-----*/
      RemoteProcedure<void> linkcreate;
      RemoteProcedure<void,
                      etoile::path::Chemin&> linkload;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&,
                      etoile::path::Way const&> linkbind;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> linkresolve;
      RemoteProcedure<void,
                      etoile::path::Way const&> linkway;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> linkdiscard;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> linkstore;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> linkdestroy;

      /*-------.
      | Access |
      `-------*/
      RemoteProcedure<std::unique_ptr<const nucleus::neutron::Record>,
                      etoile::gear::Identifier const&,
                      nucleus::neutron::Subject const&> accesslookup;
      RemoteProcedure<nucleus::neutron::Range<nucleus::neutron::Record>,
                      etoile::gear::Identifier const&,
                      nucleus::neutron::Index const&,
                      nucleus::neutron::Size const&> accessconsult;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&,
                      nucleus::neutron::Subject const&,
                      nucleus::neutron::Permissions const&> accessgrant;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&,
                      nucleus::neutron::Subject const&> accessrevoke;

      /*-----------.
      | Attributes |
      `-----------*/
      RemoteProcedure<void,
                      etoile::gear::Identifier const&,
                      elle::String const&,
                      elle::String const&> attributesset;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&,
                      elle::String const&> attributesget;
      RemoteProcedure<void,
                      nucleus::neutron::Trait&> attributestrait;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> attributesfetch;
      RemoteProcedure<void,
                      nucleus::neutron::Range<nucleus::neutron::Trait>&> attributesrange;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&,
                      elle::String const&> attributesomit;

      /*------.
      | Group |
      `------*/
      RemoteProcedure<std::pair<typename nucleus::neutron::Group::Identity,
                                etoile::gear::Identifier>,
                      elle::String const&> groupcreate;
      RemoteProcedure<etoile::gear::Identifier,
                      typename nucleus::neutron::Group::Identity const&> groupload;
      RemoteProcedure<etoile::abstract::Group,
                      etoile::gear::Identifier const&> groupinformation;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&,
                      nucleus::neutron::Subject const&> groupadd;
      RemoteProcedure<std::unique_ptr<const nucleus::neutron::Fellow>,
                      etoile::gear::Identifier const&,
                      nucleus::neutron::Subject const&> grouplookup;
      RemoteProcedure<nucleus::neutron::Range<nucleus::neutron::Fellow>,
                      etoile::gear::Identifier const&,
                      nucleus::neutron::Index&,
                      nucleus::neutron::Size&> groupconsult;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&,
                      nucleus::neutron::Subject const&> groupremove;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> groupdiscard;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> groupstore;
      RemoteProcedure<void,
                      etoile::gear::Identifier const&> groupdestroy;
    };
  }
}

#endif
