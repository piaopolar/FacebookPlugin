LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

MY_FILES_PATH  :=  $(LOCAL_PATH)
MY_FILES_PATH  +=  $(LOCAL_PATH)/../../BaseCode
MY_FILES_PATH  +=  $(LOCAL_PATH)/../../Classes

#$(warning "the value of MY_FILES_PATH is[$(MY_FILES_PATH)]")

MY_FILES_SUFFIX := %.cpp %.c %.cc

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

My_All_Files := $(foreach src_path,$(MY_FILES_PATH), $(call rwildcard,$(src_path),*.*) ) 

#$(warning "the value of My_All_Files is[$(My_All_Files)]")

My_All_Files := $(My_All_Files:$(MY_CPP_PATH)/./%=$(MY_CPP_PATH)%)
MY_SRC_LIST  := $(filter $(MY_FILES_SUFFIX),$(My_All_Files)) 

#$(warning "the value of MY_SRC_LIST is[$(MY_SRC_LIST)]")

MY_SRC_LIST  := $(MY_SRC_LIST:$(LOCAL_PATH)/%=%)

#$(warning "the value of MY_SRC_LIST is[$(MY_SRC_LIST)]")

LOCAL_SRC_FILES := $(MY_SRC_LIST)

#$(warning "the value of LOCAL_SRC_FILES is[$(LOCAL_SRC_FILES)]")

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
					$(LOCAL_PATH)/../../BaseCode 

LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
LOCAL_WHOLE_STATIC_LIBRARIES += chipmunk_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,cocos2dx)
$(call import-module,cocos2dx/platform/third_party/android/prebuilt/libcurl)
$(call import-module,CocosDenshion/android)
$(call import-module,extensions)
$(call import-module,external/Box2D)
$(call import-module,external/chipmunk)
