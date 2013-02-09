#include "sinaservices.h"

SinaServices::SinaServices(QObject *parent) :
    QObject(parent)
{
    this->services.insert("BasePath", "http://api.t.sina.com.cn/");
    this->services.insert("Source", "2545702622");
    this->services.insert("Secret","0f8dd631483332a92cbe19a84e5528b7");
    this->services.insert("FriendsTimeline", this->getServices("BasePath")+"statuses/friends_timeline.xml");/* For HomePage */
    this->services.insert("VerifyCredentials", this->getServices("BasePath")+"account/verify_credentials.xml");
    this->services.insert("Mentions",this->getServices("BasePath")+"statuses/mentions.xml");/* At me */
    this->services.insert("MyTimeline",this->getServices("BasePath")+"statuses/user_timeline.xml"); /* My Timeline */
    this->services.insert("OthersTimeline", this->getServices("BasePath")+"statuses/user_timeline/%1.xml"); /* Get Others Timeline by userid */
    this->services.insert("Favorites",this->getServices("BasePath")+"favorites.xml");
    this->services.insert("CommentsTimeline", this->getServices("BasePath")+"statuses/comments_timeline.xml");
    this->services.insert("Update", this->getServices("BasePath")+"statuses/update.xml");
    this->services.insert("Upload", this->getServices("BasePath")+"statuses/upload.xml");
    this->services.insert("Emotions", this->getServices("BasePath")+"emotions.xml");
    this->services.insert("Repost", this->getServices("BasePath")+"statuses/repost.xml");
    this->services.insert("Comment", this->getServices("BasePath")+"statuses/comment.xml");
    this->services.insert("AddFavorite", this->getServices("BasePath")+"favorites/create.xml");
    this->services.insert("Unread", this->getServices("BasePath")+"statuses/unread.xml");
    this->services.insert("ResetCount", this->getServices("BasePath")+"statuses/reset_count.xml");
    this->services.insert("DirectMessages", this->getServices("BasePath")+"direct_messages.xml");
    this->services.insert("CommentReply", this->getServices("BasePath")+"statuses/reply.xml");
    this->services.insert("DeleteFavorite", this->getServices("BasePath")+"favorites/destroy/%1.xml");
    this->services.insert("DeleteStatus", this->getServices("BasePath")+"statuses/destroy/%1.xml");
    this->services.insert("SendDirectMessage", this->getServices("BasePath")+"direct_messages/new.xml");
    this->services.insert("MyDirectMessages",this->getServices("BasePath")+"direct_messages/sent.xml");
    this->services.insert("Counts", this->getServices("BasePath")+"statuses/counts.json");
    this->services.insert("Comments", this->getServices("BasePath")+"statuses/comments.xml");
    this->services.insert("RequestToken", this->getServices("BasePath")+"oauth/request_token");
    this->services.insert("Authorize", this->getServices("BasePath")+"oauth/authorize");
    this->services.insert("AccessToken", this->getServices("BasePath")+"oauth/access_token");
    this->services.insert("Friends",this->getServices("BasePath")+"statuses/friends/%1.xml");
    this->services.insert("Followers",this->getServices("BasePath")+"statuses/followers/%1.xml");
    this->services.insert("SingleStatus",this->getServices("BasePath")+"statuses/show/%1.xml");
    this->services.insert("DailyTrends",this->getServices("BasePath")+"trends/daily.xml");
    this->services.insert("LongUrl",this->getServices("BasePath")+"short_url/expand.xml");

    this->services.insert("PublicStatuses",this->getServices("BasePath")+"statuses/public_timeline.xml");
    this->services.insert("UserInfo",this->getServices("BasePath")+"users/show.xml");



    this->lastCategoryAt = 0;
}

SinaServices* SinaServices::instance = NULL;
const QString SinaServices::MyAccessToken= "MyAccessToken";
const QString SinaServices::MyAccessTokenSecret = "MyAccessTokenSecret";

SinaServices* SinaServices::getInstance()
{
    if(!instance)
        instance = new SinaServices();
    return instance;
}

QString SinaServices::getServices(QString key)
{
    return this->services.value( key, "" );
}

void SinaServices::setServices(const QString key, const QString value)
{
    this->services.insert(key, value);
}

void SinaServices::addToEmotionCategoryList(QString categoryName)
{
    if(!categoryName.isEmpty())
    {
       if(!isExistInEmotionCategoryList(categoryName)) 
           emotionCategoryList.append(categoryName);
    }
}

bool SinaServices::isExistInEmotionCategoryList(QString categoryName)
{
    if(this->emotionCategoryList.isEmpty())
        return false;
    if(emotionCategoryList.at(this->lastCategoryAt)== categoryName)
        return true;
    for(int i=0; i<emotionCategoryList.length(); i++)
    {
        if(emotionCategoryList.at(i) == categoryName)
        {
            lastCategoryAt = i;
            return true;
        }
    }
    return false;
}
