#include "../State.hh"

#include <common/common.hh>

#include <elle/os/path.hh>
#include <elle/idiom/Close.hh>

#include <boost/filesystem.hpp>

#include <QString>
#include <QByteArray>
#include <QLocalSocket>
#include <QProcess>

ELLE_LOG_COMPONENT("infinit.surface.gap.State");

namespace surface
{
  namespace gap
  {

    namespace fs = boost::filesystem;

    FileInfos const&
    State::file_infos(std::string const& path)
    {
      std::string abspath = elle::os::path::absolute(path, true);
      ELLE_DEBUG("Get file infos of %s", abspath);
      auto it = this->_files_infos.find(abspath);
      if (it != this->_files_infos.end())
        return *(it->second);

      std::unique_ptr<FileInfos> infos;

      for (auto const& pair : this->networks_status())
        {
          std::string mount_point = pair.second->mount_point;

          if (abspath.compare(0, mount_point.length(), abspath) != 0)
//          if (boost::algorithm::starts_with(abspath, mount_point))
            {
              std::string relpath;
              if (abspath == mount_point)
                relpath = "";
              else
                relpath = abspath.substr(mount_point.size() + 1);

              infos.reset(new FileInfos{
                  mount_point,
                  pair.first,
                  abspath,
                  relpath,
                  {},
              });
              break;
            }
        }

      if (infos.get() == nullptr)
        throw Exception(gap_error,
                        "Cannot find network for path '" + abspath + "'");

      std::string const& access_binary = common::infinit::binary_path("8access");

      QStringList arguments;
      arguments << "--user" << this->_me._id.c_str()
                << "--type" << "user"
                << "--network" << this->network(infos->network_id)._id.c_str()
                << "--path" << ("/" + infos->relative_path).c_str()
                << "--consult"
                ;

      QProcess p;
      ELLE_DEBUG("LAUNCH: %s %s", access_binary, arguments.join(" ").toStdString());
      p.start(access_binary.c_str(), arguments);
      if (!p.waitForFinished()) // .8 sec
        throw Exception(gap_internal_error, "8access binary failed");
      std::stringstream ss{p.readAllStandardOutput().constData()};

      std::string line;
      while(std::getline(ss, line))
        {
          if (!line.size())
            continue;
          std::stringstream line_stream{line};
          std::string public_key;
          int permissions;
          line_stream >> public_key /* eat "User" */
                      >> public_key
                      >> permissions;
          int gap_perm = (int) gap_none;
          if (permissions & nucleus::neutron::permissions::read)
            gap_perm |= gap_read;
          if (permissions & nucleus::neutron::permissions::write)
            gap_perm |= gap_write;
          std::string const& user_id = this->user_from_public_key(public_key)._id;
          infos->accesses[user_id] = gap_perm;
        }

      this->_files_infos[abspath] = infos.get();
      return *(infos.release());
    }

    void
    State::deprecated_set_permissions(std::string const& user_id,
                                      std::string const& abspath,
                                      nucleus::neutron::Permissions permissions,
                                      bool recursive)
    {
      FileInfos const& infos = this->file_infos(abspath);
      auto it = this->networks().find(infos.network_id);

      if (it == this->networks().end())
        throw Exception(gap_error, "Wrong network id");

      Network* network = it->second;

      std::string const& access_binary = common::infinit::binary_path("8access");

      QStringList arguments;
      arguments << "--user" << this->_me._id.c_str()
                << "--type" << "user"
                << "--grant"
                << "--network" << network->_id.c_str()
                << "--path" << ("/" + infos.relative_path).c_str()
                << "--identity" << this->user(user_id).public_key.c_str()
                ;
      if (permissions & nucleus::neutron::permissions::read)
        arguments << "--read";
      if (permissions & nucleus::neutron::permissions::write)
        arguments << "--write";

      ELLE_DEBUG("LAUNCH: %s %s", access_binary, arguments.join(" ").toStdString());

      if (permissions & gap_exec)
        {
          ELLE_WARN("XXX: setting executable permissions not yet implemented");
        }

      QProcess p;
      p.start(access_binary.c_str(), arguments);
      if (!p.waitForFinished())
        throw Exception(gap_internal_error, "8access binary failed");

      if (recursive && elle::os::path::is_directory(abspath))
        {
          boost::filesystem::directory_iterator it{abspath},
                                                end{};
          for (; it != end; ++it)
            {
              this->deprecated_set_permissions(user_id,
                                               it->path().string(),
                                               permissions,
                                               true);
            }
        }
    }

    void
    State::set_permissions(std::string const& user_id,
                           std::string const& network_id,
                           nucleus::neutron::Permissions permissions)
    {
      ELLE_TRACE("setting permissions");

      // TODO: Do this only on the current device for sender and recipient.
      if (this->_wait_portal(this->_me._id, network_id) == false)
          throw Exception{gap_error, "Couldn't find portal to infinit instance"};

      std::string const& access_binary = common::infinit::binary_path("8access");

      QStringList arguments;
      arguments << "--user" << this->_me._id.c_str()
                << "--type" << "user"
                << "--grant"
                << "--network" << network_id.c_str()
                << "--path" << "/"
                << "--identity" << this->user(user_id).public_key.c_str()
        ;
      if (permissions & nucleus::neutron::permissions::read)
        arguments << "--read";
      if (permissions & nucleus::neutron::permissions::write)
        arguments << "--write";

      ELLE_DEBUG("LAUNCH: %s %s", access_binary, arguments.join(" ").toStdString());

      if (permissions & gap_exec)
      {
        ELLE_WARN("XXX: setting executable permissions not yet implemented");
      }

      QProcess p;
      p.start(access_binary.c_str(), arguments);
      if (!p.waitForFinished())
        throw Exception(gap_internal_error, "8access binary failed");
      if (p.exitCode())
        throw Exception(gap_internal_error, "8access binary exited with errors");
    }

    size_t
    State::get_size(fs::path const& path)
    {
      if (!fs::exists(path))
        return 0;

      if (!fs::is_directory(path))
        return fs::file_size(path);

      size_t size = 0;
      fs::directory_iterator end_itr;

      for (fs::directory_iterator itr(path);
           itr != end_itr;
           ++itr)
        {
          if (fs::is_directory(itr->status()))
            {
              size += get_size(itr->path());
            }
          else
            {
              size += fs::file_size(itr->path());
            }
        }
      return size;
    }


  }
}
