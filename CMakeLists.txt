cmake_minimum_required(VERSION 2.8)

# VitaSDK defines
if( NOT DEFINED CMAKE_TOOLCHAIN_FILE )
  if( DEFINED ENV{VITASDK} )
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

# Project start
project(vitapong)
include("${VITASDK}/share/vita.cmake" REQUIRED)
set(VITA_APP_NAME "VitaPong")
set(VITA_TITLEID  "VITAPONG0")
set(VITA_VERSION  "01.00")
set (SOURCE_DIR "src")

# Flags and includes
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -Wno-unused-variable -Wno-unused-but-set-variable -Wno-format-truncation -fno-lto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")
# set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")
# set(VITA_MAKE_FSELF_FLAGS "${VITA_MAKE_FSELF_FLAGS} -a 0x2808000000000000")

include_directories(
    ${SOURCE_DIR}
)

file (GLOB SOURCE_FILES
    ${SOURCE_DIR}/*.c
    ${SOURCE_DIR}/*.cpp
)

add_executable(${PROJECT_NAME}
    ${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME}
  vita2d
  png
  jpeg
  z
  m
  c
  # stdc++
  # SceAppMgr_stub
  # SceAppUtil_stub
  SceAudio_stub
  # SceAudiodec_stub
  # SceCamera_stub
  SceCommonDialog_stub
  SceCtrl_stub
  SceDisplay_stub
  SceGxm_stub
  # SceIme_stub
  # SceHttp_stub
  # SceLibKernel_stub
  # SceMtpIfDriver_stub
  # SceMusicExport_stub
  # SceNet_stub
  # SceNetCtl_stub
  # SceNpDrm_stub
  # SceRegistryMgr_stub
  # SceShellSvc_stub
  # SceSsl_stub
  SceSysmodule_stub
  ScePgf_stub
  # ScePhotoExport_stub
  # ScePower_stub
  # ScePromoterUtil_stub
  SceTouch_stub
  # SceUdcd_stub
  # SceVideoExport_stub
  # SceVshBridge_stub
)

# Create Vita artifacts
vita_create_self(eboot.bin ${PROJECT_NAME} SAFE)
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} eboot.bin
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE pkg/sce_sys/icon0.png sce_sys/icon0.png
       pkg/sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
       pkg/sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
       pkg/sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
       pkg/data/beep.wav data/beep.wav
       pkg/data/boop.wav data/boop.wav
)

add_custom_target(send
  COMMAND curl -T eboot.bin ftp://$(PSVITAIP):1337/ux0:/app/${VITA_TITLEID}/
  DEPENDS eboot.bin
)

add_custom_target(vpksend
  COMMAND curl -T ${PROJECT_NAME}.vpk ftp://$(PSVITAIP):1337/ux0:/
  DEPENDS ${PROJECT_NAME}.vpk
)

