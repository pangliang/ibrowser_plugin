/**
 * Created with JetBrains PhpStorm.
 * User: liangwei
 * Date: 13-2-26
 * Time: 下午2:16
 * To change this template use File | Settings | File Templates.
 */
var plugin;
var downloadList=new Array();
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
    
    function download(info)
    {
        info.status='downloading';
        info.target="/Volumes/h_win/mac_downloads/"+basename(info.url).split('?')[0];
        downloadList.push(info);
        
        plugin.downloadFile(info.url,info.target,
            function(rDlTotal,rDlNow){
                info.rDlTotal=rDlTotal;
                info.rDlNow=rDlNow;
            },
            function(p){
                info.status='已完成';
            },
            function(p){
                info.status='下载失败:'+p;
            }
        );
    }

    chrome.webRequest.onBeforeRequest.addListener(
        function(info) {
            log(info);
            download(info);
            return {
                redirectUrl: chrome.runtime.getURL("download.html?file="+info.url)
            };
        },
        {urls: ["*://*/*.ipa*"]},
        ["blocking"]
    );

});
