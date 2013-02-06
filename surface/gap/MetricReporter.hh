#ifndef GAP_ANALIZER_HH
# define GAP_ANALIZER_HH

# include <elle/types.hh>
# include <elle/print.hh>
# include <elle/HttpClient.hh>
# include <elle/utility/Time.hh>
# include <queue>
# include <list>
# include <fstream>
# include <mutex>

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
                     std::string const& default_user);

      virtual
      ~MetricReporter();

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

      /// XXX
      void
      flush();
    protected:
      // Do send to destination.
      virtual
      void
      _send_data(TimeMetricPair const&) = 0;

      // Enqueue a metric timestamped.
      Metric&
      _push(Metric const& metric);

      virtual
      void
      _flush();

    protected:
      std::queue<TimeMetricPair> _requests;
      elle::utility::Time _last_sent;
      std::string _tag;
      std::string _user_id;
      std::mutex _store_mutex;
    };

    class ServerReporter:
      public MetricReporter,
      private boost::noncopyable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      /// Default constructor.
      ServerReporter(std::string const& tag,
                     std::string const& default_user,
                     std::string const& server,
                     uint16_t port);

      ~ServerReporter();

      /*--------.
      | Methods |
      `--------*/
    public:
      void
      flush();
    private:
      virtual
      void
      _send_data(TimeMetricPair const&) override;

      void
      _flush() override;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      std::string _host;
      uint16_t _port;
      std::unique_ptr<elle::HTTPClient> _server;
      std::queue<TimeMetricPair> _fallback_storage;
    };

    class NoConnectionReporter:
      public MetricReporter,
      private boost::noncopyable
    {
    public:
      NoConnectionReporter(std::string const& tag,
                           std::string const& default_user,
                           std::string const& path);

      ~NoConnectionReporter();

    private:
      void
      _send_data(MetricReporter::TimeMetricPair const&) override;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      // Flush here, cause it's offline.
      std::ofstream _file_storage;
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
