/**
 * Created with JetBrains PhpStorm.
 * User: liangwei
 * Date: 13-2-26
 * Time: 下午2:16
 * To change this template use File | Settings | File Templates.
 */
var plugin;
$(function(){

    chrome.runtime.onSuspend.addListener(function() {
        log("chrome.runtime.onSuspend...");
        chrome.runtime.reload();
    });
    
    plugin=document.getElementById("pluginId");
    
    plugin.setIdeviceEventCallback(function(event){
        log("setIdeviceEventCallback",event);
        if(event == 1){
            deskNotify('一枚Iphone连接到了电脑!','你可以使用ibrowser对其进行管理咯!' );
        }
    });

    chrome.webRequest.onBeforeRequest.addListener(
        function(info) {
            log(info);
            
            return {
                redirectUrl: chrome.runtime.getURL("download.html?file="+info.url)
            };
        },
        {urls: ["*://*/*.ipa*"]},
        ["blocking"]
    );

});
