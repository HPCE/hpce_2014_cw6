#ifndef  puzzler_core_streams_file_hpp
#define  puzzler_core_streams_file_hpp

#include "puzzler/core/stream.hpp"

namespace puzzler{

  class FileInStream
    : public Stream
  {
  private:
    // No implementation for either
    FileInStream(const FileInStream &); // = delete;
    FileInStream &operator=(const FileInStream &); // = delete;

    uint64_t m_offset;
    
    int m_fd;
  public:
    FileInStream(std::string path)
      : m_offset(0)
      , m_fd(-1)
    {
      m_fd=open(path.c_str(), O_RDONLY);
      if(m_fd==-1)
        throw std::runtime_error("FileStream - Couldn't open file '"+path+"'");
    }
    
    ~FileInStream()
    {
      if(m_fd!=-1){
        close(m_fd);
        m_fd=-1;
      }
    }

    virtual void Send(size_t , const void *)
    {
      throw std::runtime_error("FileInStream::Send - no such operation.");
    }

    virtual void Recv(size_t cbData, void *pData)
    {
      int got=read(m_fd, pData, cbData);
      if(got!=(int)cbData)
        throw std::runtime_error("FileInStream::Recv - Not all data was recieved.");
      m_offset+=got;
    }

    //! Return the current offset from some arbitrary starting point
    virtual uint64_t SendOffset() const
    { return 0; }

    virtual uint64_t RecvOffset() const
    { return m_offset; }
  };

}; // puzzler

#endif
