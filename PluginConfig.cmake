#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for ibrowser
#
#\**********************************************************/

set(PLUGIN_NAME "ibrowser")
set(PLUGIN_PREFIX "IBR")
set(COMPANY_NAME "pangliang")

# ActiveX constants:
set(FBTYPELIB_NAME ibrowserLib)
set(FBTYPELIB_DESC "ibrowser 1.0 Type Library")
set(IFBControl_DESC "ibrowser Control Interface")
set(FBControl_DESC "ibrowser Control Class")
set(IFBComJavascriptObject_DESC "ibrowser IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "ibrowser ComJavascriptObject Class")
set(IFBComEventSource_DESC "ibrowser IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID 1d1f439c-41c4-5018-a0b3-548f62656843)
set(IFBControl_GUID 0991656f-9014-5904-8ec2-4b15d51fb6d5)
set(FBControl_GUID e1977e53-6af8-56de-a5a9-d64902549119)
set(IFBComJavascriptObject_GUID ca068580-391f-5083-b595-20498409888a)
set(FBComJavascriptObject_GUID 11648679-0770-517e-9a6b-2e698e9ef7a8)
set(IFBComEventSource_GUID 16ae93bb-c453-5b40-b776-1aa8cb916331)
if ( FB_PLATFORM_ARCH_32 )
    set(FBControl_WixUpgradeCode_GUID fac112a1-8bd9-5a27-80c4-39dd26e4bd9d)
else ( FB_PLATFORM_ARCH_32 )
    set(FBControl_WixUpgradeCode_GUID a6f8b346-e3bf-5bca-bb98-3bfb97d4a7ea)
endif ( FB_PLATFORM_ARCH_32 )

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "pangliang.ibrowser")
set(MOZILLA_PLUGINID "liang8305.github.com/ibrowser")

# strings
set(FBSTRING_CompanyName "pangliang")
set(FBSTRING_PluginDescription "ios helper")
set(FBSTRING_PLUGIN_VERSION "1.0.0.0")
set(FBSTRING_LegalCopyright "Copyright 2013 pangliang")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "ibrowser")
set(FBSTRING_FileExtents "")
if ( FB_PLATFORM_ARCH_32 )
    set(FBSTRING_PluginName "ibrowser")  # No 32bit postfix to maintain backward compatability.
else ( FB_PLATFORM_ARCH_32 )
    set(FBSTRING_PluginName "ibrowser_${FB_PLATFORM_ARCH_NAME}")
endif ( FB_PLATFORM_ARCH_32 )
set(FBSTRING_MIMEType "application/x-ibrowser")

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

set (FB_GUI_DISABLED 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON 0)
set(FBMAC_USE_COCOA 0)
set(FBMAC_USE_COREGRAPHICS 0)
set(FBMAC_USE_COREANIMATION 0)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 0)

# If you want to register per-machine on Windows, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)
