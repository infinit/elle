//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sun Jan 25 06:13:05 2009 texane
// Last update Sun Jan 25 06:17:58 2009 texane
//



#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "DHT.hh"



using std::vector;



// dht unit testing

#ifdef _WIN32_WINNT

#else

# include <ctime>
# include <unistd.h>
# include <sys/types.h>

static void initRandSeed()
{
  ::srand(::getpid() * ::time(NULL));
}

#endif


static void generateKey(DHT::Key& key)
{
  static bool hasRandSeed = false;

  if (!hasRandSeed)
    initRandSeed();

  for (unsigned int i = 0; i < DHT::keySize; ++i)
    key[i] = (unsigned char)(::rand() % 0xff);
}



// main. generate N random keys which
// is the value of the pair so that a
// key based comparaison can be made.
// return the error count. test async
// interface.

int main(int, char**)
{
  int res = 0;

  static struct DhtInfo
  {

#define DHT_INFO_COUNT 3

    DHT::Key key;
    DHT::Value value;
    DHT::Status status;
  } dhtInfos[DHT_INFO_COUNT];

  // synchronous interface

  for (unsigned int i = 0; i < DHT_INFO_COUNT; ++i)
    {
      struct DhtInfo& info = dhtInfos[i];

      // generate key value pair

      generateKey(info.key);

      info.value.setData(info.key, DHT::keySize);

      // put the pair

      info.status = DHT::put(info.key, info.value);
      if (info.status != DHT::STATUS_SUCCESS)
	{
	  ::printf("[!] DHT::put(%u)\n", i);
	  --res;
	  continue ;
	}

      // retrieve the value

      DHT::Value value;

      info.status = DHT::get(info.key, value);
      if (info.status != DHT::STATUS_SUCCESS)
	{
	  ::printf("[!] DHT::get(%u)\n", i);
	  --res;
	  continue;
	}

      // compare with put value

      if (info.value != value)
	{
	  ::printf("[!] mismatched values\n");
	  --res;
	  continue;
	}

      // success
    }

  // parallelize dht ops

  vector<DHT::Request> requests(DHT_INFO_COUNT);

  for (unsigned int i = 0; i < DHT_INFO_COUNT; ++i)
    {
      struct DhtInfo& info = dhtInfos[i];
      ::memcpy(requests[i].key, info.key, DHT::keySize);
    }

  get(requests);

  vector<DHT::Request>::const_iterator request = requests.begin();

  for (unsigned int i = 0; i < DHT_INFO_COUNT; ++i)  
    {
      struct DhtInfo& info = dhtInfos[i];

      if (request->value != info.value)
	{
	  ::printf("mismatched values\n");
	  --res;
	  continue;
	}

      ++request;
    }

  return res;
}
