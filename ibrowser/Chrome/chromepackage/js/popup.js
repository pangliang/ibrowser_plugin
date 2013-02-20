
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
                var pngdata=plugin.getSbservicesIconPngData(app["CFBundleIdentifier"]);
                div.find("#icon").attr("src",pngdata);
                $("#app_list_temp").before(div);
            });
        }
    });

    $("#install_app_buttion").on("click",function(){
        var selectFile=plugin.openDialog();
        $("#install_app_buttion #selTip").html("正在传输文件到手机..."+basename(selectFile));
        var pkg_path=plugin.uploadFile(
            selectFile,
            function(pkgName){
                if(pkgName && "" != pkgName)
                {
                    $("#install_app_buttion #selTip").html("传输完成, 安装..."+basename(selectFile));
                    plugin.installPackage(
                        pkgName,
                        function(xml){
                            var p= $.plist(xml);
                            console.log(p);
                            if(p.error)
                            {
                                $("#install_app_buttion #selTip").html("安装"+basename(selectFile)+"失败,"+ p.error);
                            }else if(p.Status == "Complete"){
                                $("#install_app_buttion #selTip").html("安装成功..."+basename(selectFile));
                                $("#install_app_buttion #progressBar").width("100%");
                            } else{
                                $("#install_app_buttion #selTip").html("传输完成, 安装..."+basename(selectFile)+"..."+ p.Status);
                                $("#install_app_buttion #progressBar").width(p.PercentComplete+"%");
                            }
                        }
                    );
                }else{
                    $("#install_app_buttion #selTip").html("上传失败!");
                }
            },
            function(proc){
                $("#install_app_buttion #progressBar").width(proc*100+"%");
            }
        );
        
    });

});