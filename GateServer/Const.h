#pragma once
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <json/value.h>
#include <json/reader.h>
#include <functional>

#define CODEPREFIX  "code_"

namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;
namespace beast = boost::beast;
namespace http = boost::beast::http;

enum ErrorCodes
{
	Success = 0,
	Error_Json = 1001,	   // Json解析错误
	RPCFailed = 1002,	   // RPC请求错误
	VerifyExpired = 1003,  // 验证码过期
	VerifyCodeErr = 1004,  // 验证码错误
	UserExist = 1005,	   // 用户已经存在
	PasswdErr = 1006,	   // 密码错误
	EmailNotMatch = 1007,  // 邮箱不匹配
	PasswdUpFailed = 1008, // 更新密码失败
	PasswdInvalid = 1009,  // 密码更新失败
	TokenInvalid = 1010,   // Token失效
	UidInvalid = 1011,	   // uid无效
};

// Defer类
class Defer
{
public:
	// 接受一个lambda表达式或者函数指针
	Defer(std::function<void()> func) : m_func(func) {}
	// 析构函数中执行传入的函数
	~Defer()
	{
		m_func();
	}

private:
	std::function<void()> m_func;
};