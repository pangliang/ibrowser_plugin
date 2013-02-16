//
//  funtions.h
//  IbrowserPlugin
//
//  Created by liangwei on 13-2-4.
//  Copyright (c) 2013年 liangwei. All rights reserved.
//

#ifndef IbrowserPlugin_funtions_h
#define IbrowserPlugin_funtions_h

#include <string>
#include <unordered_map>
using namespace std;

#define RESULT_BUFF_SIZE 1024000

typedef int (*InvokeFunction) ( char *, char * );

int get_device_id(char *args, char *result);
int get_device_info(char *args, char *result);
int get_app_list(char *args, char *result);
 
static unordered_map<string, InvokeFunction> invoke_functions={
        { "get_device_id", get_device_id},
        { "get_device_info", get_device_info},
        { "get_app_list", get_app_list},

};


#endif
