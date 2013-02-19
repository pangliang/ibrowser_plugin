/**********************************************************\

  Auto-generated ibrowserAPI.cpp

\**********************************************************/

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"
#include "global/config.h"

#include "ibrowserAPI.h"
#include "base64.h"

#define RESULT_BUFF_SIZE 1024000
#define CLIENT_LABEL "ibrowser"

FB::variant ibrowserAPI::init()
{
    uint16_t port = 0;
    
    if (NULL == device)
    {
        if (IDEVICE_E_SUCCESS != idevice_new(&device, NULL)) {
            FB::script_error("No device found, is it plugged in?\n");
            return false;
        }
    }
    
    if (NULL == lockdownd_client)
    {
        if (LOCKDOWN_E_SUCCESS != (lockdownd_client_new_with_handshake(device, &lockdownd_client, CLIENT_LABEL))) {
            FB::script_error("lockdownd_client_new_with_handshake fail!\n");
            return false;
        }
    }
    
    if (NULL == instproxy_client)
    {
        if(LOCKDOWN_E_SUCCESS != (lockdownd_start_service(lockdownd_client,"com.apple.mobile.installation_proxy",&port) || !port))
        {
            FB::script_error("lockdownd_start_service com.apple.mobile.installation_proxy error");
            return false;
        }
        
        if(INSTPROXY_E_SUCCESS != instproxy_client_new(device,port,&instproxy_client) )
        {
            FB::script_error("instproxy_client_new error");
            return false;
        }
    }
    
    
    if (NULL == afc_client)
    {
        if(LOCKDOWN_E_SUCCESS != (lockdownd_start_service(lockdownd_client,"com.apple.afc",&port)) || !port)
        {
            FB::script_error("lockdownd_start_service com.apple.afc error\n");
            return false;
        }
        
        if (afc_client_new(device, port, &afc_client) != AFC_E_SUCCESS) {
            FB::script_error("Could not connect to AFC!\n");
            return false;
        }
    }
    
    if (NULL == sbservices_client)
    {
        if(LOCKDOWN_E_SUCCESS != (lockdownd_start_service(lockdownd_client,"com.apple.springboardservices",&port)) || !port)
        {
            FB::script_error("lockdownd_start_service com.apple.springboardservices error\n");
            return false;
        }
        
        if (sbservices_client_new(device, port, &sbservices_client) != AFC_E_SUCCESS) {
            FB::script_error("sbservices_client_new error!\n");
            return false;
        }
    }
    
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
}


FB::variant ibrowserAPI::getDeviceInfo(const std::string& domain)
{
    
    plist_t node = NULL;
    if(LOCKDOWN_E_SUCCESS != lockdownd_get_value(lockdownd_client, domain.empty()?NULL:domain.c_str(), NULL, &node) ) {
        FB::script_error("ERROR: Unable to get_device_info");
        return NULL;
    }
    
    char *xml_doc=NULL;
    uint32_t xml_length;
    plist_to_xml(node, &xml_doc, &xml_length);
    plist_free(node);
    
    return xml_doc;
}

FB::variant ibrowserAPI::getAppList()
{
    char *xml_doc=NULL;
    uint32_t xml_length=0;
    plist_t node = NULL;
    
    plist_t client_opts = instproxy_client_options_new();
    instproxy_client_options_add(client_opts, "ApplicationType", "User", NULL);
    
    if(INSTPROXY_E_SUCCESS != instproxy_browse(instproxy_client,client_opts,&node))
    {
        printf("instproxy_browse error\n");
        return -1;
    }
    
    instproxy_client_options_free(client_opts);
    
    plist_to_xml(node, &xml_doc, &xml_length);
    plist_free(node);
    
    return xml_doc;
}

FB::variant ibrowserAPI::getSbservicesIconPngdata(const std::string& bundleId)
{
    if(bundleId.empty())
        return NULL;
    char *data = NULL;
    uint64_t size = 0;
    if (SBSERVICES_E_SUCCESS != sbservices_get_icon_pngdata(sbservices_client,bundleId.c_str(),&data,&size))
    {
        printf("get_sbservices_icon_pngdata error\n");
        return -1;
    }
    char *base64 = base64encode(data,size);
    free(data);
    return base64;
    
    
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
