/**
 * Created with JetBrains PhpStorm.
 * User: liangwei
 * Date: 13-2-26
 * Time: 下午2:16
 * To change this template use File | Settings | File Templates.
 */
var plugin;
var downloadList = new Array();
$(function () {

    chrome.runtime.onSuspend.addListener(function () {
        log("chrome.runtime.onSuspend...");
        chrome.runtime.reload();
    });

    plugin = document.getElementById("pluginId");

    plugin.setIdeviceEventCallback(function (event) {
        log("setIdeviceEventCallback", event);
        if (event == 1) {
            deskNotify('一枚Iphone连接到了电脑!', '你可以使用ibrowser对其进行管理咯!');
        }
    });

    function download(info, filename) {
        info.status = 'downloading';
        info.filename=filename;
        downloadList.push(info);

        deskNotify('下载', "开始为您下载"+filename);

        plugin.downloadFile(info.url,info.filename,
            function (rDlTotal, rDlNow) {
                info.rDlTotal = rDlTotal;
                info.rDlNow = rDlNow;
                info.progressBar=rDlNow/rDlTotal;
            },
            function (file) {
                info.status = '下载完成, 正在上传至手机...';
                deskNotify('下载完成, 开始安装...', filename+"下载完成, 马上为您安装....");
                
                plugin.uploadFile(
                    file,
                    function(proc){info.progressBar=proc;},
                    function(pkgName){
                        plugin.installPackage(
                            pkgName,
                            function(xml){
                                var p= $.plist(xml);
                                if(!p.Error && p.Status != "Complete")
                                {
                                    info.progressBar=p.PercentComplete/100;
                                    info.status="传输完成, 安装..."+fileName+"..."+ p.Status;
                                }
                            },
                            function(e){
                                info.status = '安装成功!';
                                deskNotify('安装成功!',filename+'已被安装!');
                                info.progressBar=100;
                            },
                            function(e){
                                info.status = '安装失败:'+e;
                                deskNotify('安装失败', filename+"安装错误:"+e);
                            }
                        );
                    },
                    function(e){
                        info.status = '上传失败:'+e;
                        deskNotify('安装失败', filename+"上传错误:"+e);
                    }

                );
            },
            function (p) {
                info.status = '下载失败:' + p;
            }
        );
    }

    chrome.webRequest.onHeadersReceived.addListener(
        function (info) {

            for (var index in info.responseHeaders) {
                var header = info.responseHeaders[index];
                var match = null;
                if ("Content-Disposition" == header.name && (match = header.value.match(/attachment;filename="(.*\.ipa)"/))) {
                    var filename = match[1];
                    download(info, filename);
                    window.open("download.html");
                    chrome.tabs.remove(info.tabId);
                    return {
                        cancel:true
                    };
                    break;

                }

            }

        },
        {urls:["<all_urls>"]},
        ["blocking", "responseHeaders"]
    );

});
