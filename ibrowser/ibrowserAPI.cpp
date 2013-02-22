/**********************************************************\

  Auto-generated ibrowserAPI.cpp

\**********************************************************/

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"
#include "global/config.h"

#include "ibrowserAPI.h"
#include "base64.h"
#include "DOM/Window.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>

#define RESULT_BUFF_SIZE 1024000
#define CLIENT_LABEL "ibrowser"

FB::variant ibrowserAPI::init(F_CB)
{
    uint16_t port = 0;
    
    if (NULL == device)
    {
        if (IDEVICE_E_SUCCESS != idevice_new(&device, NULL)) {
            ERRO("No device found, is it plugged in?");
            return false;
        }
    }
    
    if (NULL == lockdownd_client)
    {
        if (LOCKDOWN_E_SUCCESS != (lockdownd_client_new_with_handshake(device, &lockdownd_client, CLIENT_LABEL))) {
            ERRO("lockdownd_client_new_with_handshake fail!");
            return false;
        }
    }
    
    if (NULL == instproxy_client)
    {
        if(LOCKDOWN_E_SUCCESS != (lockdownd_start_service(lockdownd_client,"com.apple.mobile.installation_proxy",&port) || !port))
        {
            ERRO("lockdownd_start_service com.apple.mobile.installation_proxy error");
            return false;
        }
        
        if(INSTPROXY_E_SUCCESS != instproxy_client_new(device,port,&instproxy_client) )
        {
            ERRO("instproxy_client_new error");
            return false;
        }
    }
    
    
    if (NULL == afc_client)
    {
        if(LOCKDOWN_E_SUCCESS != (lockdownd_start_service(lockdownd_client,"com.apple.afc",&port)) || !port)
        {
            ERRO("lockdownd_start_service com.apple.afc error");
            return false;
        }
        
        if (afc_client_new(device, port, &afc_client) != AFC_E_SUCCESS) {
            ERRO("Could not connect to AFC!");
            return false;
        }
    }
    
    if (NULL == sbservices_client)
    {
        if(LOCKDOWN_E_SUCCESS != (lockdownd_start_service(lockdownd_client,"com.apple.springboardservices",&port)) || !port)
        {
            ERRO("lockdownd_start_service com.apple.springboardservices error");
            return false;
        }
        
        if (sbservices_client_new(device, port, &sbservices_client) != AFC_E_SUCCESS) {
            ERRO("sbservices_client_new error!");
            return false;
        }
    }
    
    SUCC();
    return true;

}

FB::variant ibrowserAPI::clean()
{
    if (NULL != device)
    {
        idevice_free(device);
        device = NULL;
    }
    
    if (NULL != instproxy_client)
    {
        instproxy_client_free(instproxy_client);
        instproxy_client = NULL;
    }
    
    if (NULL != lockdownd_client)
    {
        lockdownd_client_free(lockdownd_client);
        lockdownd_client = NULL;
    }
    
    if (NULL != sbservices_client)
    {
        sbservices_client_free(sbservices_client);
        sbservices_client = NULL;
    }
    
    if (NULL != afc_client)
    {
        afc_client_free(afc_client);
        afc_client = NULL;
    }
    
    return true;
}


FB::variant ibrowserAPI::getDeviceInfo(const std::string& domain,F_CB)
{
    
    plist_t node = NULL;
    if(LOCKDOWN_E_SUCCESS != lockdownd_get_value(lockdownd_client, domain.empty()?NULL:domain.c_str(), NULL, &node) ) {
        ERRO("ERROR: Unable to get_device_info");
        return NULL;
    }
    
    char *xml_doc=NULL;
    uint32_t xml_length;
    plist_to_xml(node, &xml_doc, &xml_length);
    plist_free(node);

    /*
    FB::DOM::WindowPtr window = m_host->getDOMWindow();

    // Check if the DOM Window has an alert peroperty
    if (window && window->getJSObject()->HasProperty("jQuery")) {
        // Create a reference to alert
        FB::JSObjectPtr obj = window->getProperty<FB::JSObjectPtr>("jQuery");

        // Invoke alert with some text
        return obj->Invoke("plist", FB::variant_list_of(xml_doc));
    }*/
    
    return xml_doc;
}

FB::variant ibrowserAPI::getAppList(F_CB)
{
    char *xml_doc=NULL;
    uint32_t xml_length=0;
    plist_t node = NULL;
    
    plist_t client_opts = instproxy_client_options_new();
    instproxy_client_options_add(client_opts, "ApplicationType", "User", NULL);
    
    if(INSTPROXY_E_SUCCESS != instproxy_browse(instproxy_client,client_opts,&node))
    {
        ERRO("instproxy_browse error");
        return NULL;
    }
    
    instproxy_client_options_free(client_opts);
    
    plist_to_xml(node, &xml_doc, &xml_length);
    plist_free(node);
    
    return xml_doc;
}

FB::variant ibrowserAPI::getSbservicesIconPngdata(const std::string& bundleId,F_CB,boost::optional<bool> noThread)
{
    if(!noThread)
    {
        boost::thread t(boost::bind(&ibrowserAPI::getSbservicesIconPngdata,this, bundleId, CB_USE,true));
        return true;
    }
    
    if(bundleId.empty())
        return NULL;
    char *data = NULL;
    uint64_t size = 0;
    if (SBSERVICES_E_SUCCESS != sbservices_get_icon_pngdata(sbservices_client,bundleId.c_str(),&data,&size))
    {
        ERRO("get_sbservices_icon_pngdata error");
        return NULL;
    }
    char *base64 = base64encode(data,size);
    free(data);
    
    SUCC(base64);
    
    return base64;
    
}

#ifdef WIN32
#else
#include <cocoa/Cocoa.h>
FB::variant ibrowserAPI::openDialog(F_SUCC, boost::optional<bool> noThread)
{
    //有安装在进行时, 文件框会卡住
    if(!noThread){
        boost::thread t(boost::bind(&ibrowserAPI::openDialog,this, scb ,true));
        return true;
    }
    
    NSOpenPanel* openDlg = [NSOpenPanel openPanel];
    
    [openDlg setCanChooseFiles:TRUE]; //确定可以选择文件
    [openDlg setCanChooseDirectories:FALSE]; //可以浏览目录
    [openDlg setAllowsMultipleSelection:TRUE]; //不可以多选
    [openDlg setAllowsOtherFileTypes:FALSE]; //不可以选择其他格式类型的文件
    [openDlg setAllowedFileTypes:[NSArray arrayWithObject:@"ipa"]]; //可以选择.png后缀的文件
    if ([openDlg runModal] == NSOKButton) {  //如果用户点OK
        std::vector<const char *> files;
        for (id obj in [openDlg URLs])
            files.insert(files.end(),[[obj path] UTF8String]);
        SUCC(files);
        return files;
    }
    
    return false;
}
#endif

FB::variant ibrowserAPI::uploadFile(const std::string& fileName, F_CB, const boost::optional<FB::JSObjectPtr>& processCallback, boost::optional<bool> noThread)
{
    if(!noThread)
    {
        boost::thread t(boost::bind(&ibrowserAPI::uploadFile,this, fileName, scb,ecb,processCallback, true));
        return true;
    }
        
    if(fileName.empty())
        return NULL;
    
    const char *file_name=fileName.c_str();
    
    char target_file[1024];
    sprintf(target_file, "%s/%s",uploadFileDir.c_str(), basename((char *)file_name));
    uint64_t target_file_handle = 0;
    if (AFC_E_SUCCESS != afc_file_open(afc_client, target_file, AFC_FOPEN_WRONLY, &target_file_handle)){
        ERRO("afc_file_open error!");
        return NULL;
    }
    
    FILE *file_handle= fopen(file_name, "r");
    if (!file_handle)
    {
        afc_remove_path(afc_client, target_file);
        ERRO("open file error!");
        return NULL;
    }
    
    off_t fileSize = 0;
    struct stat st;
    if (stat(file_name, &st) == 0)
        fileSize = st.st_size;
    static int read_buf_size = 1024;
    char read_buf[read_buf_size];
    int bytes_read;
    uint32_t bytes_written = 0;
    uint32_t doneSize = 0;
    while((bytes_read = fread(read_buf, 1, read_buf_size, file_handle)) >0 )
    {
        if (AFC_E_SUCCESS != afc_file_write(afc_client, target_file_handle, read_buf, bytes_read, &bytes_written) || bytes_read !=bytes_written)
        {
            ERRO("afc_file_write error!");
            return NULL;
        }
        
        memset(read_buf, 0, read_buf_size);
        
        doneSize = doneSize + bytes_read;
        if(processCallback && fileSize > 0)
            (*processCallback)->InvokeAsync("", FB::variant_list_of((double)doneSize/fileSize));
    }
    
    SUCC(target_file);
    
    return target_file;
}

FB::variant ibrowserAPI::installPackage(const std::string& fileName, F_CB,boost::optional<bool> noThread)
{
    if(!noThread)
    {
        boost::thread t(boost::bind(&ibrowserAPI::installPackage,this, fileName, CB_USE, true));
        return true;
    }
    
    if(fileName.empty())
        return NULL;
    
    const char *file_name=fileName.c_str();
    
    callbackMap[std::string("installPackage")+fileName]=*scb;
    if (INSTPROXY_E_SUCCESS != instproxy_install(instproxy_client, file_name, NULL, &ibrowserAPI::installCallback, (void*)(*scb).get()))
    {
        ERRO("instproxy_install error");
        return false;
    }
    return true;
}

void ibrowserAPI::installCallback(const char *operation, plist_t status, void *user_data) {
    char *xml_doc=NULL;
    uint32_t xml_length;
    plist_to_xml(status, &xml_doc, &xml_length);
    if(user_data)
    {
        boost::shared_ptr<FB::JSObject> callback = *(boost::shared_ptr<FB::JSObject>*)(user_data);
        callback->InvokeAsync("", FB::variant_list_of(xml_doc));
    }
    return;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn FB::variant ibrowserAPI::echo(const FB::variant& msg)
///
/// @brief  Echos whatever is passed from Javascript.
///         Go ahead and change it. See what happens!
///////////////////////////////////////////////////////////////////////////////
FB::variant ibrowserAPI::echo(const FB::variant& msg)
{
    static int n(0);
    fire_echo("So far, you clicked this many times: ", n++);
    // return "foobar";
    return msg;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn ibrowserPtr ibrowserAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
ibrowserPtr ibrowserAPI::getPlugin()
{
    ibrowserPtr plugin(m_plugin.lock());
    if (!plugin) {
        throw FB::script_error("The plugin is invalid");
    }
    return plugin;
}

// Read/Write property testString
std::string ibrowserAPI::get_testString()
{
    return m_testString;
}

void ibrowserAPI::set_testString(const std::string& val)
{
    m_testString = val;
}

// Read-only property version
std::string ibrowserAPI::get_version()
{
    return FBSTRING_PLUGIN_VERSION;
}

void ibrowserAPI::testEvent()
{
    fire_test();
}
