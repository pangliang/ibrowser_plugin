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
#define CLIENT_LABEL "ibrowser"

static const char *domains[] = {
        "com.apple.disk_usage",
        "com.apple.mobile.battery",
        "com.apple.iqagent",
        "com.apple.mobile.third_party_termination",
        "com.apple.mobile.lockdownd",
        "com.apple.xcode.developerdomain",
        "com.apple.international",
        "com.apple.mobile.data_sync",
        "com.apple.mobile.tethered_sync",
        "com.apple.mobile.mobile_application_usage",
        "com.apple.mobile.backup",
        "com.apple.mobile.nikita",
        "com.apple.mobile.restriction",
        "com.apple.mobile.user_preferences",
        "com.apple.mobile.sync_data_class",
        "com.apple.mobile.software_behavior",
        "com.apple.mobile.iTunes.SQLMusicLibraryPostProcessCommands",
        "com.apple.mobile.iTunes.accessories",
        "com.apple.mobile.internal", /**< iOS 4.0+ */
        "com.apple.mobile.wireless_lockdown", /**< iOS 4.0+ */
        "com.apple.fairplay",
        "com.apple.iTunes",
        "com.apple.mobile.iTunes.store",
        "com.apple.mobile.iTunes",
        NULL
};


bool init(bool reconnect);
int get_device_info(char **args, unsigned int argCount, char *result);
int get_app_list(char **args, unsigned int argCount, char *result);
int read_file(char **args, uint32_t argCount, char *result);
int upload_file(char **args, uint32_t argCount, char *result);
int install_package(char **args, uint32_t argCount, char *result);
int get_sbservices_icon_pngdata(char **args, uint32_t argCount, char *result);

static unordered_map<string, int (*) ( char **, unsigned int, char * )> INVOKE_FUNCTIONS ={
        { "get_device_info", get_device_info},
        { "get_app_list", get_app_list},
        { "read_file", read_file},
        { "upload_file", upload_file},
        { "install_package", install_package},
        { "get_sbservices_icon_pngdata", get_sbservices_icon_pngdata},

};


#endif
