/**
 * Created with JetBrains PhpStorm.
 * User: liangwei
 * Date: 13-3-1
 * Time: 下午3:24
 * To change this template use File | Settings | File Templates.
 */

function getUrlParam(name)
{
    var reg = new RegExp("(^|&)"+ name +"=([^&]*)(&|$)");
    var r = window.location.search.substr(1).match(reg);
    if (r!=null) return unescape(r[2]); return null;
}

function deskNotify(title, msg) {
    var n = new Notification(
        title,
        {
            body:msg,
            icon:"icon.jpeg"
        }
    );

    n.onshow = function () { 
        setTimeout(n.close, 5000); 
    }

}

function basename(path) {
    return path.replace(/\\/g, '/').replace(/.*\//, '');
}

function fsizeFormat(size) {
    var units = ['B', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'];
    var i = 0;
    while (size >= 1024) {
        size /= 1024;
        ++i;
    }
    return size.toFixed(2) + ' ' + units[i];
}

function log(p1,p2,p3,p4,p5){
    console.log(p1,p2,p3,p4,p5);
}
