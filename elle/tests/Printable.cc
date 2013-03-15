#define BOOST_TEST_DYN_LINK
#include <iostream>
#include <sstream>

#include <elle/Printable.hh>

#include <boost/test/unit_test.hpp>


namespace elle
{
  class ComplexNumber : public Printable
  {
  public:

    ComplexNumber(int real, int imag);

    ~ComplexNumber();

    void
    set(int real, int imag);

    void
    print(std::ostream& stream) const;

  private:

    int _real, _imag;

  };

  ComplexNumber::ComplexNumber(int real, int imag)
  {
    _real = real;
    _imag = imag;
  }

  ComplexNumber::~ComplexNumber()
  {
  }

  void
  ComplexNumber::set(int real, int imag)
  {
    _real = real;
    _imag = imag;
  }

  void
  ComplexNumber::print(std::ostream& stream) const
  {
    if (_imag >= 0)
     stream << _real << " + " << _imag << "j";
    if (_imag < 0)
     stream << _real << " - " << abs(_imag) << "j";
  }

}

static
void
test_generic(int x, int y)
{
	elle::ComplexNumber testPositive(x, y);
	std::stringstream output, expected;
	if (y >= 0)
		expected << x << " + " << y << "j";
	if (y < 0)
		expected << x << " - " << abs(y) << "j";
	testPositive.print(output);
	BOOST_CHECK_EQUAL(output.str(), expected.str());
}

static
bool
test_suite()
{
	boost::unit_test::test_suite* basics = BOOST_TEST_SUITE("Basics");
	boost::unit_test::framework::master_test_suite().add(basics);
	basics->add(BOOST_TEST_CASE(std::bind(test_generic, 1, 2)));
	basics->add(BOOST_TEST_CASE(std::bind(test_generic, 3, -5)));

	return true;
}

int
main(int argc, char** argv)
{
	return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}