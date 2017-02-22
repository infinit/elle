#pragma once

#include <queue>
#include <string>

#include <elle/Buffer.hh>
#include <elle/memory.hh>
#include <elle/reactor/Barrier.hh>
#include <elle/reactor/http/Request.hh>
#include <elle/reactor/http/fwd.hh>
#include <elle/reactor/signal.hh>

namespace elle
{
  namespace reactor
  {
    namespace http
    {
      class Request::Impl:
        public elle::StreamBuffer,
        public elle::Printable
      {
      public:
        Impl(Request& request,
             std::string const& url,
             Method method,
             Configuration conf);
        ~Impl();
        void
        start();

        ELLE_ATTRIBUTE(Request*, request);
        ELLE_ATTRIBUTE(Configuration, conf);
        ELLE_ATTRIBUTE(elle::generic_unique_ptr<curl_slist>, headers);
        ELLE_ATTRIBUTE_RX(boost::signals2::signal<void (Progress const&)>,
                          progress_changed);

      /*--------.
      | Headers |
      `--------*/
      public:
        void
        header_add(std::string const& header, std::string const& content);
        void
        header_remove(std::string const& header);
        static
        size_t
        header_callback(void *ptr, size_t size, size_t nmemb, void* userdata);
        void
        read_header(elle::ConstWeakBuffer const& data);

      /*-------.
      | Socket |
      `-------*/
      public:
        static
        int
        open_socket(void* data,
                    curlsocktype purpose,
                    struct curl_sockaddr *address);
        static
        int
        close_socket(void*, curl_socket_t);

      /*-------------.
      | StreamBuffer |
      `-------------*/
      protected:
        elle::WeakBuffer
        write_buffer() override;
        void
        flush(StreamBuffer::Size size) override;
        elle::WeakBuffer
        read_buffer() override;
        static
        size_t
        write_callback(char* ptr, size_t chunk, size_t count, void* userdata);
        void
        enqueue_data(elle::Buffer buffer);

      private:
        void
        _complete();
        bool _input_done;
        std::queue<elle::Buffer> _input;
        elle::Buffer _input_current;
        reactor::Barrier _input_available;
        bool _output_done;
        elle::Buffer _output;
        bool _output_available;
        reactor::Signal _output_consumed;
        int _output_offset;

      /*--------.
      | Cookies |
      `--------*/
      public:
        static
        std::unordered_map<std::string, std::string>
        cookies(CURL* handle);
        std::unordered_map<std::string, std::string>
        cookies() const;
        void
        cookie_add(std::string const& name,
                   std::string const& value);

      /*------.
      | Input |
      `------*/
      private:
        static
        size_t
        read_callback(char* ptr, size_t chunk, size_t count, void* userdata);
        size_t
        read_data(elle::WeakBuffer buffer);
        /// XXX: For HTTP 1.0 servers. See comment in Request constructor.
        std::unique_ptr<std::stringstream> _input_buffer;

      /*---------.
      | Progress |
      `---------*/
      private:
        static
        int
        progress_callback(void* userdata,
                          curl_off_t dltotal, curl_off_t dlnow,
                          curl_off_t ultotal, curl_off_t ulnow);
        void
        progress_set(curl_off_t dltotal, curl_off_t dlnow,
                     curl_off_t ultotal, curl_off_t ulnow);
      public:
        ELLE_ATTRIBUTE_R(Progress, progress);

      private:
        friend class Client;
        friend class Request;
        friend class Service;
        friend
        CURL*
        handle_from_request(Request const& r);
        Service& _curl;
        std::string _url;
        Method _method;
        std::string _query_string;
        CURL* _handle;
        char _error[CURL_ERROR_SIZE];
        ELLE_ATTRIBUTE_R(int, pause_count);
      /*----------.
      | Printable |
      `----------*/
      public:
        /// Pretty print the request.
        virtual
        void
        print(std::ostream& stream) const override;


      /*----------.
      | Debugging |
      `----------*/
      private:
        int _debug;
        int _debug2;
        elle::Backtrace _bt_frozen;
        elle::Backtrace _bt_unfrozen;
        std::string _bt_unfrozen_reason;
        elle::Backtrace _bt_waited;
        boost::signals2::connection _slot_frozen;
        boost::signals2::connection _slot_unfrozen;
      };
    }
  }
}
