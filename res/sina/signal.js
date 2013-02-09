var page = 1;
var base64EncodeChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
var base64DecodeChars = new Array(
　　-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
　　-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
　　-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
　　52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
　　-1,　0,　1,　2,　3,  4,　5,　6,　7,　8,　9, 10, 11, 12, 13, 14,
　　15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
　　-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
　　41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1);

function getScrollTop()
{
    var scrollTop=0;
    if(document.documentElement&&document.documentElement.scrollTop)
    {
        scrollTop=document.documentElement.scrollTop;
    }
    else if(document.body)
    {
        scrollTop=document.body.scrollTop;
    }
    return scrollTop;
}

function getClientHeight()
{
    var clientHeight=0;
    if(document.body.clientHeight&&document.documentElement.clientHeight)
    {
        var clientHeight = (document.body.clientHeight<document.documentElement.clientHeight)?document.body.clientHeight:document.documentElement.clientHeight;        
    }
    else
    {
        var clientHeight = (document.body.clientHeight>document.documentElement.clientHeight)?document.body.clientHeight:document.documentElement.clientHeight;    
    }
    return clientHeight;
}

function getScrollHeight()
{
    return Math.max(document.body.scrollHeight,document.documentElement.scrollHeight);
}

function getNewPage()
{
		dealWebkitEvent.getNewPageInfo(++page,0);
}

window.onscroll = function()
{
	if (getScrollTop()+getClientHeight()==getScrollHeight())
	{
		getNewPage();
	}
}

function setNew(content)
{
    var data = utf8to16(base64decode(content));
	var newDiv = document.createElement("DIV");
	newDiv.innerHTML = data;
	document.body.appendChild(newDiv);
	/*
    var newPage = document.createElement("DIV")
    newPage.style.position="absolute"
    newPage.style.top=0
    newPage.style.left=0
    newPage.style.background = "#DEDEDE"
    newPage.style.visibility = "visible"
    newPage.innerHTML = data
    document.body.appendChild(newPage)
	*/
}

$(document).ready(function()
		{
			try
			{
				/* This connects sampleQObjects signal to our slot */
				/* sampleQObject.signal.connect(slot);  slot is a javascript function*/
				/* This calls a slot which then in turn emits the signal. */
				/* sampleQObject.slotThatEmitsSignal(); */
				dealWebkitEvent.hideDivById.connect(hideDivByIdJs);
				dealWebkitEvent.parserStatusesCounts.connect(setStatusesContent);
				dealWebkitEvent.setCommentsListHtml.connect(setHtmlById);
				dealWebkitEvent.setDomReady(new Boolean(true));
			}
			catch (e)
			{
				alert(e);
			}
		}
				 );
				 
function setHtmlById(htmlContent,id)
{
	var commentArea = document.getElementsByName(id+"_commentArea");
	for( i=0; i<commentArea.length; i++)
	{
		commentArea[i].innerHTML = htmlContent;
	}
}
				 
function setStatusesContent(jsonContent)
{
	var counts = eval(jsonContent);
	for(var i in counts)
	{
		if(counts[i].rt>0)
		{
			var retw=document.getElementsByName(counts[i].id+"_retw");
			for(j=0; j<retw.length; j++)
				retw[j].innerHTML += ("("+counts[i].rt+")");
		}
		if(counts[i].comments>0)
		{
			var tools=document.getElementsByName(counts[i].id+"_tool");
			for( k=0; k<tools.length; k++)
				tools[k].innerHTML += ("(<a href=javascript:void(0) onclick=showCommentsList(this,"+counts[i].id+",1) title=查看评论>"+counts[i].comments+"</a>)");
		}
	}
}

function showCommentsList(obj,id,page)
{
	try
	{
		var childs = obj.parentElement.parentElement.children;
		for( var child in childs )
		{
			var reValue = childs[child].nodeType;
			if(typeof(reValue)=="undefined")
				continue;
			if(childs[child].getAttribute("name")==(id+"_commentArea"))
			{
				if(childs[child].style.display=="block")
				{
					childs[child].style.display="none";
					
				}
				else
				{
					childs[child].style.display="block";
					dealWebkitEvent.getCommentsList(id,page);
				}
			}
		}
			} catch (e)
	{
		alert(e);
	}
}

function getMoreCommentsList(id,page)
{
	dealWebkitEvent.getCommentsList(id,page);
}

function hideDivByIdJs(id)
{
	document.getElementById(id).style.display="none";
}

function retw(idText, appendText) /* 转发 */
{
	dealWebkitEvent.retwStatusAction(idText, appendText);
}

function comment(id)
{
	dealWebkitEvent.commentStatus(id);
}

function replyComment(cid,id)
{
	dealWebkitEvent.replyComment(cid,id);
}

function openUrl(url)
{
	dealWebkitEvent.openUrl(url);
}

function addEmotion(phase)
{
	dealWebkitEvent.addEmotionPhase(phase);
}

function addFavorite(id)
{
	dealWebkitEvent.addFavorite(id);
}

function getUserTimelineByScreenName(screenName,page)
{
	dealWebkitEvent.getUserTimelineByScreenName(screenName,page);
}

function getUserTimeline(screenName,page)
{
	dealWebkitEvent.getUserTimelineByScreenName(screenName,page);
}

function bigPicture(orgUrl,midUrl)
{
	dealWebkitEvent.bigPicture(orgUrl,midUrl);
}

function getMoreHomePageStatus(page,feature)
{
	dealWebkitEvent.getMoreHomePageStatus(page,feature);
}

function getMoreUserPageStatus(page,feature)
{
	dealWebkitEvent.getMoreUserPageStatus(page,feature);
}

function getMoreAtMePageStatus(page)
{
	dealWebkitEvent.getMoreAtMePageStatus(page);
}

function getMoreCommentsPageStatus(page)
{
	dealWebkitEvent.getMoreCommentsPageStatus(page);
}

function getMoreFavoritePageStatus(page)
{
	dealWebkitEvent.getMoreFavoritePageStatus(page);
}

function getMoreDirectMessagePageStatus(page)
{
	dealWebkitEvent.getMoreDirectMessagePageStatus(page);
}

function deleteFavorite(id)
{
	dealWebkitEvent.deleteFavorite(id);
	hideDivByIdJs(id);
}

function deleteStatus(id)
{
	dealWebkitEvent.deleteMyStatus(id);
}

function sendDirectMessage(id)
{
	dealWebkitEvent.sendDirectMessage(id);
}

function trends(trendsName)
{
	dealWebkitEvent.openTrends(trendsName);
}

function forwardToOtherClient(str)
{
	dealWebkitEvent.forwardToOtherClient(str);
	
}

function getDMSessionStatus(str)
{
	dealWebkitEvent.getSessionStatusSignal(str);
}

function showUserInfo(str)
{
	dealWebkitEvent.showUserInfoSlot(str);
}

function showComments(userid,statusid)
{
	dealWebkitEvent.showCommentsSlot(userid,statusid);
}

function showFriends(str)
{
	dealWebkitEvent.showFriendsSlot(str);
}

function showFollowers(str)
{
	dealWebkitEvent.showFollowersSlot(str);
}

function showPublicStatus()
{
	dealWebkitEvent.showPublicStatusSlot();
}

function showDailyTrends()
{
	dealWebkitEvent.showDailyTrendsSlot();
}

function up(offset)
{
	document.body.scrollTop   =   document.body.scrollTop   +   offset;
}
function down(offset)
{
	document.body.scrollTop   =   document.body.scrollTop   -   offset;
}

function showBigPic(bigUrl,midUrl)
{
	dealWebkitEvent.bigPicture(bigUrl,midUrl);
}

function sendStatusWithAt(username)
{
	dealWebkitEvent.sendStatusWithAtSlot(username);
}

function playVedio(url)
{
	dealWebkitEvent.playVedioSlot(url);
}

function base64encode(str) {
　　var out, i, len;
　　var c1, c2, c3;
　　len = str.length;
　　i = 0;
　　out = "";
　　while(i < len) {
 c1 = str.charCodeAt(i++) & 0xff;
 if(i == len)
 {
　　 out += base64EncodeChars.charAt(c1 >> 2);
　　 out += base64EncodeChars.charAt((c1 & 0x3) << 4);
　　 out += "==";
　　 break;
 }
 c2 = str.charCodeAt(i++);
 if(i == len)
 {
　　 out += base64EncodeChars.charAt(c1 >> 2);
　　 out += base64EncodeChars.charAt(((c1 & 0x3)<< 4) | ((c2 & 0xF0) >> 4));
　　 out += base64EncodeChars.charAt((c2 & 0xF) << 2);
　　 out += "=";
　　 break;
 }
 c3 = str.charCodeAt(i++);
 out += base64EncodeChars.charAt(c1 >> 2);
 out += base64EncodeChars.charAt(((c1 & 0x3)<< 4) | ((c2 & 0xF0) >> 4));
 out += base64EncodeChars.charAt(((c2 & 0xF) << 2) | ((c3 & 0xC0) >>6));
 out += base64EncodeChars.charAt(c3 & 0x3F);
　　}
　　return out;
}
function base64decode(str) {
　　var c1, c2, c3, c4;
　　var i, len, out;
　　len = str.length;
　　i = 0;
　　out = "";
　　while(i < len) {
 /* c1 */
 do {
　　 c1 = base64DecodeChars[str.charCodeAt(i++) & 0xff];
 } while(i < len && c1 == -1);
 if(c1 == -1)
　　 break;
 /* c2 */
 do {
　　 c2 = base64DecodeChars[str.charCodeAt(i++) & 0xff];
 } while(i < len && c2 == -1);
 if(c2 == -1)
　　 break;
 out += String.fromCharCode((c1 << 2) | ((c2 & 0x30) >> 4));
 /* c3 */
 do {
　　 c3 = str.charCodeAt(i++) & 0xff;
　　 if(c3 == 61)
　return out;
　　 c3 = base64DecodeChars[c3];
 } while(i < len && c3 == -1);
 if(c3 == -1)
　　 break;
 out += String.fromCharCode(((c2 & 0XF) << 4) | ((c3 & 0x3C) >> 2));
 /* c4 */
 do {
　　 c4 = str.charCodeAt(i++) & 0xff;
　　 if(c4 == 61)
　return out;
　　 c4 = base64DecodeChars[c4];
 } while(i < len && c4 == -1);
 if(c4 == -1)
　　 break;
 out += String.fromCharCode(((c3 & 0x03) << 6) | c4);
　　}
　　return out;
}
function utf16to8(str) {
　　var out, i, len, c;
　　out = "";
　　len = str.length;
　　for(i = 0; i < len; i++) {
 c = str.charCodeAt(i);
 if ((c >= 0x0001) && (c <= 0x007F)) {
　　 out += str.charAt(i);
 } else if (c > 0x07FF) {
　　 out += String.fromCharCode(0xE0 | ((c >> 12) & 0x0F));
　　 out += String.fromCharCode(0x80 | ((c >>　6) & 0x3F));
　　 out += String.fromCharCode(0x80 | ((c >>　0) & 0x3F));
 } else {
　　 out += String.fromCharCode(0xC0 | ((c >>　6) & 0x1F));
　　 out += String.fromCharCode(0x80 | ((c >>　0) & 0x3F));
 }
　　}
　　return out;
}
function utf8to16(str) {
　　var out, i, len, c;
　　var char2, char3;
　　out = "";
　　len = str.length;
　　i = 0;
　　while(i < len) {
 c = str.charCodeAt(i++);
 switch(c >> 4)
 {
　 case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
　　 // 0xxxxxxx
　　 out += str.charAt(i-1);
　　 break;
　 case 12: case 13:
　　 // 110x xxxx　 10xx xxxx
　　 char2 = str.charCodeAt(i++);
　　 out += String.fromCharCode(((c & 0x1F) << 6) | (char2 & 0x3F));
　　 break;
　 case 14:
　　 // 1110 xxxx　10xx xxxx　10xx xxxx
　　 char2 = str.charCodeAt(i++);
　　 char3 = str.charCodeAt(i++);
　　 out += String.fromCharCode(((c & 0x0F) << 12) |
　　　　((char2 & 0x3F) << 6) |
　　　　((char3 & 0x3F) << 0));
　　 break;
 }
　　}
　　return out;
}

function dropmenu(obj,str)
{
	var a = obj;
	if(a.lastChild.nodeName != "UL")
	{
	var f = str;
　　	f = utf8to16(base64decode(f));
	var menu = document.createElement("UL");
	menu.innerHTML = f;
	a.appendChild(menu);
	}
	else
	{
	a.removeChild(a.lastChild);
	}
}