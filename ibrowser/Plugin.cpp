//
//  Plugin.cpp
//  ibrowser
//
//  Created by liangwei on 13-3-25.
//  Copyright (c) 2013年 liangwei. All rights reserved.
//

#include "Plugin.h"

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

bool Plugin::call(NPObject *obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    bool ret=false;
    NPUTF8 *name = browser->utf8fromidentifier(methodName);
    if(NULL == INVOKE_FUNCTIONS[name])
    {
        ret=false;
    }else{
        fun f=this->INVOKE_FUNCTIONS[name];
        f(args, argCount, result);
        ret=true;
    }

    browser->memfree(name);
    return ret;
}