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
*   安装APP (支持并发操作, 后台顺序安装)
*   卸载软件
*   iphone长按卸载UI
*   后台监控手机的插入,拔出
*   浏览器通知弹窗:手机接入,安装app, 卸载app;
*   插件自动版本更新功能

###开发计划(优先级高到低)

*   获取软件照片电影占容量
*   文件管理
*   越狱手机的文件管理
*   指定上传目录
*   下载的ipa文件自动安装
*   win7平台兼容
*   firefox兼容
*   国际化
*   安装成功刷新软件列表 //弹出安装页, popup页会关闭, 暂时不实现

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

##用firebreath创建Project

* 下载firebreath

            git clone git://github.com/firebreath/FireBreath.git -b firebreath-1.7 firebreath-1.7
    
* 创建
    
            prepmac.sh ~/ibrowser_plugin/ibrowser

* 打开工程在`build/FireBreath.xcodeproj`, `ALL_BUILD`一次
* 打开工程在`build/projects/ibrowser/ibrowser.xcodeproj`, 修改所有target的`Architectures`为`32-bit Intel`
* 把`build/projects/ibrowser/Debug/ibrowser_x86_64.plugin` 链接到`ibrowser_plugin/ibrowser/Chrome/chromepackage`目录下
 
