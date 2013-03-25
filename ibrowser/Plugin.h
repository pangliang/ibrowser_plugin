//
//  Plugin.h
//  ibrowser
//
//  Created by liangwei on 13-3-25.
//  Copyright (c) 2013年 liangwei. All rights reserved.
//

#ifndef __ibrowser__Plugin__
#define __ibrowser__Plugin__

#include <WebKit/npapi.h>
#include <WebKit/npfunctions.h>
#include <WebKit/npruntime.h>
#include <string>
#include <unordered_map>

extern "C"{
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <libimobiledevice/installation_proxy.h>
#include <libimobiledevice/afc.h>
#include <libimobiledevice/sbservices.h>
#include <libimobiledevice/service.h>
}

using namespace std;

#define RESULT_BUFF_SIZE 1024000
#define CLIENT_LABEL "ibrowser"

#define ERRO(msg)   \
    do{ \
        printf("error:%s\n",msg); \
        clean(); \
        throw msg; \
    }while(0)

class Callback{
public:
    void set(string fn,NPObject* op)
    {
        op->referenceCount++;
        list[fn]=op;
    }
    
    void invoke(string fn,const NPVariant *args, uint32_t argCount)
    {
        NPObject* obj= list[fn];
        NPVariant *rs=(NPVariant *)malloc(sizeof(NPVariant));
        obj->_class->invokeDefault(obj,args,argCount,rs);
    }
    
    ~Callback(){
        printf("~Callback\n");
        
        for(auto it=list.begin();it!=list.end();it++)
        {
            ((NPObject*)it->second)->referenceCount--;
            printf("\t%s,referenceCount:%d\n",((string)(it->first)).c_str(),((NPObject*)it->second)->referenceCount);
        }
        
    }
    
private:
    unordered_map<string,NPObject*> list;
};

class Plugin
{
    typedef bool (Plugin::*fun) (const NPVariant *args, uint32_t argCount, NPVariant *result );
public:
    Plugin(NPNetscapeFuncs* browser):browser(browser){
        registerMethod("setIdeviceEventCallback",&Plugin::setIdeviceEventCallback);
        registerMethod("getDeviceInfo",&Plugin::getDeviceInfo);
    }
    void registerMethod(string,fun);
    bool hasMethod(NPObject *obj, NPIdentifier methodName);
    bool invoke(NPObject *obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result);
    
    bool getDeviceInfo(const NPVariant *args, uint32_t argCount, NPVariant *result);
    bool setIdeviceEventCallback(const NPVariant *args, uint32_t argCount, NPVariant *result);
    static void ideviceEventCallback(const idevice_event_t *event, void *user_data);
                       
private:
    NPNetscapeFuncs* browser;
    unordered_map<string, fun> INVOKE_FUNCTIONS ;
    
    bool init();
    void clean();
    
    idevice_t device ;
    instproxy_client_t instproxy_client ;
    lockdownd_client_t 	lockdownd_client ;
    sbservices_client_t sbservices_client;
    afc_client_t afc_client;
};

#endif /* defined(__ibrowser__Plugin__) */
