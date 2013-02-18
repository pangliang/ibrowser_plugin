
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
        $("#app_list > *:not(':first,:last')").html("");
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
                var pngdata=plugin.getSbservicesIconPngData(app["CFBundleIdentifier"]);
                div.find("#icon").attr("src",pngdata);
                $("#app_list_temp").after(div);
            });
        }
    });

    $("#install_app_buttion").on("click",function(){
        var pkg_path=plugin.uploadFile();
        console.log(pkg_path);
        if(pkg_path && "" != pkg_path)
            plugin.installPackage(pkg_path);
    });

    //$("#tab_device_info").click();
    //plugin.uploadFile("/Volumes/h_win/mac_downloads/Clear-v1.2.1.ipa");
    //plugin.installPackage("/Downloads/Clear-v1.2.1.ipa");
});