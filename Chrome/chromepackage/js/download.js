
$(function(){

    var plugin=chrome.extension.getBackgroundPage().plugin;

    var fileName=basename(files[i]);
    var div=$("#install_app_buttion").clone();
    div.attr("id",fileName);
    $("#install_app_buttion").after(div);

    div.find("#selTip").html("正在传输文件到手机..."+fileName);
    
    plugin.downloadFile(info.url,
        function(p){
            log(p);
        },
        function(p){
            log(p);
        },
        function(p){
            log(p);
        }
    ); 
});

