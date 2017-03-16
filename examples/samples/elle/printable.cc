#include <elle/Printable.hh>
#include <elle/assert.hh>
#include <elle/attribute.hh>
#include <elle/printf.hh>

struct HitCounter
  : public elle::Printable
{
  HitCounter()
    : _hit(0)
  {
  }

  void
  print(std::ostream& out) const override
  {
    out << "This counter that has ";
    if (this->_hit == 0)
      out << "never been hit";
    else if (this->_hit > 100)
      out << "been hit tones of times";
    else
      elle::fprintf(out, "been hit %s times", this->_hit);
  }
  ELLE_ATTRIBUTE_RW(int, hit);
};

int
main()
{
  HitCounter counter;
  ELLE_ASSERT_EQ(elle::sprintf("%s", counter),
                 "This counter that has never been hit");
  counter.hit(42);
  ELLE_ASSERT_EQ(elle::sprintf("%s", counter),
                 "This counter that has been hit 42 times");
  counter.hit(1002);
  ELLE_ASSERT_EQ(elle::sprintf("%s", counter),
                 "This counter that has been hit tones of times");
  return 0;
}
