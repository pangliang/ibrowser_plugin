#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <libimobiledevice/installation_proxy.h>

#include "functions.h"


int get_device_id(char *args, char *result)
{
    int i;
    char **dev_list;
	if (idevice_get_device_list(&dev_list, &i) < 0) {
        fprintf(stderr, "ERROR: Unable to retrieve device list!\n");
        return -1;
    }
    strcpy(result, dev_list[0]);
    idevice_device_list_free(dev_list);
    return i;
} 


int get_app_list(char *args, char *result)
{


    idevice_t phone = NULL;
    instproxy_client_t instproxy_client = NULL;
    lockdownd_client_t 	lockdownd_client = NULL;
    int ret = 0;
    const char * label="ibrowser";
    uint16_t port = 0;
    
    ret = idevice_new(&phone, NULL);
    if (ret != IDEVICE_E_SUCCESS) {
        printf("No device found, is it plugged in?\n");
        return -1;
    }
    
    if (LOCKDOWN_E_SUCCESS != (lockdownd_client_new_with_handshake(phone, &lockdownd_client, label))) {
		idevice_free(phone);
		return -1;
	}
    
    if(LOCKDOWN_E_SUCCESS != (lockdownd_start_service(lockdownd_client,"com.apple.mobile.installation_proxy",&port)))
    {
        printf("lockdownd_start_service error");
        return -1;
    }
    
    if((ret=instproxy_client_new(phone,port,&instproxy_client)) != INSTPROXY_E_SUCCESS)
    {
        printf("instproxy_client_new error");
        return -1;
    }

    plist_t client_opts = instproxy_client_options_new();
    instproxy_client_options_add(client_opts, "ApplicationType", "User", NULL);

    plist_t apps;
    
    if((ret=instproxy_browse(instproxy_client,client_opts,&apps)) != INSTPROXY_E_SUCCESS)
    {
        printf("instproxy_browse error");
        return -1;
    }

    printf("plist_get_node_type:%d\n",plist_get_node_type(apps));

    char *xml_doc;
	uint32_t xml_length=0;
    plist_to_xml(apps, &xml_doc, &xml_length);
    //strncpy(result, xml_doc,xml_length>RESULT_BUFF_SIZE?RESULT_BUFF_SIZE:xml_length);
    printf("xml_length:%d\n",xml_length);
    puts(xml_doc);

    printf("Total: %d apps\n", plist_array_get_size(apps));
    uint32_t i = 0;
    for (i = 0; i < plist_array_get_size(apps); i++) {
        plist_t app = plist_array_get_item(apps, i);
        plist_t p_appid =
                plist_dict_get_item(app, "CFBundleIdentifier");
        char *s_appid = NULL;
        char *s_dispName = NULL;
        char *s_version = NULL;
        plist_t dispName =
                plist_dict_get_item(app, "CFBundleDisplayName");
        plist_t version = plist_dict_get_item(app, "CFBundleVersion");

        if (p_appid) {
            plist_get_string_val(p_appid, &s_appid);
        }
        if (!s_appid) {
            fprintf(stderr, "ERROR: Failed to get APPID!\n");
            break;
        }

        if (dispName) {
            plist_get_string_val(dispName, &s_dispName);
        }
        if (version) {
            plist_get_string_val(version, &s_version);
        }

        if (!s_dispName) {
            s_dispName = strdup(s_appid);
        }
        if (s_version) {
            printf("%s - %s %s\n", s_appid, s_dispName, s_version);
            free(s_version);
        } else {
            printf("%s - %s\n", s_appid, s_dispName);
        }
        free(s_dispName);
        free(s_appid);
    }
    plist_free(apps);


    return xml_length;
}
