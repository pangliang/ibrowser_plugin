
$(function(){

    var plugin=new Plugin(document.getElementById("pluginId"));
    
    function basename(path) {
        return path.replace(/\\/g,'/').replace( /.*\//, '');
    }
    
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
        $("#app_list > *:not(':first,:last')").remove();
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
                plugin.getSbservicesIconPngData(app["CFBundleIdentifier"],function(data){
                    div.find("#icon").attr("src","data:image/png;base64,"+data);
                });
                
                $("#app_list_temp").before(div);
            });
        }
    });

    $("#install_app_buttion").on("click",function(){
        window.open("install.html","_blank","width=100");
    });

});