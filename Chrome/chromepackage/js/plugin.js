function Plugin(plugin)
{

    plugin.init(function(e){
        console.log(e);
    },function(e){
        console.log(e);
    });

    this.readableFileSize=function(size) {
        var units = ['B', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'];
        var i = 0;
        while(size >= 1024) {
            size /= 1024;
            ++i;
        }
        return size.toFixed(2) + ' ' + units[i];
    }

    this.getDiskUsage=function(){
        var result=plugin.getDeviceInfo("com.apple.disk_usage");
        var p= $.plist(result);
        if(p)
        {
            p.TotalDataAvailable= this.readableFileSize(p.TotalDataAvailable);
            p.TotalDataCapacity= this.readableFileSize(p.TotalDataCapacity);
            p.TotalDiskCapacity= this.readableFileSize(p.TotalDiskCapacity);
            p.TotalSystemAvailable= this.readableFileSize(p.TotalSystemAvailable);
            p.TotalSystemCapacity= this.readableFileSize(p.TotalSystemCapacity);
            p.AmountDataAvailable= this.readableFileSize(p.AmountDataAvailable);
            p.AmountDataReserved= this.readableFileSize(p.AmountDataReserved);
        }
        return p;
    }

    this.getBatteryInfo=function(){
        var result=plugin.getDeviceInfo("com.apple.mobile.battery");
        var p= $.plist(result);
        return p;
    }

    this.getDeviceInfo=function(){
        var result=plugin.getDeviceInfo("");
        var p= $.plist(result);
        return p;
    }

    this.getAppList=function(){
        var result=plugin.getAppList();
        var p= $.plist(result);
        return p;
    }

    this.uploadFile=function(fileName,succCallback,failCallback,processCallback){
        var result=plugin.uploadFile(fileName,succCallback,failCallback,processCallback);
        return result;
    }

    this.installPackage=function(fileName,processCallback,succCallback,failCallback){
        var result=plugin.installPackage(fileName,processCallback,succCallback,failCallback);
        return result;
    }

    this.getSbservicesIconPngData=function(bundleId,succCallback,failCallback){
        var result=plugin.getSbservicesIconPngdata(bundleId,succCallback,failCallback);
        return "data:image/png;base64,"+result;
    }

    this.openDialog=function(succCallback,failCallback){
        return plugin.openDialog(succCallback,failCallback);
    }

}