#include "customizeedit.h"

CustomizeEdit::CustomizeEdit(QWidget *parent):QLineEdit(parent), m_max_len(0)
{
    connect(this, &QLineEdit::textChanged, this, &CustomizeEdit::limitTextLength);
}

void CustomizeEdit::SetMaxLength(int maxLen)
{
    m_max_len = maxLen;
}

void CustomizeEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    emit sig_foucus_out();
}

void CustomizeEdit::limitTextLength(QString text)
{
    if(m_max_len <= 0){
        return;
    }

    QByteArray byteArray = text.toUtf8();

    if (byteArray.size() > m_max_len) {
        byteArray = byteArray.left(m_max_len);
        this->setText(QString::fromUtf8(byteArray));
    }
}
