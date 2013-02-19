ibrowser_plugin
===============

iphone管理软件浏览器插件版

类似itools的ios管理软件, chrome, firefox浏览器插件版.


##创建Project

* 下载firebreath

      git clone git://github.com/firebreath/FireBreath.git -b firebreath-1.7 firebreath-1.7
    
* 创建
    
      ./prepmac.sh /ibrowser_plugin/ibrowser

* 打开工程在`firebreath-1.7/build/FireBreath.xcodeproj`, `ALL_BUILD`一次
* 打开工程在`firebreath-1.7/build/projects/ibrowser/ibrowser.xcodeproj`, 修改所有target的`Architectures`为`32-bit Intel`
* 把`firebreath-1.7/build/projects/ibrowser/Debug/ibrowser_x86_64.plugin` 链接到`ibrowser_plugin/ibrowser/Chrome/chromepackage`目录下
