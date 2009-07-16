//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Wed Jan 21 16:31:30 2009 texane
// Last update Sat Jan 24 05:01:56 2009 texane
//



#include <string>
#include <cstring>

#include "SystemApi.hh"
#include "LocalStore.hh"
#include "Debug.hh"



using std::string;


// static members definitions

const LocalStore::StoreQuotaT LocalStore::invalidStoreQuota = 0;


// meta helpers, should be tempalted

#define WRITE_META( K ) writeMeta( #K, static_cast<const void*>(&K), sizeof(K) )
#define READ_META( K ) readMeta( #K, static_cast<void*>(&K), sizeof(K) )


// block id affection

static inline
LocalStore::BlockIdT&
copy(LocalStore::BlockIdT& lhs, const LocalStore::BlockIdT& rhs)
{
  ::memcpy(static_cast<void*>(lhs),
	   static_cast<const void*>(rhs),
	   LocalStore::blockIdSize);

  return lhs;
}


// exported

LocalStore::StatusT
LocalStore::createStore(const string& dirPath, const StoreQuotaT& maxQuota)
{
  const SystemApi::ErrorT error =
    SystemApi::Fs::createDirectory(dirPath);

  if (!SystemApi::isSuccess(error))
    return STATUS_FAILURE;

  dirPath_ = dirPath;
  maxQuota_ = maxQuota;

  // create store metas

  if (!SystemApi::isSuccess( WRITE_META(maxQuota_) ))
    goto onError;

  if (!SystemApi::isSuccess( WRITE_META(curQuota_) ))
    goto onError;

  return STATUS_SUCCESS;

 onError:

  SystemApi::Fs::deleteDirectory(dirPath);

  return STATUS_FAILURE;
}


LocalStore::StatusT LocalStore::openStore(const string& dirPath)
{
  if (SystemApi::Fs::isWritableDirectory(dirPath) == false)
    return STATUS_FAILURE;

  dirPath_ = dirPath;

  if (!SystemApi::isSuccess( READ_META(maxQuota_) ))
    return STATUS_FAILURE;

  if (!SystemApi::isSuccess( READ_META(curQuota_) ))
    return STATUS_FAILURE;

  return STATUS_SUCCESS;
}


void LocalStore::closeStore()
{
  WRITE_META(maxQuota_);
  WRITE_META(curQuota_);
}


// iterate over blocks

struct StoreVisitorContext
{
  LocalStore* store_;
  LocalStore::BlockVisitorT visitor_;
  void* params_;

  StoreVisitorContext(LocalStore* store,
		      LocalStore::BlockVisitorT visitor,
		      void* params = NULL)
    : store_(store), visitor_(visitor), params_(params) {}
};


static bool onFile(const string& fileName, void* params)
{
  struct StoreVisitorContext* context =
    static_cast<struct StoreVisitorContext*>(params);

  LocalStore::BlockIdT blockId;

  if (LocalStore::stringToBlockId(fileName, blockId) == false)
    {
      // skip to next
      return true;
    }

  if (context->visitor_(context->store_, blockId, context->params_) == false)
    return false;

  return true;
}


LocalStore::StatusT
LocalStore::foreachBlock(BlockVisitorT onBlock, void* params)
{
  struct StoreVisitorContext context(this, onBlock, params);

  SystemApi::ErrorT error =
    SystemApi::Fs::foreachEntry(dirPath_, onFile, static_cast<void*>(&context));

  if (!SystemApi::isSuccess(error))
    return STATUS_FAILURE;

  return STATUS_SUCCESS;
}


LocalStore::StatusT
LocalStore::createBlock(BlockHandleT& blockHandle, const BlockIdT& blockId)
{
  string fullPath = makeFullPath(toString(blockId));

  const SystemApi::ErrorT error =
    SystemApi::Fs::createFile(blockHandle.fileHandle_, fullPath);

  if (!SystemApi::isSuccess(error))
    return STATUS_FAILURE;

  copy(blockHandle.blockId_, blockId);

  return STATUS_SUCCESS;
}


LocalStore::StatusT
LocalStore::openBlock(BlockHandleT& blockHandle, const BlockIdT& blockId)
{
  string fullPath = makeFullPath(toString(blockId));

  const SystemApi::ErrorT error =
    SystemApi::Fs::openFile(blockHandle.fileHandle_, fullPath);

  if (!SystemApi::isSuccess(error))
    return STATUS_FAILURE;

  copy(blockHandle.blockId_, blockId);

  return STATUS_SUCCESS;
}


void
LocalStore::closeBlock(BlockHandleT& blockHandle)
{
  if (blockHandle.fileHandle_ != SystemApi::Fs::invalidFileHandle)
    {
      SystemApi::Fs::closeFile(blockHandle.fileHandle_);
      blockHandle.fileHandle_ = SystemApi::Fs::invalidFileHandle;
    }
}



LocalStore::StatusT
LocalStore::writeBlock(BlockHandleT& blockHandle,
		       const void* data,
		       const BlockSizeT& size)
{
  StoreQuotaT newQuota = invalidStoreQuota;
  bool writeQuota = false;
  SystemApi::Fs::FileSizeT fileSize;

  SystemApi::ErrorT error;

  // quota

  if (maxQuota_ != invalidStoreQuota)
    {
      writeQuota = true;

      error = SystemApi::Fs::getFileSize(blockHandle.fileHandle_, fileSize);
      if (SystemApi::isSuccess(error))
	{
	  if (fileSize < size)
	    {
	      // dont check for uint64 overflow
	      newQuota = curQuota_ + (size - fileSize);
	    }
	  else
	    {
	      newQuota = curQuota_ - (fileSize - size);

	      // check for underflow

	      if (newQuota > curQuota_)
		newQuota = 0;
	    }
	}

      if (newQuota > maxQuota_)
	return STATUS_FAILURE;
    }

  error = SystemApi::Fs::writeFile(blockHandle.fileHandle_, data, size);
  if (!SystemApi::isSuccess(error))
    return STATUS_FAILURE;

  if (writeQuota)
    {
      curQuota_ = newQuota;
      WRITE_META(curQuota_);
    }

  return STATUS_SUCCESS;
}


LocalStore::StatusT
LocalStore::writeBlock(const BlockIdT& blockId,
		       const void* data,
		       const BlockSizeT& size)
{
  StatusT status;
  BlockHandleT blockHandle;

  status = createBlock(blockHandle, blockId);
  if (!isSuccess(status))
    return status;

  status = writeBlock(blockHandle, data, size);

  closeBlock(blockHandle);

  return status;
}


LocalStore::StatusT
LocalStore::readBlock(BlockHandleT& blockHandle,
		      void* data,
		      const BlockSizeT& size)
{
  // return an success if size bytes read

  SystemApi::ErrorT error =
    SystemApi::Fs::readFile(blockHandle.fileHandle_, data, size);

  return SystemApi::isSuccess(error) ?
    STATUS_SUCCESS : STATUS_FAILURE;
}


LocalStore::StatusT
LocalStore::readBlock(const BlockIdT& blockId,
		      void* data,
		      const BlockSizeT& size)
{
  StatusT status;
  BlockHandleT blockHandle;

  status = openBlock(blockHandle, blockId);
  if (!isSuccess(status))
    return status;

  status = readBlock(blockHandle, data, size);

  closeBlock(blockHandle);

  return status;
}


LocalStore::StatusT
LocalStore::fetchBlock(const BlockIdT& blockId,
		       void*& blockData,
		       BlockSizeT& blockSize)
{
  BlockHandleT blockHandle;
  BlockSizeT size;
  StatusT status;
  void* data;

  blockData = NULL;
  blockSize = 0;

  status = openBlock(blockHandle, blockId);
  if (!isSuccess(status))
    return status;

  status = getBlockSize(blockHandle, size);
  if (!isSuccess(status))
    return STATUS_FAILURE;

  data = new uint8_t[size];

  status = readBlock(blockHandle, data, size);

  closeBlock(blockHandle);

  if (!isSuccess(status))
    {
      delete[] static_cast<uint8_t*>(data);
      return status;
    }

  blockSize = size;
  blockData = data;

  return STATUS_SUCCESS;
}


LocalStore::StatusT
LocalStore::deleteBlock(BlockHandleT& blockHandle)
{
  StoreQuotaT newQuota = invalidStoreQuota;
  bool writeQuota = false;

  if (maxQuota_ != invalidStoreQuota)
    {
      BlockSizeT blockSize;

      writeQuota = true;

      StatusT status = getBlockSize(blockHandle, blockSize);
      if (isSuccess(status))
	{
	  if (blockSize <= curQuota_)
	    newQuota = curQuota_ - blockSize;
	  else
	    newQuota = 0;
	}
    }

  string fullPath = makeFullPath( toString(blockHandle.blockId_) );

  const SystemApi::ErrorT error = SystemApi::Fs::deleteFile(fullPath);
  if (SystemApi::isSuccess(error))
    return STATUS_FAILURE;

  if (writeQuota)
    {
      curQuota_ = newQuota;
      WRITE_META(curQuota_);
    }

  return STATUS_SUCCESS;
}


LocalStore::StatusT
LocalStore::getBlockSize(const BlockHandleT& blockHandle, BlockSizeT& blockSize)
{
  SystemApi::ErrorT error =
    SystemApi::Fs::getFileSize(blockHandle.fileHandle_, blockSize);

  return SystemApi::isSuccess( error ) ?
    STATUS_SUCCESS : STATUS_FAILURE;
}


LocalStore::StatusT
LocalStore::getBlockSize(const BlockIdT& blockId, BlockSizeT& blockSize)
{
  BlockHandleT blockHandle;
  StatusT status;

  status = openBlock(blockHandle, blockId);
  if (!isSuccess(status))
    return status;

  status = getBlockSize(blockHandle, blockSize);

  closeBlock(blockHandle);

  return status;
}


LocalStore*
LocalStore::makeStore(const string& dirPath, const StoreQuotaT& storeQuota)
{
  LocalStore* store = new LocalStore();
  StatusT status;

  if (store == NULL)
    return NULL;

  if (SystemApi::Fs::isWritableDirectory(dirPath))
    status = store->openStore(dirPath);
  else
    status = store->createStore(dirPath, storeQuota);

  if (LocalStore::isSuccess(status))
    return store;

  delete store;

  return NULL;
}


void LocalStore::deleteStore(const string& dirPath)
{
  // delete if exists

  if (SystemApi::Fs::isWritableDirectory(dirPath))
    SystemApi::Fs::deleteDirectory(dirPath);
}


static void toHex(uint8_t n, char* p)
{
  // todo: use serialization tools

  if ((p[0] = (n >> 4) & 0x0f) < 0xa)
    p[0] += '0';
  else
    p[0] = 'a' + p[0] - 0xa;

  if ((p[1] = n & 0x0f) < 0xa)
    p[1] += '0';
  else
    p[1] = 'a' + p[1] - 0xa;
}


string LocalStore::toString(const BlockIdT& blockId)
{
  char stringBuffer[blockIdSize * 2 + 1];

  char* p = stringBuffer;

  for (size_t i = 0; i < blockIdSize; ++i)
    {
      toHex(blockId[i], p);
      p += 2;
    }

  *p = 0;

  return stringBuffer;
}


bool LocalStore::stringToBlockId(const string& s, BlockIdT& blockId)
{
  size_t size = s.size();

  // check filename, could be meta

  if (size != (blockIdSize * 2))
    return false;

  size /= 2;

  bool isValid = true;

  const char* p = s.data();

  for (size_t i = 0; i < size; ++i)
    {
      uint8_t n = 0;

      if (p[0] >= '0' && p[0] <= '9')
	{
	  n |= (p[0] - '0') << 4;
	}
      else if (p[0] >= 'a' && p[0] <= 'f')
	{
	  n |= (0xa + (p[0] - 'a')) << 4;
	}
      else
	{
	  isValid = false;
	  break;
	}

      if (p[1] >= '0' && p[1] <= '9')
	{
	  n |= p[1] - '0';
	}
      else if (p[1] >= 'a' && p[1] <= 'f')
	{
	  n |= 0xa + (p[1] - 'a');
	}
      else
	{
	  isValid = false;
	  break;
	}

      blockId[i] = n;

      p += 2;
    }

  return isValid;
}


string LocalStore::makeFullPath(const string& fileName)
{
  // return the full path of a file inside the store

  return SystemApi::Fs::concatPath(dirPath_, fileName);
}


LocalStore::StatusT
LocalStore::writeMeta(const string& key,
		      const void* value,
		      const SystemApi::Fs::FileSizeT& size)
{
  string fullPath = SystemApi::Fs::concatPath(dirPath_, key);

  SystemApi::Fs::FileHandleT fileHandle;

  SystemApi::ErrorT error =
    SystemApi::Fs::createFile(fileHandle, fullPath, SystemApi::Fs::OMODE_WRONLY);

  if (!SystemApi::isSuccess(error))
    return STATUS_FAILURE;

  error = SystemApi::Fs::writeFile(fileHandle, value, size);

  SystemApi::Fs::closeFile(fileHandle);
  
  return SystemApi::isSuccess(error) ? STATUS_SUCCESS : STATUS_FAILURE;
}


LocalStore::StatusT
LocalStore::readMeta(const string& key,
		     void* value,
		     const SystemApi::Fs::FileSizeT& size)
{
  string fullPath = SystemApi::Fs::concatPath(dirPath_, key);

  SystemApi::Fs::FileHandleT fileHandle;

  SystemApi::ErrorT error =
    SystemApi::Fs::openFile(fileHandle, fullPath, SystemApi::Fs::OMODE_RDONLY);

  if (!SystemApi::isSuccess(error))
    return STATUS_FAILURE;

  error = SystemApi::Fs::readFile(fileHandle, value, size);

  SystemApi::Fs::closeFile(fileHandle);
  
  return SystemApi::isSuccess(error) ? STATUS_SUCCESS : STATUS_FAILURE;
}
