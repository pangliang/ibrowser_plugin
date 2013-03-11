/**********************************************************\

  Auto-generated ibrowserAPI.h

\**********************************************************/

#include <string>
#include <sstream>
#include <boost/weak_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "ibrowser.h"

extern "C"{
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <libimobiledevice/installation_proxy.h>
#include <libimobiledevice/afc.h>
#include <libimobiledevice/sbservices.h>
#include <libimobiledevice/service.h>
}

#ifndef H_ibrowserAPI
#define H_ibrowserAPI

#define F_SUCC  const boost::optional<FB::JSObjectPtr>& scb
#define F_ERRO  const boost::optional<FB::JSObjectPtr>& ecb
#define F_T_FLAG    boost::optional<bool> noThread
#define F_ADD F_SUCC,F_ERRO,F_T_FLAG
#define SUCC(a1,args...)    \
    do{ \
        if(scb && (*scb)->isValid()){ \
            (*scb)->InvokeAsync("", FB::variant_list_of( a1, ##args )); \
        } \
    }while(0)
#define ERRO(msg)   \
    do{ \
        if(ecb && (*ecb)->isValid()){ \
            (*ecb)->InvokeAsync("", FB::variant_list_of(msg)); \
        } \
        printf("error:%s\n",msg); \
        clean(); \
        return false; \
    }while(0)
#define THREAD(fun,args...) \
    do{ \
        if(!noThread && scb && (*scb)->isValid() ) \
        { \
            printf("use thread:%s\n",__FUNCTION__); \
            boost::thread t(boost::bind(fun,this, ##args,scb,ecb,true)); \
            return true; \
        } \
        if(!init(scb,ecb)) return false; \
    }while(0)

class ibrowserAPI;

class Callback{
public:
    void set(const char* fn,FB::JSObjectPtr op)
    {
        list[fn]=op;
    }
    FB::JSObjectPtr get(const char* fn)
    {
        return list[fn];
    }
    
private:
    std::map<const char*,FB::JSObjectPtr> list;
};

struct DownloadConfig   //定义一个结构为了传递给my_fwrite函数.可用curl_easy_setopt的CURLOPT_WRITEDATA选项传递
{
    std::string filename;
    FILE *stream;
    FB::JSObjectPtr pcb;
    size_t doneSize;
};

class ibrowserAPI : public FB::JSAPIAuto
{
public:
    ////////////////////////////////////////////////////////////////////////////
    /// @fn ibrowserAPI::ibrowserAPI(const ibrowserPtr& plugin, const FB::BrowserHostPtr host)
    ///
    /// @brief  Constructor for your JSAPI object.
    ///         You should register your methods, properties, and events
    ///         that should be accessible to Javascript from here.
    ///
    /// @see FB::JSAPIAuto::registerMethod
    /// @see FB::JSAPIAuto::registerProperty
    /// @see FB::JSAPIAuto::registerEvent
    ////////////////////////////////////////////////////////////////////////////
    ibrowserAPI(const ibrowserPtr& plugin, const FB::BrowserHostPtr& host) :
        m_plugin(plugin), m_host(host)
    {
        registerMethod("echo",      make_method(this, &ibrowserAPI::echo));
        registerMethod("testEvent", make_method(this, &ibrowserAPI::testEvent));
        
        registerMethod("getDeviceInfo", make_method(this, &ibrowserAPI::getDeviceInfo));
        registerMethod("getAppList",      make_method(this, &ibrowserAPI::getAppList));
        registerMethod("getSbservicesIconPngdata", make_method(this, &ibrowserAPI::getSbservicesIconPngdata));
        registerMethod("openDialog", make_method(this, &ibrowserAPI::openDialog));
        registerMethod("uploadFile", make_method(this, &ibrowserAPI::uploadFile));
        registerMethod("installPackage", make_method(this, &ibrowserAPI::installPackage));
        registerMethod("uninstallPackage", make_method(this, &ibrowserAPI::uninstallPackage));
        registerMethod("setIdeviceEventCallback", make_method(this, &ibrowserAPI::setIdeviceEventCallback));
        registerMethod("downloadFile", make_method(this, &ibrowserAPI::downloadFile));
        
        // Read-write property
        registerProperty("testString",
                         make_property(this,
                                       &ibrowserAPI::get_testString,
                                       &ibrowserAPI::set_testString));
        
        // Read-only property
        registerProperty("version",
                         make_property(this,
                                       &ibrowserAPI::get_version));
        
        printf("new ibrowserAPI\n");
        
        
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @fn ibrowserAPI::~ibrowserAPI()
    ///
    /// @brief  Destructor.  Remember that this object will not be released until
    ///         the browser is done with it; this will almost definitely be after
    ///         the plugin is released.
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~ibrowserAPI() {};

    ibrowserPtr getPlugin();

    // Read/Write property ${PROPERTY.ident}
    std::string get_testString();
    void set_testString(const std::string& val);

    // Read-only property ${PROPERTY.ident}
    std::string get_version();

    // Method echo
    FB::variant echo(const FB::variant& msg);
    
    
    FB::variant getDeviceInfo(const std::vector<std::string>& domain,F_ADD);
    FB::variant getAppList(F_ADD);
    FB::variant getSbservicesIconPngdata(const std::string& bundleId,F_ADD);
    FB::variant openDialog(F_ADD);
    FB::variant uploadFile(const std::string& fileName,const boost::optional<FB::JSObjectPtr>& pcb, F_ADD);
    FB::variant installPackage(const std::string& fileName,const boost::optional<FB::JSObjectPtr>& pcb, F_ADD);
    FB::variant uninstallPackage(const std::string& fileName,const boost::optional<FB::JSObjectPtr>& pcb, F_ADD);
    FB::variant setIdeviceEventCallback(const FB::JSObjectPtr& callback,F_ADD);
    FB::variant downloadFile(const std::string& url,const std::string& target_file,const boost::optional<FB::JSObjectPtr>& pcb, F_ADD);
    static int downloadWrite(void *buffer, size_t size, size_t nmemb, void *stream);
    static int downloadProgress(void* ptr, double rDlTotal, double rDlNow, double rUlTotal, double rUlNow);
    static void installCallback(const char *operation, plist_t status, void *user_data);
    static void ideviceEventCallback(const idevice_event_t *event, void *user_data);
    
    
    // Event helpers
    FB_JSAPI_EVENT(test, 0, ());
    FB_JSAPI_EVENT(echo, 2, (const FB::variant&, const int));

    // Method test-event
    void testEvent();

private:
    ibrowserWeakPtr m_plugin;
    FB::BrowserHostPtr m_host;

    std::string m_testString;
    std::string uploadFileDir = "/Downloads";
    
    idevice_t device = NULL;
    instproxy_client_t instproxy_client = NULL;
    lockdownd_client_t 	lockdownd_client = NULL;
    sbservices_client_t sbservices_client = NULL;
    afc_client_t afc_client = NULL;
    
    
    bool init(F_SUCC,F_ERRO);
    void clean();
    
};

#endif // H_ibrowserAPI

