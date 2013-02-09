

#include "status.h"

Status::Status()
{
}

void Status::updatePlainCreatedTime()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    uint currentSecs = currentDateTime.toTime_t();
    uint createdSecs = createdAt.toTime_t();

    uint interval = currentSecs - createdSecs;
    if (interval < 60 && interval >= 0)
    {
        plainCreatedTime = "来自 " + QString::number(interval).toLocal8Bit() + " 秒前";
    }
    else if (interval < 60*60)
    {
        plainCreatedTime = "来自 " + QString::number(interval/60).toLocal8Bit() + " 分前";
    }
    else if (interval < 24*60*60)
    {
        plainCreatedTime = "来自 " + QString::number(interval/(60*60)).toLocal8Bit() + " 小时前";
    }
    else if (interval < 10*24*60*60)
    {
        plainCreatedTime = "来自 " + QString::number(interval/(24*60*60)).toLocal8Bit() + " 天前";
    }
    else
    {
        plainCreatedTime = createdAt.toString("M-d hh:mm");
    }


}
/*
void Status::updatePlainCreatedTime(QList<Status>& statusList)
{
    Status status;
    for (int i = 0; i != statusList.length(); ++i)
    {
        status = statusList.at(i);
        status.updatePlainCreatedTime();
    }
}
*/
bool Status::operator ==(const Status& aStatus)const
{
    return aStatus.getId() == this->getId();

}
