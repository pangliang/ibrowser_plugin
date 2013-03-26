//
//  Plugin.cpp
//  ibrowser
//
//  Created by liangwei on 13-3-25.
//  Copyright (c) 2013年 liangwei. All rights reserved.
//

#include "Plugin.h"

bool Plugin::init()
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

void Plugin::clean()
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

void Plugin::ideviceEventCallback(const idevice_event_t *event, void *user_data)
{
    Callback *cb = (Callback *)user_data;
    NPVariant *args=new NPVariant[1];
    INT32_TO_NPVARIANT(event->event,args[0]);
    cb->invoke("callback",args,1);
    delete cb;
}

bool Plugin::setIdeviceEventCallback(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    Callback *cb = new Callback();
    cb->set("callback",NPVARIANT_TO_OBJECT(args[0]));
    if(IDEVICE_E_SUCCESS != idevice_event_subscribe(&Plugin::ideviceEventCallback, (void *)cb))
        ERRO("idevice_event_subscribe");
    return true;
}

bool Plugin::getDeviceInfo(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    NPObject* obj=NPVARIANT_TO_OBJECT(args[0]);
    NPVariant value;
    obj->_class->getProperty(obj,browser->getstringidentifier("1"),&value);
    printf("=======getDeviceInfo,%d,%d,%s\n",argCount,value.type,NPVARIANT_TO_STRING(value).UTF8Characters);
    return TRUE;
}

void Plugin::registerMethod(string methodNmae,fun f)
{
    INVOKE_FUNCTIONS[methodNmae]=f;
}

bool Plugin::hasMethod(NPObject *obj,NPIdentifier methodName) {
    const char *name = browser->utf8fromidentifier(methodName);
    bool result = INVOKE_FUNCTIONS[name]?TRUE:FALSE;
    browser->memfree((void*)name);
    return result;
}

bool Plugin::invoke(NPObject *obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    bool ret=false;
    NPUTF8 *name = browser->utf8fromidentifier(methodName);
    if(NULL == INVOKE_FUNCTIONS[name])
    {
        ret=false;
    }else{
        
        (this->*INVOKE_FUNCTIONS[name])(args,argCount, result);
        printf("call %s succ\n",name);
        
        ret=true;
    }

    browser->memfree(name);
    return ret;
}