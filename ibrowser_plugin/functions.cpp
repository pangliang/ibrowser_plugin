#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <libimobiledevice/installation_proxy.h>
#include <libimobiledevice/afc.h>
#include <libimobiledevice/sbservices.h>
#include "functions.h"
#include "base64.h"

idevice_t device = NULL;
instproxy_client_t instproxy_client = NULL;
lockdownd_client_t 	lockdownd_client = NULL;
sbservices_client_t sbservices_client = NULL;
afc_client_t afc_client = NULL;

bool init(bool reconnect)
{
    if (true == reconnect)
    {
        if (NULL != device)
        {
            idevice_free(device);
            device = NULL;
        }

        if (NULL != lockdownd_client)
        {
            lockdownd_client_free(lockdownd_client);
            lockdownd_client = NULL;
        }
    }

    if (NULL == device)
    {
        if (IDEVICE_E_SUCCESS != idevice_new(&device, NULL)) {
            printf("No device found, is it plugged in?\n");
            return false;
        }
    }
    if (NULL == lockdownd_client)
    {
        if (LOCKDOWN_E_SUCCESS != (lockdownd_client_new_with_handshake(device, &lockdownd_client, CLIENT_LABEL))) {
            printf("lockdownd_client_new_with_handshake fail!\n");
            idevice_free(device);
            return false;
        }
    }

    return true;
}

int get_device_info(char **args, unsigned int argCount, char *result)
{
    const char *domain = NULL;
    if(argCount > 0)
        domain=args[0];

    plist_t node = NULL;
    if(LOCKDOWN_E_SUCCESS != lockdownd_get_value(lockdownd_client, domain, NULL, &node) ) {
        printf("ERROR: Unable to get_device_info, domain:%s!\n",domain);
        return -1;
    }

    char *xml_doc=NULL;
    uint32_t xml_length;
    plist_to_xml(node, &xml_doc, &xml_length);
    strncpy(result, xml_doc, xml_length);
    plist_free(node);

    return xml_length;
}

int get_app_list(char **args, uint32_t argCount, char *result)
{

    uint16_t port = 0;
    char *xml_doc=NULL;
    uint32_t xml_length=0;
    plist_t node = NULL;

    if (NULL == instproxy_client)
    {
        if(LOCKDOWN_E_SUCCESS != (lockdownd_start_service(lockdownd_client,"com.apple.mobile.installation_proxy",&port) || !port))
        {
            printf("lockdownd_start_service com.apple.mobile.installation_proxy error");
            return -1;
        }

        if(INSTPROXY_E_SUCCESS != instproxy_client_new(device,port,&instproxy_client) )
        {
            printf("instproxy_client_new error");
            return -1;
        }
    }

    plist_t client_opts = instproxy_client_options_new();
    instproxy_client_options_add(client_opts, "ApplicationType", "User", NULL);
    
    if(INSTPROXY_E_SUCCESS != instproxy_browse(instproxy_client,client_opts,&node))
    {
        instproxy_client_free(instproxy_client);
        instproxy_client = NULL;
        printf("instproxy_browse error\n");
        return -1;
    }

    instproxy_client_options_free(client_opts);

    plist_to_xml(node, &xml_doc, &xml_length);
    strncpy(result, xml_doc, xml_length);
    plist_free(node);

    return xml_length;
}

int read_file(char **args, uint32_t argCount, char *result)
{
    afc_client_t afc_client = NULL;
    uint16_t port = 0;
    const char *file_name = NULL;
    uint64_t file_handle = 0;

    if (argCount != 1)
    {
        return -1;
    }

    file_name =args[0];

    if (NULL == afc_client)
    {
        if(LOCKDOWN_E_SUCCESS != (lockdownd_start_service(lockdownd_client,"com.apple.afc2",&port)) || !port)
        {
            printf("lockdownd_start_service com.apple.afc error\n");
            return -1;
        }

        if (afc_client_new(device, port, &afc_client) != AFC_E_SUCCESS) {
            printf("Could not connect to AFC!\n");
            return -1;
        }
    }

    if (AFC_E_SUCCESS != afc_file_open(afc_client, file_name, AFC_FOPEN_RDONLY, &file_handle)){
        afc_client_free(afc_client);
        afc_client = NULL;
        printf("afc_file_open %s error!\n", file_name);
        return -1;
    }

    uint32_t bytes_read=0;
    if (AFC_E_SUCCESS != afc_file_read(afc_client, file_handle,result,RESULT_BUFF_SIZE,&bytes_read))
    {
        afc_client_free(afc_client);
        afc_client = NULL;
        printf("afc_file_read %s error!\n", file_name);
        return -1;
    }

    printf("%s",result);

    return bytes_read;
}

int upload_file(char **args, uint32_t argCount, char *result)
{
    uint16_t port = 0;
    char *file_name = NULL;

    if (argCount != 1)
    {
        return -1;
    }

    file_name =args[0];

    if (NULL == afc_client)
    {
        if(LOCKDOWN_E_SUCCESS != (lockdownd_start_service(lockdownd_client,"com.apple.afc",&port)) || !port)
        {
            printf("lockdownd_start_service com.apple.afc error\n");
            return -1;
        }

        if (afc_client_new(device, port, &afc_client) != AFC_E_SUCCESS) {
            printf("Could not connect to AFC!\n");
            return -1;
        }
    }

    char *target_file=result;
    sprintf(target_file, "/Downloads/%s",basename(file_name));
    uint64_t target_file_handle = 0;
    if (AFC_E_SUCCESS != afc_file_open(afc_client, target_file, AFC_FOPEN_WRONLY, &target_file_handle)){
        printf("afc_file_open %s error!\n", target_file);
        return -1;
    }

    FILE *file_handle= fopen(file_name, "r");
    if (!file_handle)
    {
        printf("open file %s error!\n",file_name);
        return -1;
    }

    int read_buf_size = 1024;
    char read_buf[read_buf_size];
    int bytes_read;
    uint32_t bytes_written = 0;
    while((bytes_read = fread(read_buf, 1, read_buf_size, file_handle)) >0 )
    {
        if (AFC_E_SUCCESS != afc_file_write(afc_client, target_file_handle, read_buf, bytes_read, &bytes_written) || bytes_read !=bytes_written)
        {
            printf("afc_file_write %s error!\n", target_file);
            return -1;
        }

        memset(read_buf, 0, read_buf_size);
    }

    return strlen(target_file);
}

void install_callback(const char *operation, plist_t status, void *user_data) {
    char *xml_doc=NULL;
    uint32_t xml_length;
    plist_to_xml(status, &xml_doc, &xml_length);
    printf("install_callback:%s\n%s\n",xml_doc);
}

int install_package(char **args, uint32_t argCount, char *result)
{
    uint16_t port = 0;
    char *xml_doc=NULL;
    uint32_t xml_length=0;
    plist_t node = NULL;

    char *pkg_path = NULL;

    if (argCount != 1)
    {
        return -1;
    }

    pkg_path = args[0];

    if (NULL == instproxy_client)
    {
        if(LOCKDOWN_E_SUCCESS != (lockdownd_start_service(lockdownd_client,"com.apple.mobile.installation_proxy",&port) || !port))
        {
            printf("lockdownd_start_service com.apple.mobile.installation_proxy error");
            return -1;
        }

        if(INSTPROXY_E_SUCCESS != instproxy_client_new(device,port,&instproxy_client) )
        {
            printf("instproxy_client_new error");
            return -1;
        }
    }

    if (INSTPROXY_E_SUCCESS != instproxy_install(instproxy_client, pkg_path, NULL, install_callback, NULL))
    {
        printf("instproxy_install %s error\n",pkg_path);
        return -1;
    }

    return 0;
}

int get_sbservices_icon_pngdata(char **args, uint32_t argCount, char *result)
{
    uint16_t port = 0;

    if (argCount != 1)
    {
        printf("use get_sbservices_icon_pngdata(bundle_id)\n");
        return -1;
    }

    const char *bundle_id =args[0];

    if (NULL == sbservices_client)
    {
        if(LOCKDOWN_E_SUCCESS != (lockdownd_start_service(lockdownd_client,"com.apple.springboardservices",&port)) || !port)
        {
            printf("lockdownd_start_service com.apple.springboardservices error\n");
            return -1;
        }

        if (sbservices_client_new(device, port, &sbservices_client) != AFC_E_SUCCESS) {
            printf("sbservices_client_new error!\n");
            return -1;
        }
    }

    char *data = NULL;
    uint64_t size = 0;
    if (SBSERVICES_E_SUCCESS != sbservices_get_icon_pngdata(sbservices_client,bundle_id,&data,&size))
    {
        printf("get_sbservices_icon_pngdata error\n");
        return -1;
    }

    int base64_len = base64encode(data,size,result);
    free(data);
    return base64_len;


}