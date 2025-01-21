#include "AsioIOServicePool.h"
#include <iostream>

AsioIOServicePool::AsioIOServicePool(std::size_t size) : m_ioServices(size),
														 m_WorkPtrs(size), m_nextIOService(0)
{
	// 使用work对象初始化io_context，交给智能指针管理
	for (std::size_t i = 0; i < size; ++i)
	{
		m_WorkPtrs[i] = std::unique_ptr<work>(new work(m_ioServices[i]));
	}

	// 在每个线程中启用io_context的run方法
	for (std::size_t i = 0; i < m_ioServices.size(); ++i)
	{
		m_threads.emplace_back([this, i]()
							   { m_ioServices[i].run(); });
	}
}

AsioIOServicePool::~AsioIOServicePool()
{
	Stop();
	std::cout << "AsioIOServicePool destruct" << std::endl;
}

// 使用round-robin的方式返回一个io_context
boost::asio::io_context &AsioIOServicePool::GetIOService()
{
	auto &service = m_ioServices[m_nextIOService++];
	if (m_nextIOService == m_ioServices.size())
	{
		m_nextIOService = 0;
	}
	return service;
}

void AsioIOServicePool::Stop()
{
	for (auto &wp : m_WorkPtrs)
	{
		// 当iocontext已经绑定了读或写的监听事件后，还需要手动stop该服务
		// 因为仅仅执行reset并不能让iocontext从run的状态中退出
		wp->get_io_context().stop();
		wp.reset();
	}

	for (auto &t : m_threads)
	{
		t.join();
	}
}
