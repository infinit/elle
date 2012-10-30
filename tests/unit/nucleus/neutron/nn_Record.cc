#include <iostream>

#include <elle/print.hh>
#include <elle/Buffer.hh>
#include <elle/log.hh>

#include <nucleus/neutron/Record.hh>

#define CHECK(call) if (call != elle::Status::Ok) { assert(false); } else

void
test()
{
  elle::Buffer buf;

  buf.writer() << nucleus::neutron::Record::null();

  nucleus::neutron::Record record;

  buf.reader() >> record;

  assert(record == nucleus::neutron::Record::null());
}

int main(int, char** argv)
{
  try
    {
      test();

      std::cout << "tests done.\n";
      return 0;
    }
  catch (std::exception const& e)
    {
      std::cerr << argv[0] << ": fatal error: " << e.what() << std::endl;
      if (reactor::Exception const* re =
          dynamic_cast<reactor::Exception const*>(&e))
        std::cerr << re->backtrace() << std::endl;

      return 1;
    }
}
