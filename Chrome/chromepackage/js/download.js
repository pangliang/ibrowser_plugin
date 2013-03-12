
$(function(){

    function reload()
    {
        var list=chrome.extension.getBackgroundPage().downloadList;

        for(var i in list)
        {
            
            var info=list[i];log(info);
            var fileName=basename(info.filename);
            var div;
            if((div=$("#"+info.requestId)) && div.length == 0)
            {
                div=$("#download_list_item_temp").clone();
                div.attr("id",info.requestId);
                div.find("#fileName").html(fileName);
                div.find("#url").html(info.url);
                div.attr("class","download_list_item");
                $("#download_list_item_temp").after(div);
            }
            div.find("#progressBar").width((info.progressBar*100)+"%");
            if(info.status == "downloading")
            {
                var speed=(info.rDlNow*1000/(new Date().getTime()-info.timeStamp));
                div.find("#speed").html(fsizeFormat(info.rDlNow)+"/"+fsizeFormat(info.rDlTotal)+" 速度:"+fsizeFormat(speed)+"/S");
            }else{
                div.find("#speed").html(info.status);
            }
            
        }
    }
    setInterval(reload,1000);
    
});

