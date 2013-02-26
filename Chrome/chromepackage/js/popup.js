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
    
    
    var pressTimer;
    var longPressTime = 1000; //长按生效时间
    function appOnClick(){
        console.log($(this));
    };

    $("#tab_app_list").on("click",function(){
        $("#app_list > *:not(':first,:last')").remove();
        var p= $.extend({},plugin.getAppList());
        console.log(p);
        if(p){
            $.each(p,function(i,app){
                var div=$("#app_list_temp").clone();
                
                //长按显示卸载icon
                div.on("mousedown",function(){
                    pressTimer = window.setTimeout(function() { 
                        console.log("long press....");
                        $("#app_list img.delete").show();
                    },longPressTime)
                }).on("mouseup",function(){
                    clearTimeout(pressTimer);
                    return false;
                });
                
                div.find(".item").each(function(){
                    var infoName= $(this).attr("id");
                    $(this).html(app[infoName]);
                });
                div.attr("id",app["CFBundleIdentifier"]);
                plugin.getSbservicesIconPngData(app["CFBundleIdentifier"],function(data){
                    div.find("#icon").attr("src","data:image/png;base64,"+data);
                });
                
                $("#app_list_temp").before(div);
                div.show();
            });
        }
    });

    $("#install_app_buttion").on("click",function(){
        window.open("install.html","newwindow","width=800px,height=600px");
    });

});