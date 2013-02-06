try {
    // 此处是可能产生例外的语句
    var plugin = document.getElementById("pluginId");
    var result=plugin.get_device_id();
    console.log("get_device_id"+result);
    result=plugin.get_app_list();
    console.log("get_app_list"+result);
} catch(error) {
    console.log('error:'+error);
} finally {

}

