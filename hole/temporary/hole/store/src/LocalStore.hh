//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Wed Jan 21 16:31:13 2009 texane
// Last update Sat Jan 24 04:48:11 2009 texane
//



#ifndef LOCAL_STORE_HH_INCLUDED
# define LOCAL_STORE_HH_INCLUDED



#include <string>
#include <cstring>
#include "pstdint.h"
#include "SystemApi.hh"



class LocalStore
{
public:

  // block types

  static const size_t blockIdSize = 160 / 8;

  typedef uint8_t BlockIdT[blockIdSize];

  struct BlockHandle
  {
    SystemApi::Fs::FileHandleT fileHandle_;

    BlockIdT blockId_;

    BlockHandle() : fileHandle_(SystemApi::Fs::invalidFileHandle)
    {
      ::memset(blockId_, 0, blockIdSize);
    }

    ~BlockHandle()
    {
      // auto close handle, should be considered
      // a leak until better store interface

      if (fileHandle_ != SystemApi::Fs::invalidFileHandle)
	SystemApi::Fs::closeFile(fileHandle_);

      fileHandle_ = SystemApi::Fs::invalidFileHandle;
    }

  };

  typedef struct BlockHandle BlockHandleT;

  typedef SystemApi::Fs::FileSizeT BlockSizeT;
  typedef SystemApi::Fs::FileOffsetT BlockOffsetT;

  // store quota

  typedef uint64_t StoreQuotaT;

  static const StoreQuotaT invalidStoreQuota;

  // status type

  typedef enum
    {
      // todo
      STATUS_SUCCESS = 0,
      STATUS_FAILURE
    } StatusT;

private:

  // members

  std::string dirPath_;

  StoreQuotaT maxQuota_;
  StoreQuotaT curQuota_;

  // private methods

  std::string makeFullPath(const std::string&);

  StatusT writeMeta(const std::string&, const void*, const SystemApi::Fs::FileSizeT&);
  StatusT readMeta(const std::string&, void*, const SystemApi::Fs::FileSizeT&);

public:

  // ctor

  LocalStore() : maxQuota_(0), curQuota_(0) { }
  ~LocalStore() { closeStore(); }

  // store

  StatusT createStore(const std::string&, const StoreQuotaT& = invalidStoreQuota);
  StatusT openStore(const std::string&);
  void closeStore();

  // visitor

  typedef bool (*BlockVisitorT)(LocalStore*&, const BlockIdT&, void*);

  StatusT foreachBlock(BlockVisitorT, void* = NULL);

  // block

  StatusT createBlock(BlockHandleT&, const BlockIdT&);
  StatusT openBlock(BlockHandleT&, const BlockIdT&);
  void closeBlock(BlockHandleT&);

  StatusT writeBlock(BlockHandleT&, const void*, const BlockSizeT&);
  StatusT writeBlock(const BlockIdT&, const void*, const BlockSizeT&);

  StatusT readBlock(BlockHandleT&, void*, const BlockSizeT&);
  StatusT readBlock(const BlockIdT&, void*, const BlockSizeT&);

  StatusT fetchBlock(const BlockIdT&, void*&, BlockSizeT&);

  StatusT deleteBlock(BlockHandleT&);

  StatusT getBlockSize(const BlockHandleT&, BlockSizeT&);
  StatusT getBlockSize(const BlockIdT&, BlockSizeT&);

  // static methods

  static std::string toString(const BlockIdT&);
  static bool stringToBlockId(const std::string&, BlockIdT&);

  static LocalStore* makeStore(const std::string&, const StoreQuotaT& = invalidStoreQuota);
  static void deleteStore(const std::string&);

  static inline bool isSuccess(const StatusT& status) { return status == STATUS_SUCCESS; }
};



#endif // ! LOCAL_STORE_HH_INCLUDED
