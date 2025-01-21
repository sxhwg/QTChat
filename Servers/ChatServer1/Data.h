#pragma once
#include <string>
struct UserInfo
{
    UserInfo() : name(""), pwd(""), uid(0), email(""), nick(""), desc(""), sex(0), icon(""), back("") {}
    std::string name;
    std::string pwd;
    int uid;
    std::string email;
    std::string nick;
    std::string desc;
    int sex;
    std::string icon;
    std::string back;
};

struct ApplyInfo
{
    ApplyInfo(int uid, std::string name, std::string desc, std::string icon, std::string nick, int sex, int status)
        : m_uid(uid), m_name(name), m_desc(desc), m_icon(icon), m_nick(nick), m_sex(sex), m_status(status) {}

    int m_uid;
    std::string m_name;
    std::string m_desc;
    std::string m_icon;
    std::string m_nick;
    int m_sex;
    int m_status;
};