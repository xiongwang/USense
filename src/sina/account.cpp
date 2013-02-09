#include "account.h"

Account::Account()
{

}

void Account::StatusManagerAdd(QList<Status> &addToStatusList, QMap<QString, Status> &addToRetwMap, 
                               Status &status, Status &retwStatus, int limit, bool isStart)
{
    QString rtId = retwStatus.getId();
    if(addToStatusList.length()>=limit)
    {
        int removeCount = addToStatusList.length() - limit+1;
        for(int i=0; i<removeCount; i++)
            this->StatusManagerRemove(addToStatusList, addToRetwMap);
    }
    status.setRetweetedId(rtId);
    if(isStart)
    {
        addToStatusList.prepend(status);
    }
    else
    {
        addToStatusList.append(status);
    }
    if(rtId.isEmpty()|| rtId.isNull())
        return;
    addToRetwMap.insert(rtId, retwStatus);
}

void Account::StatusManagerRemove(QList<Status> &statusListRemoveOne, QMap<QString, Status> &RetwMap)
{
    if(!statusListRemoveOne.isEmpty())
    {
        Status status = statusListRemoveOne.last();
        QString id = status.getRetweetedId();
        RetwMap.remove(id);
        statusListRemoveOne.removeLast();
    }
}

void Account::RemoveAll(QList<Status> &statusList, QMap<QString, Status> &retwMap)
{
    statusList.clear();
    retwMap.clear();
}

void Account::RemoveAll(QList<Status> &statusList)
{
    statusList.clear();
}

bool Account::updateStatus(QList<Status> &statusList, QMap<QString, Status> &retwMap, QList<Status> &sourceList, QMap<QString, Status> &sourceMap)
{
    bool bChanged = false;

    if (sourceList.size() == 0)
        return false;

    QListIterator<Status> listIter(sourceList);
    QList<Status> tmpList;
/*
    for (int i = 0; i != sourceList.size(); ++i)
    {
        Status status = sourceList.at(i);
        status.updatePlainCreatedTime();
        bool fExist = statusList.contains(status);

        if (fExist)
            continue;
        else
        {
            statusList.append(status);
            tmpList.append(status);
            bChanged = true;
        }

    }
*/

    while(listIter.hasNext())
    {
        Status status = listIter.next();
        status.updatePlainCreatedTime();
        if (statusList.contains(status))
        {
//            qDebug(status.getText().toLocal8Bit());
            continue;
        }
        else
        {
            statusList.append(status);
            tmpList.append(status);
            bChanged = true;
        }
    }

    sourceList = tmpList;
    QMapIterator<QString,Status> mapIter(sourceMap);
    while(mapIter.hasNext())
    {
       QString statusId = mapIter.next().key();
       Status status = mapIter.value();
       if (!retwMap.contains(statusId))
           retwMap.insert(statusId,status);
    }

    return bChanged;
}
