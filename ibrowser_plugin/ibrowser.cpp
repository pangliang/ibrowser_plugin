//
//  ibrowser.c
//  ibrowser_plugin
//
//  Created by liangwei on 13-1-31.
//  Copyright (c) 2013年 liangwei. All rights reserved.
//

#import <WebKit/npapi.h>
#import <WebKit/npfunctions.h>
#import <WebKit/npruntime.h>

#include "functions.h"
 
static NPNetscapeFuncs* browser=NULL;

extern "C"{

//Mach-o entry points,浏览器和创建交流的最上层的接口
NPError NP_Initialize(NPNetscapeFuncs *browserFuncs);
NPError NP_GetEntryPoints(NPPluginFuncs *pluginFuncs);
void NP_Shutdown(void);

//NPP Functions
NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved);
NPError NPP_Destroy(NPP instance, NPSavedData** save);
NPError NPP_SetWindow(NPP instance, NPWindow* window);
NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype);
NPError NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason);
int32_t NPP_WriteReady(NPP instance, NPStream* stream);
int32_t NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer);
void NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname);
void NPP_Print(NPP instance, NPPrint* platformPrint);
int16_t NPP_HandleEvent(NPP instance, void* event);
void NPP_URLNotify(NPP instance, const char* URL, NPReason reason, void* notifyData);
NPError NPP_GetValue(NPP instance, NPPVariable variable, void *value);
NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value);

//Functions for scriptablePluginClass
bool pluginHasMethod(NPObject *obj, NPIdentifier methodName);
bool pluginInvoke(NPObject *obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result);
bool hasProperty(NPObject *obj, NPIdentifier propertyName);
bool getProperty(NPObject *obj, NPIdentifier propertyName, NPVariant *result);
////////////////////////////////////

}
static struct NPClass scriptablePluginClass = {
    NP_CLASS_STRUCT_VERSION,
    NULL,
    NULL,
    NULL,
    pluginHasMethod,
    pluginInvoke,
    NULL,
    hasProperty,
    getProperty,
    NULL,
    NULL,
};

//接口的实现
NPError NP_Initialize(NPNetscapeFuncs* browserFuncs)
{
    browser = browserFuncs;
    return NPERR_NO_ERROR;
}

NPError NP_GetEntryPoints(NPPluginFuncs* pluginFuncs)
{
    pluginFuncs->version = 11;
    pluginFuncs->size = sizeof(pluginFuncs);
    pluginFuncs->newp = NPP_New;
    pluginFuncs->destroy = NPP_Destroy;
    pluginFuncs->setwindow = NPP_SetWindow;
    pluginFuncs->newstream = NPP_NewStream;
    pluginFuncs->destroystream = NPP_DestroyStream;
    pluginFuncs->asfile = NPP_StreamAsFile;
    pluginFuncs->writeready = NPP_WriteReady;
    pluginFuncs->write = (NPP_WriteProcPtr)NPP_Write;
    pluginFuncs->print = NPP_Print;
    pluginFuncs->event = NPP_HandleEvent;
    pluginFuncs->urlnotify = NPP_URLNotify;
    pluginFuncs->getvalue = NPP_GetValue;
    pluginFuncs->setvalue = NPP_SetValue;
    
    return NPERR_NO_ERROR;
}


void NP_Shutdown(void)
{
    
}

#define STRINGZ_TO_NPVARIANT(_val, _v)                                        \
NP_BEGIN_MACRO                                                                \
(_v).type = NPVariantType_String;                                         \
NPString str = { _val, (uint32_t)(strlen(_val)) };                          \
(_v).value.stringValue = str;                                             \
NP_END_MACRO

bool pluginHasMethod(NPObject *obj, NPIdentifier methodName) {
    // This function will be called when we invoke method on this plugin elements.
    NPUTF8 *name = browser->utf8fromidentifier(methodName);
    bool result = !(NULL == INVOKE_FUNCTIONS[name]);
    browser->memfree(name);
    return result;
}

bool pluginInvoke(NPObject *obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result) {

    bool ret=false;
    char **argsList = NULL;
    NPUTF8 *name = browser->utf8fromidentifier(methodName);

    if(NULL == INVOKE_FUNCTIONS[name])
    {
        ret=false;
    }else{
        init(false);

        if(argCount > 0)
        {
            argsList = new char*[argCount];
            for (int i=0;i<argCount;i++)
            {
                NPString npString = NPVARIANT_TO_STRING(args[0]);
                argsList[i]=(char *) malloc(npString.UTF8Length+1);
                memset((void *) argsList[i], 0, npString.UTF8Length+1);
                memcpy((void *) argsList[i], (void *)npString.UTF8Characters, npString.UTF8Length);
            }
        }

        //每次都要分配内存
        char *buff = (char*)browser->memalloc(RESULT_BUFF_SIZE);
        memset(buff, 0, RESULT_BUFF_SIZE);

        if (INVOKE_FUNCTIONS[name](argsList,argCount, buff) < 0)
        {
            printf("call again !\n");
            init(true);
            memset(buff, 0, RESULT_BUFF_SIZE);
            INVOKE_FUNCTIONS[name](argsList,argCount, buff);
        }
        STRINGZ_TO_NPVARIANT(buff,*result);
        ret=true;

        if(argCount > 0)
        {
            for (int i=0;i<argCount;i++)
            {
                free(argsList[i]);
            }
            delete []argsList;
        }
    }

    browser->memfree(name);
    return ret;

}

bool hasProperty(NPObject *obj, NPIdentifier propertyName) {
    return false;
}

bool getProperty(NPObject *obj, NPIdentifier propertyName, NPVariant *result) {
    return false;
}



//NPP Functions Implements
NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved)
{
    if(!instance->pdata) {
        instance->pdata = browser->createobject(instance, &scriptablePluginClass);
    }

    // Ask the browser if it supports the CoreGraphics drawing model
    NPBool supportsCoreGraphics;
    if (browser->getvalue(instance, NPNVsupportsCoreGraphicsBool, &supportsCoreGraphics) != NPERR_NO_ERROR)
        supportsCoreGraphics = FALSE;
    
    if (!supportsCoreGraphics)
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    
    // If the browser supports the CoreGraphics drawing model, enable it.
    browser->setvalue(instance, NPPVpluginDrawingModel, (void *)NPDrawingModelCoreGraphics);
    
    browser->setvalue(instance,
                      NPPVpluginEventModel,
                      (void *)NPEventModelCocoa);

    return NPERR_NO_ERROR;
}

NPError NPP_Destroy(NPP instance, NPSavedData** save)
{
    
    // If we created a plugin instance, we'll destroy and clean it up.
    NPObject *pluginInstance=(NPObject *)instance->pdata;
    if(!pluginInstance) {
        browser->releaseobject(pluginInstance);
        pluginInstance = NULL;
    }
    
    return NPERR_NO_ERROR;
}

NPError NPP_SetWindow(NPP instance, NPWindow* window)
{
    return NPERR_NO_ERROR;
}


NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype)
{
    *stype = NP_ASFILEONLY;
    return NPERR_NO_ERROR;
}

NPError NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
    return NPERR_NO_ERROR;
}

int32_t NPP_WriteReady(NPP instance, NPStream* stream)
{
    return 0;
}

int32_t NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer)
{
    return 0;
}

void NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname)
{
}

void NPP_Print(NPP instance, NPPrint* platformPrint)
{
    
}


int16_t NPP_HandleEvent(NPP instance, void* event)
{
    return 0;
}

void NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
    
}

NPError NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    if (variable == NPPVpluginScriptableNPObject) {
        void **v = (void **)value;
        NPObject *obj = (NPObject *)instance->pdata;
        
        if (obj)
           browser->retainobject(obj);
        
        *v = obj;
        return NPERR_NO_ERROR;
    }
    return NPERR_GENERIC_ERROR;
}

NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
    return NPERR_GENERIC_ERROR;
}

