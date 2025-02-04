#include "chatuserlist.h"
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QTimer>
#include "usermgr.h"

ChatUserList::ChatUserList(QWidget *parent):QListWidget(parent), m_load_pending(false)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);
}

bool ChatUserList::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this->viewport()) {
        QEvent::Type eventType = event->type();
        if (eventType == QEvent::Enter) {
            // 鼠标悬浮，显示滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else if (eventType == QEvent::Leave) {
            // 鼠标离开，隐藏滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        } else if (eventType == QEvent::Wheel) {
            // 处理滚轮事件
            QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
            int numDegrees = wheelEvent->angleDelta().y() / 8;
            int numSteps = numDegrees / 15;

            this->verticalScrollBar()->setValue(
                this->verticalScrollBar()->value() - numSteps
                );

            QScrollBar *scrollBar = this->verticalScrollBar();
            int maxScrollValue = scrollBar->maximum();
            int currentValue = scrollBar->value();

            if (maxScrollValue - currentValue <= 20) {
                auto b_loaded = UserMgr::GetInstance()->IsLoadChatFin();
                if (b_loaded) {
                    return true;
                }

                if (m_load_pending) {
                    return true;
                }

                m_load_pending = true;
                QTimer::singleShot(100, this, [this]() {
                    m_load_pending = false;
                });

                emit sig_loading_chat_user();
            }

            return true; // 停止事件传递
        }
    }

    return QListWidget::eventFilter(watched, event);
}
