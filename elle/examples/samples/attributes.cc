#include <iostream>
#include <string>

#include <elle/assert.hh>
#include <elle/attribute.hh>

struct Object
{
  Object()
    : _value("value")
    , _stuff(3)
    , _done(false)
  {
  }

  ELLE_ATTRIBUTE(std::string, value);
  ELLE_ATTRIBUTE_R(int, stuff);
  ELLE_ATTRIBUTE_RW(bool, done);

  // What has been generated:
  // private:
  //   std::string _value;
  //
  // private:
  //   int _stuff;

  // int
  // stuff() const
  // {
  //   return this->_stuff;
  // }
  //
};

int
main()
{
  Object object;
  std::cout << "object has an attribute _stuff ("
            << object.stuff()
            << ") accessible via object.stuff()" << std::endl;
  ELLE_ASSERT_EQ(object.stuff(), 3);
  std::cout << "object has an attribute _done ("
            << (object.done() ? "true" : "false")
            << ") accessible via object.done()" << std::endl;
  ELLE_ASSERT_EQ(object.done(), false);
  std::cout << "object's attribute _done can be set via object.done(true)"
            << std::endl;
  object.done(true);
  std::cout << "object attribute _done value is now "
            << (object.done() ? "true" : "false") << std::endl;
  ELLE_ASSERT_EQ(object.done(), true);
  return 0;
}
