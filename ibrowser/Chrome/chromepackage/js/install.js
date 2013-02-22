
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
            function(pkgName){
                if(pkgName && "" != pkgName)
                {
                    div.find("#selTip").html("传输完成, 安装..."+fileName);
                    plugin.installPackage(
                        pkgName,
                        function(xml){
                            var p= $.plist(xml);
                            console.log(p);
                            if(p.Error)
                            {
                                div.find("#selTip").html("安装"+fileName+"失败,"+ p.Error);
                            }else if(p.Status == "Complete"){
                                div.find("#selTip").html("安装成功..."+fileName);
                                div.find("#progressBar").width("100%");
                            } else{
                                div.find("#selTip").html("传输完成, 安装..."+fileName+"..."+ p.Status);
                                div.find("#progressBar").width(p.PercentComplete+"%");
                            }
                        }
                    );
                }else{
                    div.find("#selTip").html("上传失败!");
                }
            },
            function(e){
                console.log(e);
            },
            function(proc){
                div.find("#progressBar").width(proc*100+"%");
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

    plugin.installPackage(
        "aklsdjfkasd",
        function(xml){
            var p= $.plist(xml);
            console.log(p);
        },
        function(e){
            console.log(e);
        },
        true
    );

    
});