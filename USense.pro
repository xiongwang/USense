#-------------------------------------------------
#
# Project created by QtCreator 2011-07-16T18:39:04
#
#-------------------------------------------------

QT       += core gui webkit network xml
CONFIG += qaxcontainer
TARGET = USense
TEMPLATE = app
target.path=/usr/local/bin

desktop.files=USense.desktop
desktop.path=/usr/share/applications/

icon.files=USense.png
icon.path=/usr/share/icons/

font.files= msyh.ttf
font.path= /usr/share/fonts/zh_CN/TrueType/

INSTALLS += target  desktop icon font


SOURCES +=\
    src/sina/oauth.cpp \
    src/sina/http.cpp \
    src/sina/emotion.cpp \
    src/sina/dealwebkitevent.cpp \
    src/sina/showpicture.cpp \
    src/sina/status.cpp \
    src/sina/sinaweibo.cpp \
    src/sina/sinaservices.cpp \
    src/sina/sinaparserxml.cpp \
    src/sina/comment.cpp \
    src/sina/account.cpp \
    src/sina/sinawidget.cpp \
    src/mainwidget.cpp \
    src/main.cpp \
    src/rss/rssobject.cpp \
    src/rss/rssreader.cpp \
    src/loginwidget.cpp \
    src/sina/sinalogin.cpp \
    src/sina/configuration.cpp \
    src/sina/statuswidget.cpp \
    src/renren/utility.cpp \
    src/renren/userapi.cpp \
    src/renren/statusapi.cpp \
    src/renren/renrenwidget.cpp \
    src/renren/renrenaccount.cpp \
    src/renren/oauth2.cpp \
    src/renren/method.cpp \
    src/renren/httpget.cpp \
    src/renren/getfriendsapi.cpp \
    src/renren/feedapi.cpp \
    src/renren/emotionapi.cpp \
    src/renren/dealrenrenevent.cpp \
    src/renren/config.cpp \
    src/renren/renrenstatuswidget.cpp \
    src/renren/photoapi.cpp \
    src/renren/blogapi.cpp \
    src/renren/shareapi.cpp \
    src/clientwidget.cpp \
    src/appwidget.cpp \
    src/app/applicationwidget.cpp \
    src/mynetworkcookiejar.cpp \
    src/mywebview.cpp



HEADERS  += \
    src/sina/http.h \
    src/sina/emotion.h \
    src/sina/dealwebkitevent.h \
    src/sina/comment.h \
    src/sina/account.h \
    src/sina/oauth.h \
    src/sina/status.h \
    src/sina/sinaweibo.h \
    src/sina/sinaservices.h \
    src/sina/sinaparserxml.h \
    src/sina/showpicture.h \
    src/sina/SinaGlobalNamespace.h \
    src/sina/sinawidget.h \
    src/mainwidget.h \
    src/rss/rssobject.h \
    src/rss/rssreader.h \
    src/loginwidget.h \
    src/sina/sinalogin.h \
    src/sina/configuration.h \
    src/sina/statuswidget.h \
    src/renren/utility.h \
    src/renren/userapi.h \
    src/renren/statusapi.h \
    src/renren/renrenwidget.h \
    src/renren/renrenaccount.h \
    src/renren/oauth2.h \
    src/renren/method.h \
    src/renren/httpget.h \
    src/renren/getfriendsapi.h \
    src/renren/feedapi.h \
    src/renren/emotionapi.h \
    src/renren/dealrenrenevent.h \
    src/renren/config.h \
    src/renren/renrenstatuswidget.h \
    src/renren/photoapi.h \
    src/renren/blogapi.h \
    src/renren/shareapi.h \
    src/clientwidget.h \
    src/appwidget.h \
    src/app/applicationwidget.h \
    src/mynetworkcookiejar.h \
    src/mywebview.h


FORMS    += \
    src/mainwidget.ui \
    src/sina/sinawidget.ui \
    src/rss/rssreader.ui \
    src/loginwidget.ui \
    src/sina/statuswidget.ui \
    src/renren/renrenwidget.ui \
    src/renren/renrenwidget.ui \
    src/renren/renrenstatuswidget.ui \
    src/clientwidget.ui \
    src/appwidget.ui \
    src/app/applicationwidget.ui

RESOURCES += \
    Resource.qrc
