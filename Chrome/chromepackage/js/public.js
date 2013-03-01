/**
 * Created with JetBrains PhpStorm.
 * User: liangwei
 * Date: 13-3-1
 * Time: 下午3:24
 * To change this template use File | Settings | File Templates.
 */


tools={
    deskNotify : function(title,msg){
        var notification = webkitNotifications.createNotification(
            '/icon.jpeg',
            title,
            msg
        );
        notification.show();
    },

    basename : function (path) {
        return path.replace(/\\/g,'/').replace( /.*\//, '');
    },

    fsizeFormat: function (size) {
        var units = ['B', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'];
        var i = 0;
        while(size >= 1024) {
            size /= 1024;
            ++i;
        }
        return size.toFixed(2) + ' ' + units[i];
    },
}