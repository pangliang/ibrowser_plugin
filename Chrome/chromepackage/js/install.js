
$(function(){

    var plugin=new Plugin(document.getElementById("pluginId"));

    function basename(path) {
        return path.replace(/\\/g,'/').replace( /.*\//, '');
    }

    function install(selectFile)
    {
        var fileName=basename(selectFile);
        var div=$("#install_app_buttion").clone();
        div.attr("id",fileName);
        $("#install_app_buttion").after(div);
        
        div.find("#selTip").html("正在传输文件到手机..."+fileName);
        plugin.uploadFile(
            selectFile,
            function(proc){
                div.find("#progressBar").width(proc*100+"%");
            },
            function(pkgName){
                if(pkgName && "" != pkgName)
                {
                    console.log(pkgName);
                    div.find("#selTip").html("传输完成, 安装..."+fileName);
                    plugin.installPackage(
                        pkgName,
                        function(xml){
                            var p= $.plist(xml);
                            console.log(p);
                            if(!p.Error && p.Status != "Complete")
                            {
                                div.find("#selTip").html("传输完成, 安装..."+fileName+"..."+ p.Status);
                                div.find("#progressBar").width(p.PercentComplete+"%");
                            }
                        },
                        function(e){
                            console.log(e);
                            div.find("#selTip").html("安装成功..."+fileName);
                            div.find("#progressBar").width("100%");
                        },
                        function(e){
                            console.log(e);
                            div.find("#selTip").html("安装"+fileName+"失败,"+ e);
                        }
                    );
                }else{
                    div.find("#selTip").html("上传失败!");
                }
            },
            function(e){
                console.log(e);
            }
            
        );
    }
    
    function select()
    {
        plugin.openDialog(function(files){
            console.log(files);
            for(var i in files)
                install(files[i]);
        });
        
    }

    $("#install_app_buttion").on("click",function(){
        select();
    });

    //select();
    
});