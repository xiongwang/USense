

#ifndef STATUS_H
#define STATUS_H

#include <QDateTime>

/*! \class Status
 * \brief 微博结构
 *
 *
 */
class Status
{
public:
    Status();
    
    void setCreatedAt(QDateTime createdAt){ this->createdAt = createdAt ;updatePlainCreatedTime();}
    QDateTime getCreatedAt() const { return this->createdAt;}
    void setId(QString id){ this->id = id; }
    QString getId()const{ return this->id;}
    void setText(QString text){this->text = text; }
    QString getText()const{ return this->text; }
    void setFavourited(bool favourited){ this->favourited = favourited;}
    bool isFavourited(){ return this->favourited; }
    void setThumbnailPic(QString thumbnailPic){ this->thumbnailPic=thumbnailPic; }
    QString getThumbnailPic() const{ return this->thumbnailPic; }
    void setBmiddlePic( QString bmiddlePic ){ this->bmiddlePic= bmiddlePic; }
    QString getBmiddlePic() const{ return this->bmiddlePic; }
    void setOriginalPic( QString originalPic ){ this->originalPic = originalPic; }
    QString getOriginalPic() const{ return this->originalPic; }
    void setUserId( QString userId){ this->userId = userId;}
    QString getUserId()const{ return this->userId; }
    void setUserName( QString userName){ this->userName = userName; }
    QString getUserName(){ return this->userName; }
    void setUserProfileImageUrl(QString userProfileImageUrl){ this->userProfileImageUrl = userProfileImageUrl; }
    QString getUserProfileImageUrl(){ return this->userProfileImageUrl; }
    void setRetweetedId(QString retweetedId){ this->retweetedId = retweetedId; }
    QString getRetweetedId(){ return this->retweetedId; }

    void setSenderId(QString senderId){this->senderId = senderId;}
    QString getSenderId(){return this->senderId;}
    void setRecipientId(QString recipientId){this->recipientId = recipientId;}
    QString getRecipientId(){return this->recipientId;}

    void setVedioPic(QString url){vedioPic = url;}
    QString getVedioPic() const{return vedioPic;}

    void setShortVedioUrl(QString url){shortVedioUrl = url;}
    QString getShortVedioUrl() const{return shortVedioUrl;}

    void setLongVedioUrl(QString url){longVedioUrl = url;}
    QString getLongVedioUrl() const{return longVedioUrl;}


    void setCommentsNum(int commentsNum){this->commentsNum = commentsNum;}
    int getCommentsNum(){return commentsNum;}
    void setRetweetedNum(int retweetedNum){this->retweetedNum = retweetedNum;}
    int getRetweetedNum(){return this->retweetedNum;}

    QString getPlainCreatedTime(){updatePlainCreatedTime(); return plainCreatedTime;}
    void updatePlainCreatedTime();
//    static void updatePlainCreatedTime(QList<Status>& statusList);

    bool operator ==(const Status& aStatus)const;

private:
    QDateTime createdAt;//微博创建时间
    QString plainCreatedTime;
    QString id;//微博id
    QString text;//微博内容
    bool   favourited;//是否已收藏
    QString thumbnailPic;//缩略图
    QString bmiddlePic;//中型图片
    QString originalPic;//原始图片
    QString userId;//微博作者id
    QString userName;//微博作者昵称
    QString userProfileImageUrl;//微博作者头像
    QString retweetedId;/*转发的微博id*/
    QString vedioPic;
    QString shortVedioUrl;
    QString longVedioUrl;

    QString recipientId;
    QString senderId;


    int commentsNum;
    int retweetedNum;
};

#endif // STATUS_H
