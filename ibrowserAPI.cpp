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

extern "C"{
#include <curl/curl.h>
#include <curl/easy.h>
}

#define RESULT_BUFF_SIZE 1024000
#define CLIENT_LABEL "ibrowser"

bool ibrowserAPI::init(F_SUCC,F_ERRO)
{
    
    lockdownd_service_descriptor_t service = NULL;
    
    if (NULL == device)
    {
        if (IDEVICE_E_SUCCESS != idevice_new(&device, NULL)) {
            ERRO("idevice_new");
        }
        idevice_set_debug_level(1);
    }
    
    if (NULL == lockdownd_client)
    {
        if (LOCKDOWN_E_SUCCESS != (lockdownd_client_new_with_handshake(device, &lockdownd_client, CLIENT_LABEL))) {
            ERRO("lockdownd_client_new_with_handshake");
        }
    }
    
    if (NULL == instproxy_client)
    {
        if(LOCKDOWN_E_SUCCESS != (lockdownd_start_service(lockdownd_client,"com.apple.mobile.installation_proxy",&service) || !service->port))
        {
            ERRO("lockdownd_start_service com.apple.mobile.installation_proxy");
        }
        
        if(INSTPROXY_E_SUCCESS != instproxy_client_new(device,service,&instproxy_client) )
        {
            ERRO("instproxy_client_new");
        }
    }
    
    
    if (NULL == afc_client)
    {
        if(LOCKDOWN_E_SUCCESS != (lockdownd_start_service(lockdownd_client,"com.apple.afc",&service)) || !service->port)
        {
            ERRO("lockdownd_start_service com.apple.afc");
        }
        
        if (afc_client_new(device, service, &afc_client) != AFC_E_SUCCESS) {
            ERRO("afc_client_new");
        }
    }
    
    if (NULL == sbservices_client)
    {
        if(LOCKDOWN_E_SUCCESS != (lockdownd_start_service(lockdownd_client,"com.apple.springboardservices",&service)) || !service->port)
        {
            ERRO("lockdownd_start_service com.apple.springboardservices");
        }
        
        if (sbservices_client_new(device, service, &sbservices_client) != AFC_E_SUCCESS) {
            ERRO("sbservices_client_new");
        }
    }
    
    return true;

}

void ibrowserAPI::clean()
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

FB::variant ibrowserAPI::setIdeviceEventCallback(const FB::JSObjectPtr& callback,F_ADD)
{
    Callback *cb = new Callback();
    cb->set("callback",callback);
    
    if(IDEVICE_E_SUCCESS != idevice_event_subscribe(&ibrowserAPI::ideviceEventCallback, (void *)cb))
        ERRO("idevice_event_subscribe");
    
    return true;
}

void ibrowserAPI::ideviceEventCallback(const idevice_event_t *event, void *user_data)
{
    Callback *cb = (Callback *)user_data;
    FB::JSObjectPtr callback=cb->get("callback");
    if(callback && callback->isValid())
        callback->InvokeAsync("", FB::variant_list_of( event->event ));
}


FB::variant ibrowserAPI::getDeviceInfo(const std::vector<std::string>& domains,F_ADD)
{
    
    THREAD(&ibrowserAPI::getDeviceInfo, domains);
    
    std::vector<std::string> result;
    
    for(int i=0;i<domains.size();i++)
    {
        std::string domain = domains[i];
        plist_t node = NULL;
        int ret = 0;
        if(LOCKDOWN_E_SUCCESS != (ret = lockdownd_get_value(lockdownd_client, domain.empty()?NULL:domain.c_str(), NULL, &node)) ) {
            ERRO("lockdownd_get_value");
        }
        
        char *xml_doc=NULL;
        uint32_t xml_length;
        plist_to_xml(node, &xml_doc, &xml_length);
        plist_free(node);
        
        result.insert(result.end(),std::string(xml_doc));
        free(xml_doc);
    }

    SUCC(result);
    
    return result;
}

FB::variant ibrowserAPI::getAppList(F_ADD)
{
    THREAD(&ibrowserAPI::getAppList);
    
    char *xml_doc=NULL;
    uint32_t xml_length=0;
    plist_t node = NULL;
    
    plist_t client_opts = instproxy_client_options_new();
    instproxy_client_options_add(client_opts, "ApplicationType", "User", NULL);
    
    if(INSTPROXY_E_SUCCESS != instproxy_browse(instproxy_client,client_opts,&node))
    {
        ERRO("instproxy_browse");
    }
    
    instproxy_client_options_free(client_opts);
    
    plist_to_xml(node, &xml_doc, &xml_length);
    plist_free(node);
    
    SUCC(xml_doc);
    
    return xml_doc;
}

FB::variant ibrowserAPI::getSbservicesIconPngdata(const std::string& bundleId,F_ADD)
{
    THREAD(&ibrowserAPI::getSbservicesIconPngdata, bundleId);
    
    if(bundleId.empty())
        return NULL;
    char *data = NULL;
    uint64_t size = 0;
    if (SBSERVICES_E_SUCCESS != sbservices_get_icon_pngdata(sbservices_client,bundleId.c_str(),&data,&size))
    {
        ERRO("get_sbservices_icon_pngdata error");
    }
    char *base64 = base64encode(data,size);
    free(data);
    
    SUCC(base64);
    
    return base64;
    
}

#ifdef WIN32
#else
#include <cocoa/Cocoa.h>
FB::variant ibrowserAPI::openDialog(F_ADD)
{
    //有安装在进行时, 文件框会卡住
    //THREAD(&ibrowserAPI::openDialog);
    
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

FB::variant ibrowserAPI::uploadFile(const std::string& fileName, const boost::optional<FB::JSObjectPtr>& pcb, F_ADD)
{

    if(fileName.empty())
        return false;
    
    THREAD(&ibrowserAPI::uploadFile,fileName,pcb);
    
    const char *file_name=fileName.c_str();
    
    char target_file[1024];
    sprintf(target_file, "%s/%s",uploadFileDir.c_str(), basename((char *)file_name));
    uint64_t target_file_handle = 0;
    if (AFC_E_SUCCESS != afc_file_open(afc_client, target_file, AFC_FOPEN_WRONLY, &target_file_handle)){
        ERRO("afc_file_open error!");
    }
    
    FILE *file_handle= fopen(file_name, "r");
    if (!file_handle)
    {
        afc_remove_path(afc_client, target_file);
        ERRO("open file error!");
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
        }
        
        memset(read_buf, 0, read_buf_size);
        
        doneSize = doneSize + bytes_read;
        if(pcb && fileSize > 0)
            (*pcb)->InvokeAsync("", FB::variant_list_of((double)doneSize/fileSize));
    }
    
    SUCC(target_file);
    
    return target_file;
}

FB::variant ibrowserAPI::installPackage(const std::string& fileName, const boost::optional<FB::JSObjectPtr>& pcb, F_ADD)
{
    if(fileName.empty())
        return false;
    
    THREAD(&ibrowserAPI::installPackage,fileName,pcb);

    int ret=0;
    Callback *cb = new Callback();
    cb->set("pcb",*pcb);
    cb->set("scb",*scb);
    cb->set("ecb",*ecb);
    
    while (INSTPROXY_E_OP_IN_PROGRESS == (ret = instproxy_install(instproxy_client, fileName.c_str(), NULL, &ibrowserAPI::installCallback, (void*)cb)))
    {
        log("installPackage %s sleep...",fileName.c_str());
        sleep(1);
    }
    
    if(INSTPROXY_E_SUCCESS != ret)
    {
        ERRO("instproxy_install");
    }
    
    return true;
    
}

FB::variant ibrowserAPI::uninstallPackage(const std::string& fileName, const boost::optional<FB::JSObjectPtr>& pcb, F_ADD)
{
    if(fileName.empty())
        return false;
    
    THREAD(&ibrowserAPI::uninstallPackage,fileName,pcb);
    
    int ret=0;
    Callback *cb = new Callback();
    cb->set("pcb",*pcb);
    cb->set("scb",*scb);
    cb->set("ecb",*ecb);
    
    while (INSTPROXY_E_OP_IN_PROGRESS == (ret = instproxy_uninstall(instproxy_client, fileName.c_str(), NULL, &ibrowserAPI::installCallback, (void*)cb)))
    {
        log("uninstallPackage %s sleep...",fileName.c_str());
        sleep(1);
    }
    
    if(INSTPROXY_E_SUCCESS != ret)
    {
        ERRO("instproxy_uninstall");
    }
    
    return true;
    
}

void ibrowserAPI::installCallback(const char *operation, plist_t status, void *user_data) {
    char *xml_doc=NULL;
    uint32_t xml_length;
    
    Callback *cb = (Callback *)user_data;
    
    FB::JSObjectPtr pcb = cb->get("pcb");
    FB::JSObjectPtr scb = cb->get("scb");
    FB::JSObjectPtr ecb = cb->get("ecb");
    
    if(pcb)
    {
        plist_to_xml(status, &xml_doc, &xml_length);
        pcb->InvokeAsync("", FB::variant_list_of(xml_doc));
        free(xml_doc);
    }
    
    plist_dict_iter it = NULL;
	char* key = NULL;
    char*   s = NULL;
	plist_t subnode = NULL;
	plist_dict_new_iter(status, &it);
	plist_dict_next_item(status, it, &key, &subnode);
	while (subnode)
	{
		if(strcmp(key, "Error") == 0)
        {
            plist_get_string_val(subnode, &s);
            if(ecb)
                ecb->InvokeAsync("", FB::variant_list_of(s));
            
        }else if(strcmp(key, "Status") == 0){
            plist_get_string_val(subnode, &s);
            if(strcmp(s, "Complete") == 0){
                if(scb)
                    scb->InvokeAsync("", FB::variant_list_of(NULL));
            }
        }
		plist_dict_next_item(status, it, &key, &subnode);
        
	}

    if(key)
        free(key);
    if(s)
        free(s);
	free(it);
    return;
}

FB::variant ibrowserAPI::downloadFile(const std::string& url,const std::string& filename,const boost::optional<FB::JSObjectPtr>& pcb, F_ADD)
{
    if(url.empty() || filename.empty())
        ERRO("url or filename is empty!");
    
    THREAD(&ibrowserAPI::downloadFile,url,filename,pcb);
    
    curl_global_init(CURL_GLOBAL_ALL);
    
    double fileSize = -1;
    CURL *fileSizeCurl = curl_easy_init();
    curl_easy_setopt(fileSizeCurl, CURLOPT_URL,url.c_str());
    curl_easy_setopt(fileSizeCurl, CURLOPT_HEADER, 1);
    curl_easy_setopt(fileSizeCurl, CURLOPT_NOBODY, 1);
    curl_easy_perform(fileSizeCurl);
    curl_easy_getinfo(fileSizeCurl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &fileSize);
    curl_easy_cleanup(fileSizeCurl);
    
    log("download file size:%.0f",fileSize);
    if(0 >= fileSize)
    {
        ERRO("get file size error");
    }
    
    char tmpName[1024];
    sprintf(tmpName,"%s/%s",dirname(tmpnam(NULL)),filename.c_str());
    
    log("start download, save file:%s, use %d threads",tmpName,this->downloadThreads);
    
    double partSize=fileSize/this->downloadThreads;
    pthread_t threads[this->downloadThreads];
    std::vector<double> counter;
    for(int i=0;i<this->downloadThreads;i++)
    {
        double start=partSize*i;
        double end=0;
        if(i+1<this->downloadThreads)
            end=partSize*(i+1)-1;
        else
            end=fileSize;
        FILE *tmpFile=fopen(tmpName,"wb+");
        if(!tmpFile)
            ERRO("create tmp file error");
        fseek(tmpFile,start,SEEK_SET);
        counter.push_back(0);
        
        log("threadid:%d,range:%.0f-%.0f",i,start,end);
        DownloadConfig *cfg=new DownloadConfig(i,url,tmpFile,*pcb,fileSize,start,end,&counter);
        pthread_create(&threads[i],NULL,&ibrowserAPI::downloadThread,cfg);
    }
    
    for(int i=0; i< this->downloadThreads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    curl_global_cleanup();
    
    double downloadSize=0;
    int len=counter.size();
    for(int i=0;i<len;i++)
    {
        downloadSize+=counter.at(i);
    }
    //downloadSize+=1;
    log("downloadSize:%.0f",downloadSize);
    if(downloadSize == fileSize)
    {
        SUCC(tmpName);
    }else{
        ERRO("download error");
    }
    
    
    return true;
}

void *ibrowserAPI::downloadThread(void *data)
{
    DownloadConfig *cfg=(DownloadConfig *)data;
    CURL *curl;
    long http_code = 0;
    while((curl = curl_easy_init()))
    {
        char range[512];
        sprintf(range,"%.0f-%.0f",cfg->start,cfg->end);
        curl_easy_setopt(curl, CURLOPT_URL,cfg->url.c_str());
        curl_easy_setopt(curl, CURLOPT_RANGE,range);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &ibrowserAPI::downloadWrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, cfg);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &ibrowserAPI::downloadProgress);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, cfg);
        if(CURLE_OK != curl_easy_perform(curl))
            break;

        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
        curl_easy_cleanup(curl);
        
        log("threadid:%d,http_code:%ld",cfg->id, http_code);
        if(http_code != 206 && http_code != 200 )
        {
            sleep(3);
            if(cfg->getDownloadedSize() <= 0)
                break;  //其他线程也无法下载数据, 此资源不可下载;
            log("anther thread down size:%.0f, try again...",cfg->getDownloadedSize());
        }else{
            //下载完成
            break;
        }
    }
    
    if(cfg->stream)
        fclose(cfg->stream);
    return NULL;
}

int ibrowserAPI::downloadProgress(void* ptr, double rDlTotal, double rDlNow, double rUlTotal, double rUlNow)
{
    DownloadConfig *cfg=(DownloadConfig *)ptr;
    cfg->counter->at(cfg->id)=rDlNow;
    double done=cfg->getDownloadedSize();
    if(cfg->pcb)
        cfg->pcb->InvokeAsync("", FB::variant_list_of(cfg->filesize)(done));
    return 0;
}

int ibrowserAPI::downloadWrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
    DownloadConfig *cfg=(DownloadConfig *)stream;
    return fwrite(buffer, size, nmemb, cfg->stream);
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
