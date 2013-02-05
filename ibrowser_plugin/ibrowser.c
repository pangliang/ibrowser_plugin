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

#include "funtions.h"


// Browser function table，可以通过它来得到浏览器提供的功能
static NPNetscapeFuncs* browser;
static const char *plugin_method_name_open = "open";
static NPP _instance;

////////////////////////////////////
/*******各种接口的声明*********/
//在NPAPI编程的接口中你会发现有NP_打头的，有NPP_打头的，有NPN_打头的
//NP是npapi的插件库提供给浏览器的最上层的接口
//NPP即NP Plugin是插件本身提供给浏览器调用的接口，主要被用来填充NPPluginFuncs的结构体
//NPN即NP Netscape ,是浏览器提供给插件使用的接口，这些接口一般都在NPNetscapeFuncs结构体中

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
bool plugin_has_method(NPObject *obj, NPIdentifier methodName);
bool plugin_invoke(NPObject *obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result);
bool hasProperty(NPObject *obj, NPIdentifier propertyName);
bool getProperty(NPObject *obj, NPIdentifier propertyName, NPVariant *result);
////////////////////////////////////

static struct NPClass scriptablePluginClass = {
    NP_CLASS_STRUCT_VERSION,
    NULL,
    NULL,
    NULL,
    plugin_has_method,
    plugin_invoke,
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

#define STRINGN_TO_NPVARIANT(_val, _len, _v)                                  \
NP_BEGIN_MACRO                                                                \
(_v).type = NPVariantType_String;                                         \
NPString str = { _val, (uint32_t)(_len) };                                  \
(_v).value.stringValue = str;                                             \
NP_END_MACRO

#define STRINGZ_TO_NPVARIANT(_val, _v)                                        \
NP_BEGIN_MACRO                                                                \
(_v).type = NPVariantType_String;                                         \
NPString str = { _val, (uint32_t)(strlen(_val)) };                          \
(_v).value.stringValue = str;                                             \
NP_END_MACRO

bool plugin_has_method(NPObject *obj, NPIdentifier methodName) {
    // This function will be called when we invoke method on this plugin elements.
    NPUTF8 *name = browser->utf8fromidentifier(methodName);
    bool result = strcmp(name, plugin_method_name_open) == 0;
    browser->memfree(name);
    return result;
}
bool plugin_invoke(NPObject *obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result) {
    // Make sure the method called is "open".
    NPUTF8 *name = browser->utf8fromidentifier(methodName);
    if(strcmp(name, plugin_method_name_open) == 0) {
        browser->memfree(name);
        BOOLEAN_TO_NPVARIANT(false, *result);
        // Meke sure the arugment has at least one String parameter.
        if(argCount > 0 && NPVARIANT_IS_STRING(args[0])) {
            char **dev_list = NULL;
            int i;
            
            i=get_device_id(&dev_list);
            char *yovae = (char*)browser->memalloc(40);
            strcpy(yovae,dev_list[0]);
            STRINGZ_TO_NPVARIANT(yovae,*result);
            
            printf("%s",dev_list[0]);
            
            //free(dev_list);
            
        }
        return true;
    }
    browser->memfree(name);
    return false;
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
    // Create per-instance storage
    //obj = (PluginObject *)malloc(sizeof(PluginObject));
    //bzero(obj, sizeof(PluginObject));
    
    //obj->npp = instance;
    //instance->pdata = obj;
    
    if(!instance->pdata) {
        instance->pdata = browser->createobject(instance, &scriptablePluginClass);
    }
    
    _instance=instance;
    
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
