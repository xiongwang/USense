#ifndef EMOTION_H
#define EMOTION_H

#include <QtCore>

/*! \class Emotion
 * \brief 表情处理
 *
 *
 */
class Emotion
{
    public:
        Emotion();
        void setPhrase(QString phrase){ this->phrase = phrase; }
        QString getPhrase(){ return this->phrase; }
        void setUrl(QString url){ this->url = url; }
        QString getUrl(){ return this->url; }
        void setHot(bool hot){ this->hot = hot; }
        bool isHot(){ return this->hot; }
        void setCategory(QString category){ this->category = category; }
        QString getCategory(){ return this->category; }
        
    private:
        QString phrase;
        QString url;
        bool hot;
        QString category;
};

#endif // EMOTION_H
