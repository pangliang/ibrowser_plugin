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
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <string>
#include <map>

#include "NPObjectAPI.h"

using namespace std;

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
    map<string,NPObject*> list;
};
typedef boost::function< bool(const NPVariant *args, uint32_t argCount, NPVariant *result )> fun;
class Plugin
{
    
public:
    Plugin(NPP npp,NPNetscapeFuncs* browser):npp(npp),browser(browser){
        
    }
    void registerMethod(string,fun);
    bool hasMethod(NPObject *obj, NPIdentifier methodName);
    bool call(NPObject *obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result);
    
                       
protected:
    NPP npp;
    NPNetscapeFuncs* browser;
    map<string, fun> INVOKE_FUNCTIONS ;
    
};

#endif /* defined(__ibrowser__Plugin__) */
