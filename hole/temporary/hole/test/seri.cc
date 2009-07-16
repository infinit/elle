#include <iostream>
#include <sstream>

// include headers that implement a archive in simple text format
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

/////////////////////////////////////////////////////////////
// gps coordinate
//
// illustrates serialization for a simple type
//
class gps_position
{
private:
  friend class boost::serialization::access;
  // When the class Archive corresponds to an output archive, the
  // & operator is defined similar to <<.  Likewise, when the class Archive
  // is a type of input archive the & operator is defined similar to >>.
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & degrees;
    ar & minutes;
    ar & seconds;
  }
public:
  gps_position(){};
  gps_position(int d, int m, float s) :
    degrees(d), minutes(m), seconds(s)
  {}
  int degrees;
  int minutes;
  float seconds;
};

int main() {

  std::stringstream ss;
  std::string v = "pipi";
  {
    boost::archive::text_oarchive oa(ss);
    oa << v;
  }
  {
    boost::archive::text_iarchive ia(ss);
    ia >> v;
  }

  std::cout << "v: " << v << std::endl;


  return 0;
}

