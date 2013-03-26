//
//  NPObjectAPI.h
//  ibrowser
//
//  Created by liangwei on 13-3-26.
//  Copyright (c) 2013年 liangwei. All rights reserved.
//

#ifndef ibrowser_NPObjectAPI_h
#define ibrowser_NPObjectAPI_h

#include <WebKit/npapi.h>
#include <WebKit/npfunctions.h>
#include <WebKit/npruntime.h>

class NPObjectAPI
{
public:
    NPObjectAPI(NPP npp,NPNetscapeFuncs* browser):npp(npp),browser(browser){
        
    }
    
    ~NPObjectAPI()
    {
        obj->referenceCount--;
    }
protected:
    NPP npp;
    NPNetscapeFuncs* browser;
    NPObject* obj;
};

class NPArray:public NPObjectAPI
{
public:
    NPArray(NPP npp,NPNetscapeFuncs* browser):NPObjectAPI(npp,browser)
    {
        NPObject* window = NULL;
        browser->getvalue(npp, NPNVWindowNPObject, &window);
        
        NPVariant arrayVar;
        browser->invoke(npp, window, browser->getstringidentifier("Array"), NULL, 0, &arrayVar);
        obj = arrayVar.value.objectValue;
        obj->referenceCount++;
        browser->releasevariantvalue(&arrayVar);
    }
    
    NPObject* getNPObject()
    {
        return obj;
    }
    
    void push(NPVariant value)
    {
        NPVariant res;
        browser->invoke(npp, obj, browser->getstringidentifier("push"), &value, 1, &res);
        browser->releasevariantvalue(&res);
    }
    
    void push(const char *c,uint32_t len)
    {
        NPVariant value;
        char *s=(char *)browser->memalloc(len);
        memcpy(s, c, len);
        STRINGN_TO_NPVARIANT(c,len,value);
        this->push(value);
    }
private:
};

#endif
