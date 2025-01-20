#pragma once
#include <iostream>
#include <string>
#include "Const.h"

class LogicSystem;
class MsgNode
{
public:
    MsgNode(short max_len);
    ~MsgNode();

    void Clear();

    short m_cur_len;
    short m_total_len;
    char *m_data;
};

class RecvNode : public MsgNode
{
public:
    RecvNode(short max_len, short msg_id);

private:
    friend class LogicSystem;
    short m_msg_id;
};

class SendNode : public MsgNode
{
public:
    SendNode(const char *msg, short max_len, short msg_id);

private:
    friend class LogicSystem;
    short m_msg_id;
};