#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <cocoa/Cocoa.h>
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

bool clean()
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

bool init()
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


void ideviceEventCallback(const idevice_event_t *event, void *user_data)
{
    Callback *cb = (Callback *)user_data;
    NPObject *callback=cb->get("callback");
    NPVariant *l=new NPVariant[1];
    INT32_TO_NPVARIANT(event->event,l[0]);
    NPVariant *result = (NPVariant*) malloc(sizeof(NPVariant));
    callback->_class->invoke(callback,(NPIdentifier)"",l,1,result);
    printf("===rs:%d\n",result->type);
}

bool setIdeviceEventCallback(NPObject *obj,const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    Callback *cb = new Callback();
    cb->set("callback",NPVARIANT_TO_OBJECT(args[0]));
    
    if(IDEVICE_E_SUCCESS != idevice_event_subscribe(&ideviceEventCallback, (void *)cb))
        ERRO("idevice_event_subscribe");
    return true;
}
