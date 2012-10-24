#ifndef  PLASMA_WATCHDOG_INFINITNETWORK_HH
# define PLASMA_WATCHDOG_INFINITNETWORK_HH

# include <elle/idiom/Close.hh>

# include <string>

# include <QDir>
# include <QProcess>

# include "plasma/meta/Client.hh"

namespace plasma
{
  namespace watchdog
  {

    class Manager;
    namespace meta = ::plasma::meta;

    class InfinitNetwork
      : QObject
    {
      Q_OBJECT

    private:
      meta::NetworkResponse   _description;
      Manager&                _manager;
      QProcess                _process;
      std::string             _network_dir;
      std::string             _mount_point;

    public:
      InfinitNetwork(Manager& manager,
                     meta::NetworkResponse const& response);
      ~InfinitNetwork();
      void update(meta::NetworkResponse const& response);
      void stop();

      std::string mount_point() const;
      std::string const& id() const;

    private:
      void _prepare_directory();
      void _update();
      void _create_network_root_block(std::string const& id);
      void _on_got_descriptor(meta::UpdateNetworkResponse const& response);
      void _on_any_error(elle::ResponseCode error,
                         std::string const& reason);
      void _register_device();
      void _on_network_nodes(meta::NetworkNodesResponse const& response);
      void _start_process();

    private Q_SLOTS:
      void _on_process_started();
      void _on_process_error(QProcess::ProcessError error);
      void _on_process_finished(int exit_code,
                                QProcess::ExitStatus exit_status);
    };

  }
}

#endif /* ! INFINITNETWORK_HH */
