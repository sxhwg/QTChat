#pragma once
#include <grpcpp/grpcpp.h>
#include <mutex>
#include "message.grpc.pb.h"
#include "message.pb.h"
#include "Data.h"

using grpc::ServerContext;
using grpc::Status;

using message::AddFriendReq;
using message::AddFriendRsp;
using message::AuthFriendReq;
using message::AuthFriendRsp;
using message::ChatService;
using message::TextChatData;
using message::TextChatMsgReq;
using message::TextChatMsgRsp;

class ChatServiceImpl final : public ChatService::Service
{
public:
	ChatServiceImpl();
	~ChatServiceImpl();

	Status NotifyAddFriend(ServerContext *context, const AddFriendReq *request, AddFriendRsp *reply) override;
	Status NotifyAuthFriend(ServerContext *context, const AuthFriendReq *request, AuthFriendRsp *response) override;
	Status NotifyTextChatMsg(ServerContext *context, const TextChatMsgReq *request, TextChatMsgRsp *response) override;

	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo> &userinfo);
};