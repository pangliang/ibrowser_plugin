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
    });
});
