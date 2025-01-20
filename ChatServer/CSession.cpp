#include "CSession.h"
#include <iostream>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include "CServer.h"
#include "LogicSystem.h"

CSession::CSession(boost::asio::io_context &io_context, CServer *server)
    : m_socket(io_context), m_server(server), m_close(false), m_head_parse(false), m_user_uid(0)
{
    boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
    m_session_id = boost::uuids::to_string(a_uuid);
    m_recv_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
}
CSession::~CSession()
{
    std::cout << "~CSession destruct" << std::endl;
}

void CSession::Start()
{
    AsyncReadHead(HEAD_TOTAL_LEN);
}

void CSession::Close()
{
    m_socket.close();
    m_close = true;
}

std::shared_ptr<CSession> CSession::SharedSelf()
{
    return shared_from_this();
}

tcp::socket &CSession::GetSocket()
{
    return m_socket;
}

std::string &CSession::GetSessionId()
{
    return m_session_id;
}

void CSession::SetUserId(int uid)
{
    m_user_uid = uid;
}

int CSession::GetUserId()
{
    return m_user_uid;
}

void CSession::Send(char *msg, short max_length, short msgid)
{
    std::lock_guard<std::mutex> lock(m_send_lock);
    int send_que_size = m_send_que.size();
    if (send_que_size > MAX_SENDQUE)
    {
        std::cout << "session: " << m_session_id << " send que fulled, size is " << MAX_SENDQUE << std::endl;
        return;
    }

    m_send_que.push(std::make_shared<SendNode>(msg, max_length, msgid));
    if (send_que_size > 0)
    {
        return;
    }
    auto &msgnode = m_send_que.front();
    boost::asio::async_write(m_socket, asio::buffer(msgnode->m_data, msgnode->m_total_len),
                             std::bind(&CSession::HandleWrite, this, std::placeholders::_1, SharedSelf()));
}

void CSession::Send(std::string msg, short msgid)
{
    std::lock_guard<std::mutex> lock(m_send_lock);
    int send_que_size = m_send_que.size();
    if (send_que_size > MAX_SENDQUE)
    {
        std::cout << "session: " << m_session_id << " send que fulled, size is " << MAX_SENDQUE << std::endl;
        return;
    }

    m_send_que.push(std::make_shared<SendNode>(msg.c_str(), msg.length(), msgid));
    if (send_que_size > 0)
    {
        return;
    }
    auto &msgnode = m_send_que.front();
    boost::asio::async_write(m_socket, boost::asio::buffer(msgnode->m_data, msgnode->m_total_len),
                             std::bind(&CSession::HandleWrite, this, std::placeholders::_1, SharedSelf()));
}

void CSession::AsyncReadHead(int total_len)
{
    auto self = shared_from_this();
    asyncReadFull(HEAD_TOTAL_LEN, [self, this](const boost::system::error_code &ec, std::size_t bytes_transfered)
                  {
		try {
			if (ec) {
				std::cout << "handle read failed, error is " << ec.what() << std::endl;
				Close();
				m_server->ClearSession(m_session_id);
				return;
			}

			if (bytes_transfered < HEAD_TOTAL_LEN) {
				std::cout << "read length not match, read [" << bytes_transfered << "] , total ["
					<< HEAD_TOTAL_LEN << "]" << std::endl;
				Close();
				m_server->ClearSession(m_session_id);
				return;
			}

			m_recv_head_node->Clear();
			memcpy(m_recv_head_node->m_data, m_data, bytes_transfered);

			//获取头部MSGID数据
			short msg_id = 0;
			memcpy(&msg_id, m_recv_head_node->m_data, HEAD_ID_LEN);
			//网络字节序转化为本地字节序
			msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
			std::cout << "msg_id is " << msg_id << std::endl;
			//id非法
			if (msg_id > MAX_LENGTH) {
				std::cout << "invalid msg_id is " << msg_id << std::endl;
				m_server->ClearSession(m_session_id);
				return;
			}
			short msg_len = 0;
			memcpy(&msg_len, m_recv_head_node->m_data + HEAD_ID_LEN, HEAD_DATA_LEN);
			//网络字节序转化为本地字节序
			msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
			std::cout << "msg_len is " << msg_len << std::endl;

			//id非法
			if (msg_len > MAX_LENGTH) {
				std::cout << "invalid data length is " << msg_len << std::endl;
				m_server->ClearSession(m_session_id);
				return;
			}

			m_recv_msg_node = std::make_shared<RecvNode>(msg_len, msg_id);
			AsyncReadBody(msg_len);
		}
		catch (std::exception& e) {
			std::cout << "Exception code is " << e.what() << std::endl;
		} });
}

void CSession::AsyncReadBody(int total_len)
{
    auto self = shared_from_this();
    asyncReadFull(total_len, [self, this, total_len](const boost::system::error_code &ec, std::size_t bytes_transfered)
                  {
		try {
			if (ec) {
				std::cout << "handle read failed, error is " << ec.what() << std::endl;
				Close();
				m_server->ClearSession(m_session_id);
				return;
			}

			if (bytes_transfered < total_len) {
				std::cout << "read length not match, read [" << bytes_transfered << "] , total ["
					<< total_len<<"]" << std::endl;
				Close();
				m_server->ClearSession(m_session_id);
				return;
			}

			memcpy(m_recv_msg_node->m_data , m_data , bytes_transfered);
			m_recv_msg_node->m_cur_len += bytes_transfered;
			m_recv_msg_node->m_data[m_recv_msg_node->m_total_len] = '\0';
			std::cout << "receive data is " << m_recv_msg_node->m_data << std::endl;
			//此处将消息投递到逻辑队列中
			LogicSystem::GetInstance()->PostMsgToQue(std::make_shared<LogicNode>(shared_from_this(), m_recv_msg_node));
			//继续监听头部接受事件
			AsyncReadHead(HEAD_TOTAL_LEN);
		}
		catch (std::exception& e) {
			std::cout << "Exception code is " << e.what() << std::endl;
		} });
}

// 读取完整长度
void CSession::asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code &, std::size_t)> handler)
{
    ::memset(m_data, 0, MAX_LENGTH);
    asyncReadLen(0, maxLength, handler);
}

// 读取指定字节数
void CSession::asyncReadLen(std::size_t read_len, std::size_t total_len,
                            std::function<void(const boost::system::error_code &, std::size_t)> handler)
{
    auto self = shared_from_this();
    m_socket.async_read_some(boost::asio::buffer(m_data + read_len, total_len - read_len),
                             [read_len, total_len, handler, self](const boost::system::error_code &ec, std::size_t bytesTransfered)
                             {
                                 if (ec)
                                 {
                                     // 出现错误，调用回调函数
                                     handler(ec, read_len + bytesTransfered);
                                     return;
                                 }

                                 if (read_len + bytesTransfered >= total_len)
                                 {
                                     // 长度够了就调用回调函数
                                     handler(ec, read_len + bytesTransfered);
                                     return;
                                 }

                                 // 没有错误，且长度不足则继续读取
                                 self->asyncReadLen(read_len + bytesTransfered, total_len, handler);
                             });
}

void CSession::HandleWrite(const boost::system::error_code &error, std::shared_ptr<CSession> shared_self)
{
    // 增加异常处理
    try
    {
        if (!error)
        {
            std::lock_guard<std::mutex> lock(m_send_lock);
            // cout << "send data " << _send_que.front()->_data+HEAD_LENGTH << endl;
            m_send_que.pop();
            if (!m_send_que.empty())
            {
                auto &msgnode = m_send_que.front();
                boost::asio::async_write(m_socket, boost::asio::buffer(msgnode->m_data, msgnode->m_total_len),
                                         std::bind(&CSession::HandleWrite, this, std::placeholders::_1, shared_self));
            }
        }
        else
        {
            std::cout << "handle write failed, error is " << error.what() << std::endl;
            Close();
            m_server->ClearSession(m_session_id);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception code : " << e.what() << std::endl;
    }
}

LogicNode::LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recvnode)
    : m_session(session), m_recvnode(recvnode)
{
}