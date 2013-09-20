#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Gzip
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include <elle/format/gzip.hh>

BOOST_AUTO_TEST_CASE(data)
{
  std::string content(
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam velit"
    "tortor, facilisis eget nisl ac, convallis mattis dui. Donec laoreet purus"
    "at ligula congue, ut gravida felis viverra. Suspendisse potenti. Quisque"
    "elementum lacus ante, ut pellentesque purus tincidunt et. Aliquam"
    "fringilla, augue a consectetur viverra, ante urna pharetra lectus, et"
    "hendrerit purus orci ut lacus. Aliquam erat volutpat. Cras ligula elit,"
    "lobortis euismod pellentesque non, consequat nec sapien. Sed pharetra,"
    "nulla nec hendrerit tempor, odio nisl suscipit sapien, at lacinia dolor"
    "dolor ut lorem. Mauris lacinia ultrices feugiat. Nullam sit amet dolor ut"
    "posuere.");

  std::stringstream buffer;

  {
    elle::format::gzip::Stream filter(buffer);
    filter << content;
  }

  BOOST_CHECK_LT(buffer.str().size(), content.size());

  elle::Buffer output(content.size());
  {
    boost::iostreams::filtering_istream filter;
    filter.push(boost::iostreams::gzip_decompressor());
    filter.push(buffer);
    filter.read(reinterpret_cast<char*>(output.mutable_contents()),
                output.size());
    BOOST_CHECK_EQUAL(filter.gcount(), content.size());
    char c;
    filter.read(&c, 1);
    BOOST_CHECK_EQUAL(filter.gcount(), 0);
    BOOST_CHECK(filter.eof());
  }
  BOOST_CHECK_EQUAL(content, output.string());
}

BOOST_AUTO_TEST_CASE(empty_content)
{
  std::string content;
  std::stringstream buffer;
  {
    elle::format::gzip::Stream filter(buffer);
    filter << content;
  }
  BOOST_CHECK_EQUAL(buffer.str().size(), 0);
}

BOOST_AUTO_TEST_CASE(no_content)
{
  std::string content;
  std::stringstream buffer;
  {
    elle::format::gzip::Stream filter(buffer);
  }
  BOOST_CHECK_EQUAL(buffer.str().size(), 0);
}
