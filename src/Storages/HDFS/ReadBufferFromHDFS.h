#pragma once

#include <Common/config.h>

#if USE_HDFS
#include <IO/ReadBuffer.h>
#include <IO/BufferWithOwnMemory.h>
#include <IO/AsynchronousReader.h>
#include <string>
#include <memory>
#include <hdfs/hdfs.h>
#include <base/types.h>
#include <Interpreters/Context.h>
#include <IO/SeekableReadBuffer.h>


namespace DB
{

/** Accepts HDFS path to file and opens it.
 * Closes file by himself (thus "owns" a file descriptor).
 */
class ReadBufferFromHDFS : public SeekableReadBufferWithSize
{
struct ReadBufferFromHDFSImpl;

public:
    using ReadResult = IAsynchronousReader::Result;

    ReadBufferFromHDFS(const String & hdfs_uri_, const String & hdfs_file_path_,
                       const Poco::Util::AbstractConfiguration & config_,
                       size_t buf_size_ = DBMS_DEFAULT_BUFFER_SIZE,
                       size_t read_until_position_ = 0);

    ~ReadBufferFromHDFS() override;

    bool nextImpl() override;

    off_t seek(off_t offset_, int whence) override;

    off_t getPosition() override;

    std::optional<size_t> getTotalSize() override;

    size_t getFileOffsetOfBufferEnd() const override;

    ReadResult readInto(char * data, size_t size, size_t offset, size_t ignore = 0);

private:
    class StatusGuard
    {
    public:
        StatusGuard(ReadBufferFromHDFS * buf_, const String & action_)
            : buf(buf_)
            , action(action_)
        {
            std::cout << "before " << buf->getStatus(action) << std::endl;
        }

        ~StatusGuard()
        {
            std::cout << "after " << buf->getStatus(action) << std::endl;
        }
    private:
        ReadBufferFromHDFS * buf;
        String action;
    };

    std::unique_ptr<ReadBufferFromHDFSImpl> impl;

    String getId() const { return std::to_string(reinterpret_cast<std::uintptr_t>(this)); }
    String getStatus(const String & action);
    std::mutex mutex;
};
}

#endif
