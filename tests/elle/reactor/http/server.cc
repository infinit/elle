#include <elle/reactor/http/Server.hh>

#include <elle/test.hh>

class Server
  : public elle::reactor::http::Server
{
public:
  using Super = elle::reactor::http::Server;
  using Super::Super;

  void
  _handle(Request r) override
  {
    std::string body(std::istreambuf_iterator<char>(r), {});
    std::cerr << "BODY: " << body << std::endl;
    auto resp = r.respond(elle::reactor::http::StatusCode::OK);
    resp << body;
  }
};

ELLE_TEST_SCHEDULED(simple)
{
  Server s(8080);
  s.run();
}


ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(simple));
}
