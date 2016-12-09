/// Crash on OS X when using: echo "something" | /path/to/fdstream

#include <elle/test.hh>

#include <reactor/FDStream.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("reactor.FDStream.test");

ELLE_TEST_SCHEDULED(basics)
{
  int fds[2];
  BOOST_CHECK_EQUAL(::pipe(fds), 0);
  static const std::string data("something");
  BOOST_CHECK_EQUAL(::write(fds[1], data.c_str(), data.size()), data.size());
  BOOST_CHECK_EQUAL(::close(fds[1]), 0);
  reactor::FDStream stream(fds[0]);
  ELLE_ASSERT(!stream.eof());
  char content[1024] = {0};
  stream.read(content, sizeof(content));
  ELLE_ASSERT_EQ(static_cast<unsigned long>(stream.gcount()), data.size());
  ELLE_ASSERT_EQ(content, data);
  ELLE_ASSERT(stream.eof());
}

ELLE_TEST_SCHEDULED(destruction_segv)
{
  int fds[2];
  BOOST_CHECK_EQUAL(::pipe(fds), 0);
  auto stream = std::make_unique<reactor::FDStream>(fds[0]);
  reactor::Barrier reading;
  reactor::Thread reader(
    "reader",
    [&]
    {
      ELLE_ASSERT(!stream->eof());
      char content[1024] = {0};
      reading.open();
      stream->read(content, sizeof(content));
    });
  reactor::wait(reading);
  reader.terminate_now();
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(basics), 0, 1);
  suite.add(BOOST_TEST_CASE(destruction_segv), 0, 1);
}
