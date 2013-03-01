$(function(){

    var plugin=chrome.extension.getBackgroundPage().plugin;
    
    $("#tabs li").on("click",function(){
        $("#tabs li.active").removeClass("active");
        $(this).addClass("active");

        $("#content > div").hide();
        var tabId=$(this).attr("id").replace("tab_","");
        $("#"+tabId).show();
    });

    $("#tab_device_info").on("click",function(){
        plugin.getDeviceInfo(new Array("","com.apple.mobile.battery","com.apple.disk_usage"),function(rs){
            var p=null;
            for(index in rs)
            {
                p=$.extend(p, $.plist(rs[index]));
                
            }
            console.log(p);
            if(p){
                p.TotalDataAvailable= tools.fsizeFormat(p.TotalDataAvailable);
                p.TotalDataCapacity= tools.fsizeFormat(p.TotalDataCapacity);
                p.TotalDiskCapacity= tools.fsizeFormat(p.TotalDiskCapacity);
                p.TotalSystemAvailable= tools.fsizeFormat(p.TotalSystemAvailable);
                p.TotalSystemCapacity= tools.fsizeFormat(p.TotalSystemCapacity);
                p.AmountDataAvailable= tools.fsizeFormat(p.AmountDataAvailable);
                p.AmountDataReserved= tools.fsizeFormat(p.AmountDataReserved);
                
                $("#device_info").find(".item").each(function(){
                    var infoName= $(this).attr("id");
                    $(this).text(p[infoName]);
                });
            }
        });
        
        
    });
    
    
    var pressTimer;
    var longPressTime = 400; //长按生效时间
    function appOnClick(){
        console.log($(this));
    };

    $("#tab_app_list").on("click",function(){
        $("#app_list > *:not(':first,:last')").remove();
        plugin.getAppList(function(result){
            var p= $.plist(result);
            console.log(p);
            if(p){
                $.each(p,function(i,app){
                    var div=$("#app_list_temp").clone();

                    //长按显示卸载icon
                    div.on("mousedown",function(){
                        pressTimer = window.setTimeout(function() {
                            console.log("long press....");
                            $("#app_list img.delete").show();
                            $("#app_list img.delete").on("click",function(){
                                var appid=$(this).attr("id");
                                plugin.uninstallPackage(appid,function(){},function(){
                                    var divid=appid.replace(new RegExp("\\.","gm"),"\\.");
                                    var displayName=$("div#"+divid+" #CFBundleDisplayName").html();
                                    tools.deskNotify("卸载成功!",displayName+"已从您的手机卸载")
                                    $("div#"+divid).hide();
                                },function(e){
                                    console.log(e);
                                });
                                return false;
                            });
                            $(".app").jrumble({
                                x: 1,
                                y: 1,
                                rotation: 2,
                                speed:80
                            }).trigger('startRumble');
                            $(document).on("mousedown",function(){
                                console.log("document click");
                                $("#app_list img.delete").hide();
                                $(".app").trigger('stopRumble');
                                $(document).off("mousedown");
                            });

                        },longPressTime);
                        return false;
                    }).on("mouseup",function(){
                            clearTimeout(pressTimer);
                            return false;
                        });

                    div.find(".item").each(function(){
                        var infoName= $(this).attr("id");
                        $(this).html(app[infoName]);
                    });
                    div.attr("id",app["CFBundleIdentifier"]);
                    div.find(".delete").attr("id",div.attr("id"));

                    plugin.getSbservicesIconPngdata(app["CFBundleIdentifier"],function(data){
                        div.find("#icon").attr("src","data:image/png;base64,"+data);
                    });

                    $("#app_list_temp").before(div);
                    div.show();
                });
            }
        });
        
    });

    $("#install_app_buttion").on("click",function(){
        window.open("install.html","newwindow","width=800px,height=600px");
    });

    $("#tab_device_info").click();

});