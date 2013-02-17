
$(function(){

    var plugin=new Plugin(document.getElementById("pluginId"));

    $("#tabs li").on("click",function(){
        $("#tabs li.active").removeClass("active");
        $(this).addClass("active");

        $("#content > div").hide();
        var tabId=$(this).attr("id").replace("tab_","");
        $("#"+tabId).show();
    });

    $("#tab_device_info").on("click",function(){
        var p= $.extend({},plugin.getBatteryInfo(),plugin.getDeviceInfo(),plugin.getDiskUsage());
        console.log(p);
        if(p){
            $("#device_info").find(".item").each(function(){
                var infoName= $(this).attr("id");
                $(this).text(p[infoName]);
            });
        }
    });

    $("#tab_app_list").on("click",function(){
        var p= $.extend({},plugin.getAppList());
        console.log(p);
        if(p){
            $.each(p,function(){
                var div=$("#app_list_temp").clone();
                var app=this;
                div.find(".item").each(function(){
                    var infoName= $(this).attr("id");
                    $(this).html(app[infoName]);
                });
                div.attr("id",app["CFBundleDisplayName"]);
                $("#app_list").append(div);
            });
        }
    });

    //$("#tab_device_info").click();
    plugin.readFile("/private/var/mobile/Applications/797E872E-3BC8-4A18-9736-A06CFA54D096/AutoNavi.app/default57.png");
});