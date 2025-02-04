#ifndef SEARCHLIST_H
#define SEARCHLIST_H

#include <QListWidget>
#include "userdata.h"
#include "loadingdlg.h"

class SearchList: public QListWidget
{
    Q_OBJECT
public:
    explicit SearchList(QWidget *parent = nullptr);
    void CloseFindDlg();
    void SetSearchEdit(QWidget* edit);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void slot_item_clicked(QListWidgetItem *item);
    void slot_user_search(std::shared_ptr<SearchInfo> si);

private:
    void waitPending(bool pending = true);
    void addTipItem();

    bool m_send_pending;
    std::shared_ptr<QDialog> m_find_dlg;
    QWidget *m_search_edit;
    LoadingDlg *m_loadingDialog;

signals:
    void sig_jump_chat_item(std::shared_ptr<SearchInfo> si);
};

#endif // SEARCHLIST_H
