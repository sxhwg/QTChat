#ifndef CUSTOMIZEEDIT_H
#define CUSTOMIZEEDIT_H

#include <QLineEdit>

class CustomizeEdit: public QLineEdit
{
    Q_OBJECT

public:
    explicit CustomizeEdit(QWidget *parent = nullptr);
    void SetMaxLength(int maxLen);

protected:
    void focusOutEvent(QFocusEvent *event) override;

private:
    void limitTextLength(QString text);

    int m_max_len;

signals:
    void sig_foucus_out();
};

#endif // CUSTOMIZEEDIT_H
