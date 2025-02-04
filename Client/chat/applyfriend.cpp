#include "applyfriend.h"
#include "ui_applyfriend.h"
#include <QScrollBar>
#include <QJsonDocument>
#include "tcpmgr.h"
#include "usermgr.h"

ApplyFriend::ApplyFriend(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApplyFriend),m_label_point(2,6)
{
    ui->setupUi(this);
    // 隐藏对话框标题栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setObjectName("ApplyFriend");
    this->setModal(true);
    ui->name_ed->setPlaceholderText(tr("星火微光"));
    ui->lb_ed->setPlaceholderText("搜索、添加标签");
    ui->back_ed->setPlaceholderText("");

    ui->lb_ed->SetMaxLength(21);
    ui->lb_ed->move(2, 2);
    ui->lb_ed->setFixedHeight(20);
    ui->lb_ed->setMaxLength(10);
    ui->input_tip_wid->hide();

    m_tip_cur_point = QPoint(5, 5);

    m_tip_data = { "同学","家人","菜鸟教程","C++ Primer","Rust 程序设计",
                 "父与子学Python","nodejs开发指南","go 语言开发指南",
                 "游戏伙伴","金融投资","微信读书","拼多多拼友" };

    connect(ui->more_lb, &ClickedOnceLabel::clicked, this, &ApplyFriend::ShowMoreLabel);
    InitTipLbs();
    //链接输入标签回车事件
    connect(ui->lb_ed, &CustomizeEdit::returnPressed, this, &ApplyFriend::SlotLabelEnter);
    connect(ui->lb_ed, &CustomizeEdit::textChanged, this, &ApplyFriend::SlotLabelTextChange);
    connect(ui->lb_ed, &CustomizeEdit::editingFinished, this, &ApplyFriend::SlotLabelEditFinished);
    connect(ui->tip_lb, &ClickedOnceLabel::clicked, this, &ApplyFriend::SlotAddFirendLabelByClickTip);

    ui->scrollArea->horizontalScrollBar()->setHidden(true);
    ui->scrollArea->verticalScrollBar()->setHidden(true);
    ui->scrollArea->installEventFilter(this);
    ui->sure_btn->SetState("normal","hover","press");
    ui->cancel_btn->SetState("normal","hover","press");
    //连接确认和取消按钮的槽函数
    connect(ui->cancel_btn, &QPushButton::clicked, this, &ApplyFriend::SlotApplyCancel);
    connect(ui->sure_btn, &QPushButton::clicked, this, &ApplyFriend::SlotApplySure);
}

ApplyFriend::~ApplyFriend()
{
    delete ui;
}

void ApplyFriend::InitTipLbs()
{
    int lines = 1;
    for(int i = 0; i < m_tip_data.size(); i++){

        auto* lb = new ClickedLabel(ui->lb_list);
        lb->SetState("normal", "hover", "pressed", "selected_normal",
                     "selected_hover", "selected_pressed");
        lb->setObjectName("tipslb");
        lb->setText(m_tip_data[i]);
        connect(lb, &ClickedLabel::clicked, this, &ApplyFriend::SlotChangeFriendLabelByTip);

        QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
        int textWidth = fontMetrics.width(lb->text()); // 获取文本的宽度
        int textHeight = fontMetrics.height(); // 获取文本的高度

        if (m_tip_cur_point.x() + textWidth + tip_offset > ui->lb_list->width()) {
            lines++;
            if (lines > 2) {
                delete lb;
                return;
            }

            m_tip_cur_point.setX(tip_offset);
            m_tip_cur_point.setY(m_tip_cur_point.y() + textHeight + 15);

        }

        auto next_point = m_tip_cur_point;

        AddTipLbs(lb, m_tip_cur_point,next_point, textWidth, textHeight);

        m_tip_cur_point = next_point;
    }
}

void ApplyFriend::AddTipLbs(ClickedLabel *lb, QPoint cur_point, QPoint &next_point, int text_width, int text_height)
{
    lb->move(cur_point);
    lb->show();
    m_add_labels.insert(lb->text(), lb);
    m_add_label_keys.push_back(lb->text());
    next_point.setX(lb->pos().x() + text_width + 15);
    next_point.setY(lb->pos().y());
}

bool ApplyFriend::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->scrollArea && event->type() == QEvent::Enter)
    {
        ui->scrollArea->verticalScrollBar()->setHidden(false);
    }
    else if (obj == ui->scrollArea && event->type() == QEvent::Leave)
    {
        ui->scrollArea->verticalScrollBar()->setHidden(true);
    }
    return QObject::eventFilter(obj, event);
}

void ApplyFriend::SetSearchInfo(std::shared_ptr<SearchInfo> si)
{
    m_si = si;
    auto applyname = UserMgr::GetInstance()->GetName();
    auto bakname = si->m_name;
    ui->name_ed->setText(applyname);
    ui->back_ed->setText(bakname);
}

void ApplyFriend::ShowMoreLabel()
{
    qDebug()<< "receive more label clicked";
    ui->more_lb_wid->hide();

    ui->lb_list->setFixedWidth(325);
    m_tip_cur_point = QPoint(5, 5);
    auto next_point = m_tip_cur_point;
    int textWidth;
    int textHeight;
    //重拍现有的label
    for(auto & added_key : m_add_label_keys){
        auto added_lb = m_add_labels[added_key];

        QFontMetrics fontMetrics(added_lb->font()); // 获取QLabel控件的字体信息
        textWidth = fontMetrics.width(added_lb->text()); // 获取文本的宽度
        textHeight = fontMetrics.height(); // 获取文本的高度

        if(m_tip_cur_point.x() +textWidth + tip_offset > ui->lb_list->width()){
            m_tip_cur_point.setX(tip_offset);
            m_tip_cur_point.setY(m_tip_cur_point.y()+textHeight+15);
        }
        added_lb->move(m_tip_cur_point);

        next_point.setX(added_lb->pos().x() + textWidth + 15);
        next_point.setY(m_tip_cur_point.y());

        m_tip_cur_point = next_point;

    }

    //添加未添加的
    for(int i = 0; i < m_tip_data.size(); i++){
        auto iter = m_add_labels.find(m_tip_data[i]);
        if(iter != m_add_labels.end()){
            continue;
        }

        auto* lb = new ClickedLabel(ui->lb_list);
        lb->SetState("normal", "hover", "pressed", "selected_normal",
                     "selected_hover", "selected_pressed");
        lb->setObjectName("tipslb");
        lb->setText(m_tip_data[i]);
        connect(lb, &ClickedLabel::clicked, this, &ApplyFriend::SlotChangeFriendLabelByTip);

        QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
        int textWidth = fontMetrics.width(lb->text()); // 获取文本的宽度
        int textHeight = fontMetrics.height(); // 获取文本的高度

        if (m_tip_cur_point.x() + textWidth + tip_offset > ui->lb_list->width()) {

            m_tip_cur_point.setX(tip_offset);
            m_tip_cur_point.setY(m_tip_cur_point.y() + textHeight + 15);

        }

        next_point = m_tip_cur_point;

        AddTipLbs(lb, m_tip_cur_point, next_point, textWidth, textHeight);

        m_tip_cur_point = next_point;

    }

    int diff_height = next_point.y() + textHeight + tip_offset - ui->lb_list->height();
    ui->lb_list->setFixedHeight(next_point.y() + textHeight + tip_offset);

    //qDebug()<<"after resize ui->lb_list size is " <<  ui->lb_list->size();
    ui->scrollcontent->setFixedHeight(ui->scrollcontent->height()+diff_height);
}

void ApplyFriend::SlotLabelEnter()
{
    if(ui->lb_ed->text().isEmpty()){
        return;
    }

    auto text = ui->lb_ed->text();

    addLabel(ui->lb_ed->text());

    ui->input_tip_wid->hide();

    auto find_it = std::find(m_tip_data.begin(), m_tip_data.end(), text);
    //找到了就只需设置状态为选中即可
    if (find_it == m_tip_data.end()) {
        m_tip_data.push_back(text);
    }

    //判断标签展示栏是否有该标签
    auto find_add = m_add_labels.find(text);
    if (find_add != m_add_labels.end()) {
        find_add.value()->SetCurState(ClickLbState::Selected);
        return;
    }

    //标签展示栏也增加一个标签, 并设置绿色选中
    auto* lb = new ClickedLabel(ui->lb_list);
    lb->SetState("normal", "hover", "pressed", "selected_normal",
                 "selected_hover", "selected_pressed");
    lb->setObjectName("tipslb");
    lb->setText(text);
    connect(lb, &ClickedLabel::clicked, this, &ApplyFriend::SlotChangeFriendLabelByTip);
    qDebug() << "ui->lb_list->width() is " << ui->lb_list->width();
    qDebug() << "_tip_cur_point.x() is " << m_tip_cur_point.x();

    QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
    int textWidth = fontMetrics.width(lb->text()); // 获取文本的宽度
    int textHeight = fontMetrics.height(); // 获取文本的高度
    qDebug() << "textWidth is " << textWidth;

    if (m_tip_cur_point.x() + textWidth + tip_offset + 3 > ui->lb_list->width()) {

        m_tip_cur_point.setX(5);
        m_tip_cur_point.setY(m_tip_cur_point.y() + textHeight + 15);

    }

    auto next_point = m_tip_cur_point;

    AddTipLbs(lb, m_tip_cur_point, next_point, textWidth, textHeight);
    m_tip_cur_point = next_point;

    int diff_height = next_point.y() + textHeight + tip_offset - ui->lb_list->height();
    ui->lb_list->setFixedHeight(next_point.y() + textHeight + tip_offset);

    lb->SetCurState(ClickLbState::Selected);

    ui->scrollcontent->setFixedHeight(ui->scrollcontent->height() + diff_height);
}

void ApplyFriend::SlotRemoveFriendLabel(QString name)
{
    qDebug() << "receive close signal";

    m_label_point.setX(2);
    m_label_point.setY(6);

    auto find_iter = m_friend_labels.find(name);

    if(find_iter == m_friend_labels.end()){
        return;
    }

    auto find_key = m_friend_label_keys.end();
    for(auto iter = m_friend_label_keys.begin(); iter != m_friend_label_keys.end();
         iter++){
        if(*iter == name){
            find_key = iter;
            break;
        }
    }

    if(find_key != m_friend_label_keys.end()){
        m_friend_label_keys.erase(find_key);
    }


    delete find_iter.value();

    m_friend_labels.erase(find_iter);

    resetLabels();

    auto find_add = m_add_labels.find(name);
    if(find_add == m_add_labels.end()){
        return;
    }

    find_add.value()->ResetNormalState();
}

void ApplyFriend::SlotChangeFriendLabelByTip(QString lbtext, ClickLbState state)
{
    auto find_iter = m_add_labels.find(lbtext);
    if(find_iter == m_add_labels.end()){
        return;
    }

    if(state == ClickLbState::Selected){
        //编写添加逻辑
        addLabel(lbtext);
        return;
    }

    if(state == ClickLbState::Normal){
        //编写删除逻辑
        SlotRemoveFriendLabel(lbtext);
        return;
    }
}

void ApplyFriend::SlotLabelTextChange(const QString& text)
{
    if (text.isEmpty()) {
        ui->tip_lb->setText("");
        ui->input_tip_wid->hide();
        return;
    }

    auto iter = std::find(m_tip_data.begin(), m_tip_data.end(), text);
    if (iter == m_tip_data.end()) {
        auto new_text = add_prefix + text;
        ui->tip_lb->setText(new_text);
        ui->input_tip_wid->show();
        return;
    }
    ui->tip_lb->setText(text);
    ui->input_tip_wid->show();
}

void ApplyFriend::SlotLabelEditFinished()
{
    ui->input_tip_wid->hide();
}

void ApplyFriend::SlotAddFirendLabelByClickTip(QString text)
{
    int index = text.indexOf(add_prefix);
    if (index != -1) {
        text = text.mid(index + add_prefix.length());
    }
    addLabel(text);

    auto find_it = std::find(m_tip_data.begin(), m_tip_data.end(), text);
    //找到了就只需设置状态为选中即可
    if (find_it == m_tip_data.end()) {
        m_tip_data.push_back(text);
    }

    //判断标签展示栏是否有该标签
    auto find_add = m_add_labels.find(text);
    if (find_add != m_add_labels.end()) {
        find_add.value()->SetCurState(ClickLbState::Selected);
        return;
    }

    //标签展示栏也增加一个标签, 并设置绿色选中
    auto* lb = new ClickedLabel(ui->lb_list);
    lb->SetState("normal", "hover", "pressed", "selected_normal",
                 "selected_hover", "selected_pressed");
    lb->setObjectName("tipslb");
    lb->setText(text);
    connect(lb, &ClickedLabel::clicked, this, &ApplyFriend::SlotChangeFriendLabelByTip);
    qDebug() << "ui->lb_list->width() is " << ui->lb_list->width();
    qDebug() << "_tip_cur_point.x() is " << m_tip_cur_point.x();

    QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
    int textWidth = fontMetrics.width(lb->text()); // 获取文本的宽度
    int textHeight = fontMetrics.height(); // 获取文本的高度
    qDebug() << "textWidth is " << textWidth;

    if (m_tip_cur_point.x() + textWidth+ tip_offset+3 > ui->lb_list->width()) {

        m_tip_cur_point.setX(5);
        m_tip_cur_point.setY(m_tip_cur_point.y() + textHeight + 15);

    }

    auto next_point = m_tip_cur_point;

    AddTipLbs(lb, m_tip_cur_point, next_point, textWidth,textHeight);
    m_tip_cur_point = next_point;

    int diff_height = next_point.y() + textHeight + tip_offset - ui->lb_list->height();
    ui->lb_list->setFixedHeight(next_point.y() + textHeight + tip_offset);

    lb->SetCurState(ClickLbState::Selected);

    ui->scrollcontent->setFixedHeight(ui->scrollcontent->height()+ diff_height );
}

void ApplyFriend::SlotApplySure()
{
    qDebug()<<"Slot Apply Sure called" ;
    //发送请求逻辑
    QJsonObject jsonObj;
    auto uid = UserMgr::GetInstance()->GetUid();
    jsonObj["uid"] = uid;
    auto name = ui->name_ed->text();
    if(name.isEmpty()){
        name = ui->name_ed->placeholderText();
    }

    jsonObj["applyname"] = name;

    auto bakname = ui->back_ed->text();
    if(bakname.isEmpty()){
        bakname = ui->back_ed->placeholderText();
    }

    jsonObj["bakname"] = bakname;
    jsonObj["touid"] = m_si->m_uid;

    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    //发送tcp请求给chat server
    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_ADD_FRIEND_REQ, jsonData);
    this->hide();
    deleteLater();
}

void ApplyFriend::SlotApplyCancel()
{
    qDebug() << "Slot Apply Cancel";
    this->hide();
    deleteLater();
}

void ApplyFriend::resetLabels()
{
    auto max_width = ui->gridWidget->width();
    auto label_height = 0;
    for(auto iter = m_friend_labels.begin(); iter != m_friend_labels.end(); iter++){
        if( m_label_point.x() + iter.value()->width() > max_width) {
            m_label_point.setY(m_label_point.y()+iter.value()->height()+6);
            m_label_point.setX(2);
        }

        iter.value()->move(m_label_point);
        iter.value()->show();

        m_label_point.setX(m_label_point.x()+iter.value()->width()+2);
        m_label_point.setY(m_label_point.y());
        label_height = iter.value()->height();
    }

    if(m_friend_labels.isEmpty()){
        ui->lb_ed->move(m_label_point);
        return;
    }

    if(m_label_point.x() + MIN_APPLY_LABEL_ED_LEN > ui->gridWidget->width()){
        ui->lb_ed->move(2,m_label_point.y()+label_height+6);
    }else{
        ui->lb_ed->move(m_label_point);
    }
}

void ApplyFriend::addLabel(QString name)
{
    if (m_friend_labels.find(name) != m_friend_labels.end()) {
        ui->lb_ed->clear();
        return;
    }

    auto tmplabel = new FriendLabel(ui->gridWidget);
    tmplabel->SetText(name);
    tmplabel->setObjectName("FriendLabel");

    auto max_width = ui->gridWidget->width();
    //todo... 添加宽度统计
    if (m_label_point.x() + tmplabel->width() > max_width) {
        m_label_point.setY(m_label_point.y() + tmplabel->height() + 6);
        m_label_point.setX(2);
    }
    else {

    }

    tmplabel->move(m_label_point);
    tmplabel->show();
    m_friend_labels[tmplabel->Text()] = tmplabel;
    m_friend_label_keys.push_back(tmplabel->Text());

    connect(tmplabel, &FriendLabel::sig_close, this, &ApplyFriend::SlotRemoveFriendLabel);

    m_label_point.setX(m_label_point.x() + tmplabel->width() + 2);

    if (m_label_point.x() + MIN_APPLY_LABEL_ED_LEN > ui->gridWidget->width()) {
        ui->lb_ed->move(2, m_label_point.y() + tmplabel->height() + 2);
    }
    else {
        ui->lb_ed->move(m_label_point);
    }

    ui->lb_ed->clear();

    if (ui->gridWidget->height() < m_label_point.y() + tmplabel->height() + 2) {
        ui->gridWidget->setFixedHeight(m_label_point.y() + tmplabel->height() * 2 + 2);
    }
}
