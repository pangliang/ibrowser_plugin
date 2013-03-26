//
//  IbrowserAPI.h
//  ibrowser
//
//  Created by liangwei on 13-3-26.
//  Copyright (c) 2013年 liangwei. All rights reserved.
//

#ifndef __ibrowser__IbrowserAPI__
#define __ibrowser__IbrowserAPI__

#include "Plugin.h"

extern "C"{
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <libimobiledevice/installation_proxy.h>
#include <libimobiledevice/afc.h>
#include <libimobiledevice/sbservices.h>
#include <libimobiledevice/service.h>
}

#define RESULT_BUFF_SIZE 1024000
#define CLIENT_LABEL "ibrowser"

#define ERRO(msg)   \
do{ \
printf("error:%s\n",msg); \
clean(); \
}while(0)

class IbrowserAPI : public Plugin
{
public:
    IbrowserAPI(NPP npp,NPNetscapeFuncs* browser):Plugin(npp,browser){
        registerMethod("setIdeviceEventCallback",boost::bind(&IbrowserAPI::setIdeviceEventCallback,this,_1,_2,_3));
        registerMethod("getDeviceInfo",boost::bind(&IbrowserAPI::getDeviceInfo,this,_1,_2,_3));
    }
    
    bool getDeviceInfo(const NPVariant *args, uint32_t argCount, NPVariant *result);
    bool setIdeviceEventCallback(const NPVariant *args, uint32_t argCount, NPVariant *result);
    static void ideviceEventCallback(const idevice_event_t *event, void *user_data);
private:
    
    bool init();
    void clean();
    
    idevice_t device ;
    instproxy_client_t instproxy_client ;
    lockdownd_client_t 	lockdownd_client ;
    sbservices_client_t sbservices_client;
    afc_client_t afc_client;
};

#endif /* defined(__ibrowser__IbrowserAPI__) */
