try {
    　　// 此处是可能产生例外的语句
    var plugin = document.getElementById("pluginId");
    var result=plugin.open("http://www.geeklu.com");
    console.log(result);
} catch(error) {
        　　// 此处是负责例外处理的语句
    console.log('error:'+error);
} finally {
            　　// 此处是出口语句
}

