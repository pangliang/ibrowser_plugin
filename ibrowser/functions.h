//
//  functions.h
//  ibrowser
//
//  Created by liangwei on 13-3-25.
//  Copyright (c) 2013年 liangwei. All rights reserved.
//

#ifndef ibrowser_functions_h
#define ibrowser_functions_h

#include <string>
#include <unordered_map>
#import <WebKit/npruntime.h>
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
    void set(const char* fn,NPObject* op)
    {
        list[fn]=op;
    }
    NPObject* get(const char* fn)
    {
        return list[fn];
    }
    
private:
    unordered_map<const char*,NPObject*> list;
};

bool init();
bool clean();
bool setIdeviceEventCallback(NPObject *obj,const NPVariant *args, uint32_t argCount, NPVariant *result);

static unordered_map<string, bool (*) (NPObject *obj, const NPVariant *args, uint32_t argCount, NPVariant *result )> INVOKE_FUNCTIONS ={
    { "setIdeviceEventCallback", setIdeviceEventCallback},
};

#endif
