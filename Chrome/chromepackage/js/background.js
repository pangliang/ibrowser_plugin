/**
 * Created with JetBrains PhpStorm.
 * User: liangwei
 * Date: 13-2-26
 * Time: 下午2:16
 * To change this template use File | Settings | File Templates.
 */
var plugin;
$(function(){
    plugin=document.getElementById("pluginId");
    
    plugin.setIdeviceEventCallback(function(event){
        console.log("setIdeviceEventCallback",event);
        if(event == 1){
            tools.deskNotify('一枚Iphone连接到了电脑!','你可以使用ibrowser对其进行管理咯!' );
        }
    });
});
