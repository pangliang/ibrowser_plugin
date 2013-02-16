
$(function(){

    var plugin=document.getElementById("pluginId");

    $("#tabs li").on("click",function(){
        $("#tabs li.active").removeClass("active");
        $(this).addClass("active");
    });

    $("#tab_device_info").on("click",function(){
        var result=plugin.get_device_info();
        var p= $.plist(result);
        console.log(p);
    });

});