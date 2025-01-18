#pragma once
#include <vector>
#include <boost/asio.hpp>
#include "Singleton.h"

/* io_context池，用于提高并发性能 */
class AsioIOServicePool : public Singleton<AsioIOServicePool>
{
public:
    using IOService = boost::asio::io_context;
    using work = boost::asio::io_context::work;
    using WorkPtr = std::unique_ptr<work>;

    ~AsioIOServicePool();
    AsioIOServicePool(const AsioIOServicePool &) = delete;
    AsioIOServicePool &operator=(const AsioIOServicePool &) = delete;
    
    boost::asio::io_context &GetIOService();
    void Stop();

private:
    friend class Singleton<AsioIOServicePool>;
    AsioIOServicePool(std::size_t size = 2 /* std::thread::hardware_concurrency() */);
    std::vector<IOService> m_ioServices;
    std::vector<WorkPtr> m_WorkPtrs;
    std::vector<std::thread> m_threads;
    std::size_t m_nextIOService;
};