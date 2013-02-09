#ifndef METHOD_H
#define METHOD_H

#include <qstring.h>

class Method
{
public:
    static const QString FEED_GET;
    static const QString FEED_PUBLISHFEED;
    static const QString FRIENDS_GET;
    static const QString FRIENDS_GETFRIENDS;
    static const QString FRIENDS_SEARCH;
    static const QString FRIENDS_GETSAMEFRIENDS;
    static const QString USERS_GETINFO;
    static const QString USERS_GETLOGGEDINUSER;
    static const QString USERS_HASAPPPERMISSION;
    static const QString USERS_ISAPPUSER;
    static const QString USERS_GETPROFILEINFO;
    static const QString LIKE_ISLIKED;
    static const QString LIKE_GETCOUNT;
    static const QString LIKE_LIKE;
    static const QString LIKE_UNLIKE;
    static const QString STATUS_GETS;
    static const QString STATUS_SET;
    static const QString STATUS_GET;
    static const QString STATUS_GETCOMMENT;
    static const QString STATUS_ADDCOMMENT;
    static const QString BLOG_ADDBLOG;
    static const QString BLOG_GETS;
    static const QString BLOG_GET;
    static const QString BLOG_GETCOMMENTS;
    static const QString BLOG_ADDCOMMENT;
    static const QString PHOTOS_CREATALBUM;
    static const QString PHOTOS_GETALBUMS;
    static const QString PHOTOS_UPLOAD;
    static const QString PHOTOS_GET;
    static const QString PHOTOS_ADDCOMMENT;
    static const QString PHOTOS_GETCOMMENTS;
    static const QString PHOTOS_TAG;
    static const QString PHOTOS_GETTAGS;
    static const QString PHOTOS_ACCEPTTAG;
    static const QString PHOTOS_REFUSETAG;
    static const QString SHARE_SHARE;
};

#endif // METHOD_H
