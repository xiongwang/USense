#ifndef COMMENT_H
#define COMMENT_H

#include <QtCore>
#include "status.h"

/*! \class Comment
 * \brief 评论
 *
 *
 */
class Comment
{
public:
    Comment();
    QString getText()const{return this->text;}
    
private:
    QString id;
    QString text;
    bool favorited;
    QDateTime createAt;
    QString userId;
    QString userName;
    QString userProfileImageUrl;
    Status commentStatus;
    Status commentStatusRetw;
};

#endif // COMMENT_H
