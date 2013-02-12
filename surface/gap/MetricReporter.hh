#ifndef GAP_ANALIZER_HH
# define GAP_ANALIZER_HH

# include <elle/types.hh>
# include <elle/print.hh>
# include <elle/HttpClient.hh>
# include <elle/utility/Time.hh>
# include <queue>
# include <list>
# include <fstream>
# include <reactor/scheduler.hh>
# include <memory>
# include <thread>

namespace surface
{
  namespace gap
  {

    class MetricReporter
    {
      /*------.
      | Types |
      `------*/
    public:
      typedef std::list<std::pair<std::string, std::string>> Metric;
      typedef std::pair<elle::utility::Time, Metric> TimeMetricPair;

      /*-------------.
      | Construction |
      `-------------*/
      public:
      MetricReporter(std::string const& tag,
                     std::string const& default_user,
                     std::string const& host,
                     uint16_t port);

      virtual
      ~MetricReporter();

      MetricReporter(MetricReporter const&) = delete;
      MetricReporter(MetricReporter&&) = delete;


    public:
      // Push data directly, without enqueuing.
      virtual
      void
      publish(std::string const& name,
              Metric const&);

      // Enqueue data.
      virtual
      void
      store(std::string const& name, Metric const&);

      // Sugar: Store(name, {})
      void
      store(std::string const& caller);

      // Sugar: Store(name, {key, value})
      void
      store(std::string const& name,
            std::string const& key,
            std::string const& value);

      // Defaultly, user is anonymous.
      void
      update_user(std::string const&);

    protected:
      // Do send to destination.
      virtual
      void
      _send_data(TimeMetricPair const&);

    protected:
      elle::utility::Time _last_sent;
      std::string _tag;
      std::string _user_id;
      reactor::Scheduler _flusher_sched;
      std::unique_ptr<boost::asio::io_service::work> _keep_alive;
      std::unique_ptr<std::thread> _run_thread;
      std::unique_ptr<elle::HTTPClient> _server;
    };


    namespace metrics
    {
      namespace google
      {

        //XXX: create an enum and a map that list with a user friendly name the
        // key of the field such as Key::session_control instead of "sc".
        static const std::pair<std::string, std::string> Success{"cd1", "success"};
        static const std::pair<std::string, std::string> Failure{"cd1", "failure"};
        static const std::pair<std::string, std::string> Pending{"cd1", "pending"};

        std::string
        retrieve_id(std::string const& path);

        void
        update_id(std::string const& id);

        // Create a static MetricReporter with google analytics server.
        MetricReporter&
        server(std::string const& host = "",
               uint16_t port = 80,
               std::string const& tag = "",
               std::string const& id = "");
      }
    }
  }
}

#endif
