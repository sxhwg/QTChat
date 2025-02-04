#include "httpmgr.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>

HttpMgr::~HttpMgr() {}

void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    //序列化json数据
    QByteArray data = QJsonDocument(json).toJson();
    //构造网络请求
    QNetworkRequest request(url);
    //设置请求头数据
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, data.length());
    //获取自身的智能指针，复制增加计数，构造伪闭包
    auto self = shared_from_this();
    //异步发送post请求
    QNetworkReply * reply = m_manager.post(request, data);
    //异步处理相应
    connect(reply, &QNetworkReply::finished, [reply, self, req_id, mod](){
        //处理错误
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << "QNetworkReply error:" << reply->errorString();
            emit self->sig_http_finish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater();
            return;
        }

        //读取请求
        QByteArray res = reply->readAll();
        //发出请求完成信号
        emit self->sig_http_finish(req_id, res, ErrorCodes::SUCCESS, mod);
        reply->deleteLater();
        return;
    });
}

HttpMgr::HttpMgr()
{
    //连接请求完成信号和槽函数，利用信号槽机制模拟队列
    connect(this, &HttpMgr::sig_http_finish, this, &HttpMgr::slot_http_finish);
}

void HttpMgr::slot_http_finish(ReqId id, QByteArray res, ErrorCodes err, Modules mod)
{
    if(mod == Modules::REGISTERMOD){
        emit sig_reg_mod_finish(id, res, err);
    }

    if(mod == Modules::RESETMOD){
        emit sig_reset_mod_finish(id, res, err);
    }

    if(mod == Modules::LOGINMOD){
        emit sig_login_mod_finish(id, res, err);
    }
}
