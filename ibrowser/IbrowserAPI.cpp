//
//  IbrowserAPI.cpp
//  ibrowser
//
//  Created by liangwei on 13-3-26.
//  Copyright (c) 2013年 liangwei. All rights reserved.
//

#include "IbrowserAPI.h"



bool IbrowserAPI::init()
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

void IbrowserAPI::clean()
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

void IbrowserAPI::ideviceEventCallback(const idevice_event_t *event, void *user_data)
{
    Callback *cb = (Callback *)user_data;
    NPVariant *args=new NPVariant[1];
    INT32_TO_NPVARIANT(event->event,args[0]);
    cb->invoke("callback",args,1);
    delete cb;
}

bool IbrowserAPI::setIdeviceEventCallback(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    Callback *cb = new Callback();
    cb->set("callback",NPVARIANT_TO_OBJECT(args[0]));
    if(IDEVICE_E_SUCCESS != idevice_event_subscribe(&IbrowserAPI::ideviceEventCallback, (void *)cb))
        ERRO("idevice_event_subscribe");
    return true;
}

bool IbrowserAPI::getDeviceInfo(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    init();
    NPObject* obj=NPVARIANT_TO_OBJECT(args[0]);
    NPVariant tmp;
    obj->_class->getProperty(obj,browser->getstringidentifier("length"),&tmp);

    double size = tmp.value.doubleValue;
    
    NPArray* array=new NPArray(npp,browser);

    for(int i=0;i<size;i++)
    {
        obj->_class->getProperty(obj,browser->getintidentifier(i),&tmp);
        std::string domain (tmp.value.stringValue.UTF8Characters,tmp.value.stringValue.UTF8Length);
        printf("=====%s\n",domain.c_str());
        plist_t node = NULL;
        int ret = 0;
        if(LOCKDOWN_E_SUCCESS != (ret = lockdownd_get_value(lockdownd_client, domain.empty()?NULL:domain.c_str(), NULL, &node)) ) {
            ERRO("lockdownd_get_value");
        }
        
        char *xml_doc=NULL;
        uint32_t xml_length;
        plist_to_xml(node, &xml_doc, &xml_length);
        plist_free(node);
        
        //array->push(xml_doc,xml_length);
        //free(xml_doc);
    }
    
    result->type=NPVariantType_Object;
    result->value.objectValue=array->getNPObject();
    return TRUE;
}





