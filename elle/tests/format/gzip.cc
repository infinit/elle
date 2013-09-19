#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Gzip
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include <elle/format/gzip.hh>

static
bool
my_popen(const std::string& cmd, std::string& out)
{
    const int SIZEBUF = 1234;
    char buf [SIZEBUF];
    FILE* fp;

    if ((fp = popen(cmd.c_str (), "r")) == NULL)
        return false;

    fgets(buf, sizeof (buf), fp);
    out = std::string(buf);

    pclose(fp);
    return true;
}

BOOST_AUTO_TEST_CASE(basic_case)
{
  std::string content("this is CONTENT!!!");
  std::string tmpfile1("/tmp/streamed1");

  {
    std::ofstream tmp1(tmpfile1);
    BOOST_CHECK(tmp1.good());
    elle::format::gzip::Stream filter(tmp1);
    filter << content;
  }

  std::string output;
  my_popen("gzip -cd " + tmpfile1, output);

  BOOST_CHECK_EQUAL(content, output);
}

BOOST_AUTO_TEST_CASE(bigger_content)
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

  std::string tmpfile1("/tmp/streamed2");

  {
    std::ofstream tmp1(tmpfile1);
    BOOST_CHECK(tmp1.good());
    elle::format::gzip::Stream filter(tmp1);
    filter << content;
  }

  std::string output;
  my_popen("gzip -cd " + tmpfile1, output);

  BOOST_CHECK_EQUAL(content, output);
}

BOOST_AUTO_TEST_CASE(empty_content)
{
  std::string content("");
  std::string tmpfile1("/tmp/streamed3");

  {
    std::ofstream tmp1(tmpfile1);
    BOOST_CHECK(tmp1.good());
    elle::format::gzip::Stream filter(tmp1);
    filter << content;
  }

  BOOST_CHECK_EQUAL(boost::filesystem::file_size(tmpfile1), 0);
}

BOOST_AUTO_TEST_CASE(no_content)
{
  std::string tmpfile1("/tmp/streamed4");

  {
    std::ofstream tmp1(tmpfile1);
    BOOST_CHECK(tmp1.good());
    elle::format::gzip::Stream filter(tmp1);
  }

  BOOST_CHECK_EQUAL(boost::filesystem::file_size(tmpfile1), 0);
}
