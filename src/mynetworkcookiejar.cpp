#include "mynetworkcookiejar.h"

MyNetworkCookieJar::MyNetworkCookieJar(QObject *parent) :
    QNetworkCookieJar(parent)
{
    initLists();
}

void MyNetworkCookieJar::setCookies(const QList<QNetworkCookie> &cookieList)
{
    this->setAllCookies(cookieList);
}

QString MyNetworkCookieJar::getImageUrl(const QString &sourceUrl)
{
    this->setCookies(cookieList);
    QString result = getResult(sourceUrl);
    QRegExp regExp("\"(http:.*)\"");
    result.indexOf(regExp);
    QString tmp = regExp.cap(1).replace("\\/","/");
    return tmp;

}

QString MyNetworkCookieJar::getResult(const QString &sourceUrl)
{
    QString requestUrl = "http://t.qq.com/asyn/validvideo.php?url=" + sourceUrl;
    HttpGet http;
    http.setCookieJar(this);
    QString result = http.get(QUrl(requestUrl));
    return result;
}

void MyNetworkCookieJar::initLists()
{
    nameList.append("date");
    valueList.append("26");
    dateTimeList.append("2011-09-27 04:22:40");
    nameList.append("showInfo");
    valueList.append("262855680");
    dateTimeList.append("2011-09-27 04:22:45");
    nameList.append("qq_index_skin_20110928");
    valueList.append("true");
    dateTimeList.append("2011-12-30 14:12:02");
    nameList.append("ad_play_index");
    valueList.append("95");
    dateTimeList.append("2011-10-02 14:12:02");
    nameList.append("mb_reg_from");
    valueList.append("8");
    dateTimeList.append("2011-10-02 14:12:02");
    nameList.append("qm_sid");
    valueList.append("9f1b4df21dab35339820f2e4b335c501,1QHRpeG4zcVJtVQ..");
    dateTimeList.append("2011-10-02 14:12:02");
    nameList.append("qm_username");
    valueList.append("262855680");
    dateTimeList.append("2011-10-02 14:12:02");
    nameList.append("qm_domain");
    valueList.append("http://m220.mail.qq.com");
    dateTimeList.append("2011-10-02 14:12:02");
    nameList.append("qm_qz_key");
    valueList.append("1_3c2746f6dea20b2bb4e6f919177319f5010a090f050c07020206");
    dateTimeList.append("2011-10-02 14:12:02");
    nameList.append("qm_sk");
    valueList.append("262855680&nft1f1Xp");
    dateTimeList.append("2011-10-02 14:12:02");
    nameList.append("qm_ssum");
    valueList.append("262855680&5d2fd48cb60d9320d16116df0201ced2");
    dateTimeList.append("2011-10-02 14:12:02");
    nameList.append("verifysession");
    valueList.append("h0089b63d92f3653b1d6d675a8e7554a279a4017f990079da0f6e8825cd453261c1429f58534a8e5144e982566c9df55262");
    dateTimeList.append("2011-10-02 14:12:02");
    nameList.append("pt2gguin");
    valueList.append("o0262855680");
    dateTimeList.append("2020-01-01 23:59:55");
    nameList.append("uin");
    valueList.append("o0262855680");
    dateTimeList.append("2020-01-01 23:59:55");
    nameList.append("skey");
    valueList.append("@fWJtrYG3r");
    dateTimeList.append("2020-01-01 23:59:55");
    nameList.append("ptisp");
    valueList.append("edu");
    dateTimeList.append("2020-01-01 23:59:55");
    nameList.append("luin");
    valueList.append("o0262855680");
    dateTimeList.append("2011-11-11 06:17:09");
    nameList.append("lskey");
    valueList.append("00010000db7f87d20113fc55356550894deecdefb7f8aa12657ca24ae097fe9ba05087a0be30f29be142e152");
    dateTimeList.append("2011-11-11 06:17:09");
    nameList.append("show_id");
    valueList.append("");
    dateTimeList.append("2011-11-11 06:17:09");
    nameList.append("ptui_loginuin");
    valueList.append("262855680@qq.com");
    dateTimeList.append("2011-11-11 14:17:09");
    nameList.append("pgv_pvid");
    valueList.append("9412661425");
    dateTimeList.append("2038-01-18 08:00:00");
    nameList.append("pgv_info");
    valueList.append("ssid=s9865783966");
    dateTimeList.append("2038-01-18 08:00:00");
    nameList.append("o_cookie");
    valueList.append("262855680");
    dateTimeList.append("2038-01-18 08:00:00");
    nameList.append("wb_regf");
    valueList.append("%3B0%3B%3Bmessage.t.qq.com");
    dateTimeList.append("2011-10-12 15:17:41");

    Q_ASSERT(nameList.size() == valueList.size());
    Q_ASSERT(nameList.size() == dateTimeList.size());

    QNetworkCookie cookie;

    cookie.setDomain(".qq.com");
    cookie.setPath("/");

    QString formatString = "yyyy-MM-dd HH:mm:ss";

    for (int i = 0; i != nameList.size(); ++i)
    {
        cookie.setName(nameList.at(i));
        cookie.setValue(valueList.at(i));
        cookie.setExpirationDate(QDateTime::fromString(dateTimeList.at(i),formatString));
        cookieList.append(cookie);
    }
}
