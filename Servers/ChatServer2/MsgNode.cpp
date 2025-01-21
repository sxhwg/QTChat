#include "MsgNode.h"
#include <cstring>
#include <boost/asio.hpp>
#include "Const.h"

MsgNode::MsgNode(short max_len) : m_total_len(max_len), m_cur_len(0)
{
    m_data = new char[m_total_len + 1]();
    m_data[m_total_len] = '\0';
}

MsgNode::~MsgNode()
{
    std::cout << "destruct MsgNode" << std::endl;
    delete[] m_data;
}

void MsgNode::Clear()
{
    memset(m_data, 0, m_total_len);
    m_cur_len = 0;
}

RecvNode::RecvNode(short max_len, short msg_id) : MsgNode(max_len), m_msg_id(msg_id)
{
}

SendNode::SendNode(const char *msg, short max_len, short msg_id) : MsgNode(max_len + HEAD_TOTAL_LEN), m_msg_id(msg_id)
{
    //将消息头中的数据转为网络字节序
    short msg_id_host = boost::asio::detail::socket_ops::host_to_network_short(msg_id);
    memcpy(m_data, &msg_id_host, HEAD_ID_LEN);
    short max_len_host = boost::asio::detail::socket_ops::host_to_network_short(max_len);
    memcpy(m_data + HEAD_ID_LEN, &max_len_host, HEAD_DATA_LEN);

    memcpy(m_data + HEAD_ID_LEN + HEAD_DATA_LEN, msg, max_len);
}
