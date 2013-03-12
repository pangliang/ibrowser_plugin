
$(function(){

    var plugin=chrome.extension.getBackgroundPage().plugin;
    
    var installList = new Array();

    function install()
    {

        log(installList);
        
        var install = installList.pop();
        if(!install)
            return;
        
        var selectFile = install[0];
        var div=install[1];
        
        plugin.uploadFile(
            selectFile,
            function(proc){
                div.find("#progressBar").width(proc*100+"%");
            },
            function(pkgName){
                upload_succ(pkgName,div)
            },
            function(e){
                log(e);
            }
            
        );
    }

    function upload_succ(pkgName,div)
    {
        if(pkgName && "" != pkgName)
        {
            var fileName=basename(pkgName);
            div.find("#selTip").html("传输完成, 安装..."+fileName);
            plugin.installPackage(
                pkgName,
                function(xml){
                    var p= $.plist(xml);
                    if(!p.Error && p.Status != "Complete")
                    {
                        div.find("#selTip").html("传输完成, 安装..."+fileName+"..."+ p.Status);
                        div.find("#progressBar").width(p.PercentComplete+"%");
                    }
                },
                function(e){
                    log(e);
                    div.find("#selTip").html("安装成功..."+fileName);
                    div.find("#progressBar").width("100%");

                    install();

                    deskNotify('安装成功!',fileName+'已被安装!');


                },
                function(e){
                    log(e);
                    div.find("#selTip").html("安装"+fileName+"失败,"+ e);

                    install();
                }
            );
        }else{
            div.find("#selTip").html("上传失败!");
        }
    }
    
    function select()
    {
        plugin.openDialog(function(files){
            log(files);
            for(var i in files)
            {
                var fileName=basename(files[i]);
                var div=$("#install_app_buttion").clone();
                div.attr("id",fileName);
                $("#install_app_buttion").after(div);

                div.find("#selTip").html("正在传输文件到手机..."+fileName);

                installList.push(new Array(files[i],div));
            }
            install();
        });
        
    }

    $("#install_app_buttion").on("click",function(){
        select();
    });

    select();
    
});