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
        info.target = "/Volumes/h_win/mac_downloads/" + filename;
        downloadList.push(info);

        plugin.downloadFile(info.url, info.target,
            function (rDlTotal, rDlNow) {
                info.rDlTotal = rDlTotal;
                info.rDlNow = rDlNow;
            },
            function (p) {
                info.status = '已完成';
                deskNotify('下载完成, 开始安装...', filename+"下载完成, 马上为您安装....");
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
