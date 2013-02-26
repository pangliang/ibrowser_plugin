/**
 * Created with JetBrains PhpStorm.
 * User: liangwei
 * Date: 13-2-26
 * Time: 下午2:16
 * To change this template use File | Settings | File Templates.
 */
var plugin;
$(function(){
    var checkTime=3000;
    var int;
    
    function init()
    {
        console.log("init..."+new Date().getTime());
        
        plugin=document.getElementById("pluginId");
        plugin.init(function(){
            checkTime=10*1000;
            window.clearInterval(int);
            int=self.setInterval(init,checkTime);
        },function(e){
            checkTime=3*1000;
            window.clearInterval(int);
            int=self.setInterval(init,checkTime);
        });
    }

    int=self.setInterval(init,checkTime);

});
