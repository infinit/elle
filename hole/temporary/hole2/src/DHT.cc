//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sun Jan 25 03:57:27 2009 texane
// Last update Sun Jan 25 11:08:25 2009 texane
//



#include <cstdlib>
#include <cstring>
#include <vector>
#include "DHT.hh"



using std::vector;



// DHT interface implementation

DHT::Status DHT::get(const DHT::Key&, DHT::Value&)
{
  return STATUS_FAILURE;
}


DHT::Status DHT::put(const Key&, const Value&)
{
  return STATUS_FAILURE;
}


void DHT::get(vector<DHT::Request>& request)
{
}


void DHT::put(vector<DHT::Request>&)
{
}
