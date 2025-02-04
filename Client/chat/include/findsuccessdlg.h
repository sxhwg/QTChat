#ifndef FINDSUCCESSDLG_H
#define FINDSUCCESSDLG_H

#include <QDialog>
#include "userdata.h"

namespace Ui {
class FindSuccessDlg;
}

class FindSuccessDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindSuccessDlg(QWidget *parent = nullptr);
    ~FindSuccessDlg();
    void SetSearchInfo(std::shared_ptr<SearchInfo> si);
private slots:
    void on_add_friend_btn_clicked();

private:
    Ui::FindSuccessDlg *ui;
    QWidget *m_parent;
    std::shared_ptr<SearchInfo> m_si;
};

#endif // FINDSUCCESSDLG_H
