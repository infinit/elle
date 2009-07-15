//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sun Jan 25 03:52:58 2009 texane
// Last update Sun Jan 25 05:10:08 2009 texane
//



#ifndef DHT_HH_INCLUDED
# define DHT_HH_INCLUDED



#include <cstring>
#include <vector>



namespace DHT
{
  enum Status
    {
      STATUS_SUCCESS,
      STATUS_FAILURE
    };

  static const unsigned int keySize = 160 / 8;

  typedef unsigned char Key[keySize];

  class Value
  {
  private:

    unsigned char* data_;
    unsigned int size_;

  public:

    Value() : data_(NULL), size_(0)
    {
    }

    ~Value()
    {
      if (data_ != NULL)
	{
	  delete[] data_;
	  data_ = NULL;
	  size_ = 0;
	}
    }

    void setData(const unsigned char* data, unsigned int size)
    {
      data_ = new unsigned char[size];
      ::memcpy(data_, data, size);
      size_ = size;
    }

    bool operator ==(const Value& rhs) const
    {
      if (!size_ || (size_ != rhs.size_))
	return false;

      return ::memcmp(data_, rhs.data_, size_) ? false : true;
    }

    bool operator !=(const Value& rhs) const
    {
      return !(*this == rhs);
    }

  };

  struct Request
  {
    Key key;
    Value value;
    Status status;

    Request() : status(STATUS_FAILURE) {}
  };

  Status get(const Key&, Value&);
  Status put(const Key&, const Value&);

  void get(std::vector<Request>&);
  void put(std::vector<Request>&);
}



#endif // ! DHT_HH_INCLUDED
