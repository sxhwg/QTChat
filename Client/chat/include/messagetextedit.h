#ifndef MESSAGETEXTEDIT_H
#define MESSAGETEXTEDIT_H

#include <QTextEdit>
#include <QVector>
#include "global.h"

class MessageTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit MessageTextEdit(QWidget *parent = nullptr);
    ~MessageTextEdit();

    QVector<MsgInfo> getMsgList();
    void insertFileFromUrl(const QStringList &urls);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool canInsertFromMimeData(const QMimeData *source) const override;
    void insertFromMimeData(const QMimeData *source) override;

private slots:
    void textEditChanged();

private:
    void insertImages(const QString &url);
    void insertTextFile(const QString &url);
    bool isImage(QString url);//判断文件是否为图片
    void insertMsgList(QVector<MsgInfo> &list,QString flag, QString text, QPixmap pix);
    QStringList getUrl(QString text);
    QPixmap getFileIconPixmap(const QString &url);//获取文件图标及大小信息，并转化成图片
    QString getFileSize(qint64 size);//获取文件大小

    QVector<MsgInfo> m_MsgList;
    QVector<MsgInfo> m_GetMsgList;

signals:
    void send();
};

#endif // MESSAGETEXTEDIT_H
