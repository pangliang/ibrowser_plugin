ibrowser_plugin
===============

iphone管理软件浏览器插件版

类似itools的ios管理软件, chrome, firefox浏览器插件版.

##软件功能
###完成:

*   获取基础信息
*   获取电量信息
*   获取硬盘容量
*   获取以安装软件列表和信息
*   获取软件icon
*   上传文件
*   安装APP

###开发计划(优先级高到低)
*   将ibrowser中的文件放出到上一层
*   同时install多个app是不允许的
*   卸载软件
*   后台监控手机的插入
*   自动reinit
*   浏览器通知弹窗
*   指定上传目录
*   获取软件照片电影占容量
*   文件管理
*   越狱手机的文件管理

##支持浏览器和平台

###macos
*   chrome   -- ok
*   firefox  -- no

###windows
*   chrome   -- no
*   firefox  -- no

###linux
*   chrome   -- no
*   firefox  -- no


##已知BUG
*   js回调函数是smart port, instproxy_install 本身就使用了异步返回, 
需要在instproxy_install的回调函数中调用js回调函数. 单离开installPackage函数, js声明周期到, 被回收,
instproxy_install的回调函数中就无法再回调js的回调函数.
临时解决办法是使用一个全局的`std::map<std::string,FB::JSObjectPtr> callbackMap`把js回调保存起来, 
这样永远不清的话这个map会越来越大. 不过仅仅是不优雅, 没有谁只用会装个几十万个软件

##创建Project

* 下载firebreath

            git clone git://github.com/firebreath/FireBreath.git -b firebreath-1.7 firebreath-1.7
    
* 创建
    
            prepmac.sh ~/ibrowser_plugin/ibrowser

* 打开工程在`build/FireBreath.xcodeproj`, `ALL_BUILD`一次
* 打开工程在`build/projects/ibrowser/ibrowser.xcodeproj`, 修改所有target的`Architectures`为`32-bit Intel`
* 把`build/projects/ibrowser/Debug/ibrowser_x86_64.plugin` 链接到`ibrowser_plugin/ibrowser/Chrome/chromepackage`目录下
 
