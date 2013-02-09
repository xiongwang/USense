

#ifndef SINAGLOBALNAMESPACE_H
#define SINAGLOBALNAMESPACE_H
#include <QString>
#include "status.h"
namespace WithU {

    enum MainWidgetPartNumber{
        HomePageNumber = 0,
        AtMePageNumber,
        CommentPageNumber,
        DirectMessagePageNumber,
        Other
    };
    
    struct NotifyObject{
        WithU::MainWidgetPartNumber type;//消息类型
        Status status;//微博
        Status retwStatus;//转发的微博
    };
}
#endif // SINAGLOBALNAMESPACE_H
