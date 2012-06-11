
#include <elle/log.hh>

#include <boost/lexical_cast.hpp>

#include <elle/cryptography/Digest.hh>
#include <nucleus/proton/MutableBlock.hh>
#include <nucleus/proton/History.hh>

#include <elle/radix/Variables.hh>
#include <elle/io/File.hh>
#include <elle/io/Piece.hh>

#include <nucleus/proton/MutableBlock.hh>
#include <nucleus/proton/History.hh>

#include <lune/Lune.hh>
#include <hole/Hole.hh>
#include <Infinit.hh>

namespace nucleus
{
  namespace proton
  {

    ELLE_LOG_TRACE_COMPONENT("nucleus.proton.MutableBlock");

//
// ---------- constructs & destructors ----------------------------------------
//

    ///
    /// default constructor.
    ///
    MutableBlock::MutableBlock():
      Block(FamilyUnknown, neutron::ComponentUnknown),

      version(Version::First)
    {
    }

    ///
    /// specific constructor.
    ///
    MutableBlock::MutableBlock(const Family                     family,
                               const neutron::Component         component):
      Block(family, component),

      version(Version::First)
    {
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(MutableBlock, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the block's internals.
    ///
    elle::Status        MutableBlock::Dump(const elle::Natural32 margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[MutableBlock]" << std::endl;

      // dump the parent.
      if (Block::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the parent");

      // dump the version.
      if (this->version.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the version");

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method archives the block attributes.
    ///
    //elle::Status        MutableBlock::Serialize(elle::Archive&  archive) const
    //{
    //  // serialize the parent class.
    //  if (Block::Serialize(archive) == elle::Status::Error)
    //    escape("unable to serialize the underlying block");

    //  // serialize the attributes.
    //  if (archive.Serialize(this->version) == elle::Status::Error)
    //    escape("unable to serialize the attributess");

    //  return elle::Status::Ok;
    //}

    /////
    ///// this method extracts the attributes.
    /////
    //elle::Status        MutableBlock::Extract(elle::Archive&    archive)
    //{
    //  // extract the parent class.
    //  if (Block::Extract(archive) == elle::Status::Error)
    //    escape("unable to extract the underlying block");

    //  // extracts the attributes.
    //  if (archive.Extract(this->version) == elle::Status::Error)
    //    escape("unable to extract the attributes");

    //  return elle::Status::Ok;
    //}

//
// ---------- fileable --------------------------------------------------------
//

    ///
    /// this method loads the block.
    ///
    elle::Status        MutableBlock::Load(const Network&       network,
                                           const Address&       address,
                                           const Version&       version)
    {
      elle::Path        path;
      elle::String      unique;

      // first, turn the block's address into a hexadecimal string.
      if (address.digest->Save(unique) == elle::Status::Error)
        escape("unable to convert the address in its hexadecimal form");

      ELLE_LOG_TRACE("Load mutable block from address %s", unique);

      // debug.
      if (Infinit::Configuration.nucleus.debug == true)
        printf("[nucleus] proton::MutableBlock::Load(%s)\n",
               unique.c_str());

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::MutableBlock) ==
          elle::Status::Error)
        escape("unable to create the path");

      // operate depending on the network's support of history.
      if (hole::Hole::Descriptor.history == false)
        {
          elle::standalone::Region  region;

          // complete the path with the network name.
          if (path.Complete(elle::io::Piece("%NETWORK%", network.name),
                            elle::io::Piece("%ADDRESS%", unique),
                            elle::io::Piece("%VERSION%", "@")) ==
              elle::Status::Error)
            escape("unable to complete the path");

          if (this->Load(path) == elle::Status::Error)
            escape("unable to load block");

          //// read the file's content.
          //if (elle::io::File::Read(path, region) == elle::Status::Error)
          //  escape("unable to read the file's content");

          //// wrap the region into an archive.
          //if (archive.Wrap(region) == elle::Status::Error)
          //  escape("unable to prepare the archive");

          //// extract from the archive.
          //if (archive.Extract(*this) == elle::Status::Error)
          //  escape("unable to extract the archive");
        }
      else
        {
          elle::standalone::Region  region;
          elle::String  number;

          // if the requested version is the latest...
          if (version == Version::Last)
            {
              elle::Path        link;
              elle::standalone::Region      r;

              // copy the path.
              link = path;

              // complete the link path.
              if (link.Complete(elle::io::Piece("%NETWORK%", network.name),
                                elle::io::Piece("%ADDRESS%", unique),
                                elle::io::Piece("%VERSION%", "@")) ==
                  elle::Status::Error)
                escape("unable to complete the path");

              // read the file's content.
              if (elle::io::File::Read(link, r) == elle::Status::Error)
                escape("unable to read the file's content");

              // set the number of the latest version.
              number = elle::String(reinterpret_cast<const char*>(r.contents),
                                    r.size);
            }
          else
            {
              try
                {
                  number = boost::lexical_cast<std::string>(version.number);
                }
              catch (std::exception const& err)
                {
                  escape(
                      "unable to convert the version number into a string: %s",
                      err.what()
                  );
                }
            }

          // complete the path with the network name.
          if (path.Complete(elle::io::Piece("%NETWORK%", network.name),
                            elle::io::Piece("%ADDRESS%", unique),
                            elle::io::Piece("%VERSION%", number)) ==
              elle::Status::Error)
            escape("unable to complete the path");

          if (this->Load(path) == elle::Status::Error)
            escape("unable to load '%s'", path.str().c_str());
        }

      return elle::Status::Ok;
    }

    ///
    /// this method stores the block in its file format.
    ///
    elle::Status        MutableBlock::Store(const Network&      network,
                                            const Address&      address)
      const
    {
      elle::Path        file;
      elle::String      unique;


      if (address.digest->Save(unique) == elle::Status::Error)
        escape("unable to convert the address in its hexadecimal form");

      ELLE_LOG_TRACE("Store mutable block from address %s", unique);
      // debug.
      if (Infinit::Configuration.nucleus.debug == true)
        printf("[nucleus] proton::MutableBlock::Store(%s)\n",
               unique.c_str());

      // create the shelter path.
      if (file.Create(lune::Lune::Network::Shelter::MutableBlock) ==
          elle::Status::Error)
        escape("unable to create the path");

      // complete the path with the network name.
      if (file.Complete(elle::io::Piece("%NETWORK%", network.name),
                        elle::io::Piece("%ADDRESS%", unique)) == elle::Status::Error)
        escape("unable to complete the path");

      // operate depending on the network's support of history.
      if (hole::Hole::Descriptor.history == false)
        {
          //
          // if the history is not supported, store the mutable block
          // in a file without version number extension.
          //
          elle::standalone::Region          region;
          MutableBlock          block;

          // complete the file path.
          if (file.Complete(elle::io::Piece("%VERSION%", "@")) ==
              elle::Status::Error)
            escape("unable to complete the path");

          // does the block already exist.
          // XXX This block checks the version, need to put this test back !
          //if (block.Exist(network,
          //                address,
          //                nucleus::Version::Last) == elle::Status::True)
          //  {
          //    MutableBlock      current;

          //    ELLE_LOG_TRACE(
          //        "The block %p already exists, fetch the last version",
          //        &block
          //    ) {
          //        // load the latest version.
          //        if (current.Load(network,
          //                         address,
          //                         nucleus::Version::Last) == elle::Status::Error)
          //          escape("unable to load the current block");

          //        // does the given block derive the current version.
          //        if (!(this->version > current.version))
          //          escape("the block to store does not seem to derive the "
          //                 "current version");
          //      }
          //  }

          if (this->Store(file) == elle::Status::Error)
            escape("Cannot store into %s", file.str().c_str());
        }
      else
        {
          //
          // otherwise, store the block in a file with a name of the
          // form [identifier]#[version number].blk. besides, a special
          // file of the form [identifier]#@.blk contains the number
          // of the latest version of the mutable block.
          //
          elle::String          number;
          elle::Path            link;
          elle::standalone::Region          region;
          nucleus::History      history;

          try
            {
              number = boost::lexical_cast<std::string>(this->version.number);
            }
          catch (std::exception const&)
            {
              escape("unable to transform the version number into a string");
            }

          // duplicate the path.
          link = file;

          // complete the file path.
          if (file.Complete(elle::io::Piece("%VERSION%", number)) ==
              elle::Status::Error)
            escape("unable to complete the path");

          if (this->Store(file) != elle::Status::Ok)
            escape("unable to store the block");

          // complete the link path.
          if (link.Complete(elle::io::Piece("%VERSION%", "@")) ==
              elle::Status::Error)
            escape("unable to complete the path");

          // if there is a link, remove it.
          if (elle::io::File::Exist(link) == elle::Status::True)
            {
              // delete the file which references the latest version since a
              // new version has been created.
              if (elle::io::File::Erase(link) == elle::Status::Error)
                escape("unable to erase the block link");
            }

          // finally, create the block link.
          if (elle::io::File::Write(
                link,
                elle::standalone::Region(
                  reinterpret_cast<const elle::Byte*>(number.c_str()),
                  number.length())) == elle::Status::Error)
            escape("unable to create the block link");

          // XXX here the history should be loaded before the file is
          // XXX being stored. then the file should be stored only if the
          // XXX file does not already exist. finally, the @ link should be
          // XXX updated (if the file has been stored) with the latest version
          // XXX number (which is not necessarily the one given here; indeed
          // XXX this method could be called for version 4 though a version
          // XXX 32 already exists). finally, the history should be re-worked
          // XXX so as to used a sorted data structure such as a map which
          // XXX would then be used to retrieve the latest version number.
          escape("XXX");

          // if there is a history, load it.
          if (history.Exist(network, address) == elle::Status::True)
            {
              // load the history.
              if (history.Load(network, address) == elle::Status::Error)
                escape("unable to load the history");
            }

          // register the new version.
          if (history.Register(version) == elle::Status::Error)
            escape("unable to register the new version");

          // store the history.
          if (history.Store(network, address) == elle::Status::Error)
            escape("unable to store the history");
        }

      return elle::Status::Ok;
    }

    ///
    /// this method erases a block.
    ///
    elle::Status        MutableBlock::Erase(const Network&      network,
                                            const Address&      address)
    {
      elle::Unique      unique;
      elle::Path        path;


      // turn the block's address into a hexadecimal string.
      if (address.digest->Save(unique) == elle::Status::Error)
        escape("unable to convert the address in its hexadecimal form");

      // debug.
      if (Infinit::Configuration.nucleus.debug == true)
        printf("[nucleus] proton::MutableBlock::Erase(%s)\n",
               unique.c_str());

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::MutableBlock) ==
          elle::Status::Error)
        escape("unable to create the path");

      // operate depending on the network's support of history.
      if (hole::Hole::Descriptor.history == false)
        {
          // complete the path with the network name.
          if (path.Complete(elle::io::Piece("%NETWORK%", network.name),
                            elle::io::Piece("%ADDRESS%", unique),
                            elle::io::Piece("%VERSION%", "@")) ==
              elle::Status::Error)
            escape("unable to complete the path");

          // is the file present...
          if (elle::io::File::Exist(path) == elle::Status::True)
            {
              // erase the file.
              if (elle::io::File::Erase(path) == elle::Status::Error)
                escape("unable to erase the file");
            }
        }
      else
        {
          History       history;
          Version::Type size;
          Version::Type i;

          // complete the path with the network name.
          if (path.Complete(elle::io::Piece("%NETWORK%", network.name),
                            elle::io::Piece("%ADDRESS%", unique)) ==
              elle::Status::Error)
            escape("unable to complete the path");

          // load the history.
          if (history.Load(network, address) == elle::Status::Error)
            escape("unable to load the history");

          // retrieve the number of versions.
          if (history.Size(size) == elle::Status::Error)
            escape("unable to retrieve the size of the history");

          // go through the versions.
          for (i = 0; i < size; i++)
            {
              nucleus::Version  version;
              elle::String      number;
              elle::Path        file;

              // select a particular version.
              if (history.Select(i, version) == elle::Status::Error)
                escape("unable to select a particular version");

              try
                {
                  number = boost::lexical_cast<std::string>(version.number);
                }
              catch (std::exception const&)
                {
                  escape("unable to transform the version number into a string");
                }

              // duplicate the generic path.
              file = path;

              // complete the path with the version number.
              if (file.Complete(elle::io::Piece("%VERSION%",
                                            number)) == elle::Status::Error)
                escape("unable to complete the path");

              // is the file present...
              if (elle::io::File::Exist(file) == elle::Status::True)
                {
                  // erase the file.
                  if (elle::io::File::Erase(file) == elle::Status::Error)
                    escape("unable to erase the file");
                }
            }

          // complete the path with the last version pointer.
          if (path.Complete(elle::io::Piece("%VERSION%", "@")) ==
              elle::Status::Error)
            escape("unable to complete the path");

          // is the file present...
          if (elle::io::File::Exist(path) == elle::Status::True)
            {
              // delete the link which references the latest version.
              if (elle::io::File::Erase(path) == elle::Status::Error)
                escape("unable to erase the block link");
            }

          // erase the history.
          if (history.Erase(network, address) == elle::Status::Error)
            escape("unable to erase the history");
        }

      return elle::Status::Ok;
    }

    ///
    /// this method returns true if the block exists.
    ///
    elle::Status        MutableBlock::Exist(const Network&      network,
                                            const Address&      address,
                                            const Version&      version)
      const
    {
      elle::Path                path;
      elle::String              unique;

      // first, turn the block's address into a hexadecimal string.
      if (address.digest->Save(unique) == elle::Status::Error)
        flee("unable to convert the address in its hexadecimal form");

      // debug.
      if (Infinit::Configuration.nucleus.debug == true)
        printf("[nucleus] proton::MutableBlock::Exist(%s)\n",
               unique.c_str());

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::MutableBlock) ==
          elle::Status::Error)
        flee("unable to create the path");

      // operate depending on the network's support of history.
      if (hole::Hole::Descriptor.history == false)
        {
          // complete the path with the network name.
          if (path.Complete(elle::io::Piece("%NETWORK%", network.name),
                            elle::io::Piece("%ADDRESS%", unique),
                            elle::io::Piece("%VERSION%", "@")) ==
              elle::Status::Error)
            flee("unable to complete the path");

          // test the file.
          if (elle::io::File::Exist(path) == elle::Status::True)
            return elle::Status::True;
        }
      else
        {
          elle::String  number;

          // if the requested version is the latest...
          if (version == Version::Last)
            {
              elle::Path        link;
              elle::standalone::Region      r;

              // copy the path.
              link = path;

              // complete the link path.
              if (link.Complete(elle::io::Piece("%NETWORK%", network.name),
                                elle::io::Piece("%ADDRESS%", unique),
                                elle::io::Piece("%VERSION%", "@")) ==
                  elle::Status::Error)
                flee("unable to complete the path");

              // read the file's content.
              if (elle::io::File::Read(link, r) == elle::Status::Error)
                flee("unable to read the file's content");

              // set the number of the latest version.
              number = elle::String(reinterpret_cast<const char*>(r.contents),
                                    r.size);
            }
          else
            {
              try
                {
                  number = boost::lexical_cast<std::string>(version.number);
                }
              catch (std::exception const&)
                {
                  flee("unable to convert the version number into a string");
                }
            }

          // complete the path with the network name.
          if (path.Complete(elle::io::Piece("%NETWORK%", network.name),
                            elle::io::Piece("%ADDRESS%", unique),
                            elle::io::Piece("%VERSION%", number)) ==
              elle::Status::Error)
            flee("unable to complete the path");

          // test the file.
          if (elle::io::File::Exist(path) == elle::Status::True)
            return elle::Status::True;
        }

      return elle::Status::False;
    }

  }
}
