#include <iostream>

#include <elle/Elle.hh>
#include <elle/print.hh>
#include <elle/utility/Buffer.hh>

#include <nucleus/Nucleus.hh>
#include <nucleus/neutron/Record.hh>

#define CHECK(call) if (call != elle::Status::Ok) { show(); assert(false); } else

int main()
{
  CHECK(elle::Elle::Initialize());
  CHECK(nucleus::Nucleus::Initialize());

  elle::utility::Buffer buf;

  buf.Writer() << nucleus::neutron::Record::Null;

  nucleus::neutron::Record record;

  buf.Reader() >> record;

  assert(record == nucleus::neutron::Record::Null);

  std::cout << "tests done.\n";
  return 0;
}


