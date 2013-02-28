$(function(){

    var plugin=chrome.extension.getBackgroundPage().plugin;
    
    function basename(path) {
        return path.replace(/\\/g,'/').replace( /.*\//, '');
    }
    
    function readableFileSize(size) {
        var units = ['B', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'];
        var i = 0;
        while(size >= 1024) {
            size /= 1024;
            ++i;
        }
        return size.toFixed(2) + ' ' + units[i];
    }
    
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
                p.TotalDataAvailable= readableFileSize(p.TotalDataAvailable);
                p.TotalDataCapacity= readableFileSize(p.TotalDataCapacity);
                p.TotalDiskCapacity= readableFileSize(p.TotalDiskCapacity);
                p.TotalSystemAvailable= readableFileSize(p.TotalSystemAvailable);
                p.TotalSystemCapacity= readableFileSize(p.TotalSystemCapacity);
                p.AmountDataAvailable= readableFileSize(p.AmountDataAvailable);
                p.AmountDataReserved= readableFileSize(p.AmountDataReserved);
                
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
                                    div.hide();
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

});