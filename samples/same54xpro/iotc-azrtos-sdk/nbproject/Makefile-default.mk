#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=a
DEBUGGABLE_SUFFIX=
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/iotc-azrtos-sdk.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=a
DEBUGGABLE_SUFFIX=
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/iotc-azrtos-sdk.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=azrtos-layer/nx-http-client/nx_web_http_client.c azrtos-layer/src/azrtos_https_client.c azrtos-layer/src/azrtos_iothub_client.c azrtos-layer/src/azrtos_time.c azrtos-layer/src/nx_azure_iot_ciphersuites.c cJSON/cJSON.c iotc-c-lib/src/iotconnect_common.c iotc-c-lib/src/iotconnect_discovery.c iotc-c-lib/src/iotconnect_event.c iotc-c-lib/src/iotconnect_lib.c iotc-c-lib/src/iotconnect_telemetry.c src/iotconnect.c src/iotconnect_certs.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/azrtos-layer/nx-http-client/nx_web_http_client.o ${OBJECTDIR}/azrtos-layer/src/azrtos_https_client.o ${OBJECTDIR}/azrtos-layer/src/azrtos_iothub_client.o ${OBJECTDIR}/azrtos-layer/src/azrtos_time.o ${OBJECTDIR}/azrtos-layer/src/nx_azure_iot_ciphersuites.o ${OBJECTDIR}/cJSON/cJSON.o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_common.o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_discovery.o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_event.o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_lib.o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_telemetry.o ${OBJECTDIR}/src/iotconnect.o ${OBJECTDIR}/src/iotconnect_certs.o
POSSIBLE_DEPFILES=${OBJECTDIR}/azrtos-layer/nx-http-client/nx_web_http_client.o.d ${OBJECTDIR}/azrtos-layer/src/azrtos_https_client.o.d ${OBJECTDIR}/azrtos-layer/src/azrtos_iothub_client.o.d ${OBJECTDIR}/azrtos-layer/src/azrtos_time.o.d ${OBJECTDIR}/azrtos-layer/src/nx_azure_iot_ciphersuites.o.d ${OBJECTDIR}/cJSON/cJSON.o.d ${OBJECTDIR}/iotc-c-lib/src/iotconnect_common.o.d ${OBJECTDIR}/iotc-c-lib/src/iotconnect_discovery.o.d ${OBJECTDIR}/iotc-c-lib/src/iotconnect_event.o.d ${OBJECTDIR}/iotc-c-lib/src/iotconnect_lib.o.d ${OBJECTDIR}/iotc-c-lib/src/iotconnect_telemetry.o.d ${OBJECTDIR}/src/iotconnect.o.d ${OBJECTDIR}/src/iotconnect_certs.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/azrtos-layer/nx-http-client/nx_web_http_client.o ${OBJECTDIR}/azrtos-layer/src/azrtos_https_client.o ${OBJECTDIR}/azrtos-layer/src/azrtos_iothub_client.o ${OBJECTDIR}/azrtos-layer/src/azrtos_time.o ${OBJECTDIR}/azrtos-layer/src/nx_azure_iot_ciphersuites.o ${OBJECTDIR}/cJSON/cJSON.o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_common.o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_discovery.o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_event.o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_lib.o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_telemetry.o ${OBJECTDIR}/src/iotconnect.o ${OBJECTDIR}/src/iotconnect_certs.o

# Source Files
SOURCEFILES=azrtos-layer/nx-http-client/nx_web_http_client.c azrtos-layer/src/azrtos_https_client.c azrtos-layer/src/azrtos_iothub_client.c azrtos-layer/src/azrtos_time.c azrtos-layer/src/nx_azure_iot_ciphersuites.c cJSON/cJSON.c iotc-c-lib/src/iotconnect_common.c iotc-c-lib/src/iotconnect_discovery.c iotc-c-lib/src/iotconnect_event.c iotc-c-lib/src/iotconnect_lib.c iotc-c-lib/src/iotconnect_telemetry.c src/iotconnect.c src/iotconnect_certs.c

# Pack Options 
PACK_COMMON_OPTIONS=-I "${CMSIS_DIR}/CMSIS/Core/Include"



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/iotc-azrtos-sdk.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=ATSAME54P20A
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/azrtos-layer/nx-http-client/nx_web_http_client.o: azrtos-layer/nx-http-client/nx_web_http_client.c  .generated_files/7c93c345bd773b1c83ddb8756b15ad1711e7873.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/azrtos-layer/nx-http-client" 
	@${RM} ${OBJECTDIR}/azrtos-layer/nx-http-client/nx_web_http_client.o.d 
	@${RM} ${OBJECTDIR}/azrtos-layer/nx-http-client/nx_web_http_client.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/azrtos-layer/nx-http-client/nx_web_http_client.o.d" -o ${OBJECTDIR}/azrtos-layer/nx-http-client/nx_web_http_client.o azrtos-layer/nx-http-client/nx_web_http_client.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/azrtos-layer/src/azrtos_https_client.o: azrtos-layer/src/azrtos_https_client.c  .generated_files/d570105890bf0e29a3495ed2b61b1d8ebebb4f02.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/azrtos-layer/src" 
	@${RM} ${OBJECTDIR}/azrtos-layer/src/azrtos_https_client.o.d 
	@${RM} ${OBJECTDIR}/azrtos-layer/src/azrtos_https_client.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/azrtos-layer/src/azrtos_https_client.o.d" -o ${OBJECTDIR}/azrtos-layer/src/azrtos_https_client.o azrtos-layer/src/azrtos_https_client.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/azrtos-layer/src/azrtos_iothub_client.o: azrtos-layer/src/azrtos_iothub_client.c  .generated_files/b545a5934778e1988b56edef168bed1ddfb664f6.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/azrtos-layer/src" 
	@${RM} ${OBJECTDIR}/azrtos-layer/src/azrtos_iothub_client.o.d 
	@${RM} ${OBJECTDIR}/azrtos-layer/src/azrtos_iothub_client.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/azrtos-layer/src/azrtos_iothub_client.o.d" -o ${OBJECTDIR}/azrtos-layer/src/azrtos_iothub_client.o azrtos-layer/src/azrtos_iothub_client.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/azrtos-layer/src/azrtos_time.o: azrtos-layer/src/azrtos_time.c  .generated_files/c4a4f10e83c001924cc0d0c94eb23efbde3c0618.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/azrtos-layer/src" 
	@${RM} ${OBJECTDIR}/azrtos-layer/src/azrtos_time.o.d 
	@${RM} ${OBJECTDIR}/azrtos-layer/src/azrtos_time.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/azrtos-layer/src/azrtos_time.o.d" -o ${OBJECTDIR}/azrtos-layer/src/azrtos_time.o azrtos-layer/src/azrtos_time.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/azrtos-layer/src/nx_azure_iot_ciphersuites.o: azrtos-layer/src/nx_azure_iot_ciphersuites.c  .generated_files/8f1ba41f9fb8f537413d4b795b571ee00079d255.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/azrtos-layer/src" 
	@${RM} ${OBJECTDIR}/azrtos-layer/src/nx_azure_iot_ciphersuites.o.d 
	@${RM} ${OBJECTDIR}/azrtos-layer/src/nx_azure_iot_ciphersuites.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/azrtos-layer/src/nx_azure_iot_ciphersuites.o.d" -o ${OBJECTDIR}/azrtos-layer/src/nx_azure_iot_ciphersuites.o azrtos-layer/src/nx_azure_iot_ciphersuites.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/cJSON/cJSON.o: cJSON/cJSON.c  .generated_files/7456a6593581d482fb265be90cf1b483834f9a24.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/cJSON" 
	@${RM} ${OBJECTDIR}/cJSON/cJSON.o.d 
	@${RM} ${OBJECTDIR}/cJSON/cJSON.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/cJSON/cJSON.o.d" -o ${OBJECTDIR}/cJSON/cJSON.o cJSON/cJSON.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/iotc-c-lib/src/iotconnect_common.o: iotc-c-lib/src/iotconnect_common.c  .generated_files/9d4d2bf3a7b10516c3a689b935ea1d4db634bbb9.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/iotc-c-lib/src" 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_common.o.d 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_common.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/iotc-c-lib/src/iotconnect_common.o.d" -o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_common.o iotc-c-lib/src/iotconnect_common.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/iotc-c-lib/src/iotconnect_discovery.o: iotc-c-lib/src/iotconnect_discovery.c  .generated_files/abee1dcebb06ee5eb7c58247cb9530f9d0229c7a.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/iotc-c-lib/src" 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_discovery.o.d 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_discovery.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/iotc-c-lib/src/iotconnect_discovery.o.d" -o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_discovery.o iotc-c-lib/src/iotconnect_discovery.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/iotc-c-lib/src/iotconnect_event.o: iotc-c-lib/src/iotconnect_event.c  .generated_files/59c603d3267db6c5b4ef873bed56343d4b3f1c88.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/iotc-c-lib/src" 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_event.o.d 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_event.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/iotc-c-lib/src/iotconnect_event.o.d" -o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_event.o iotc-c-lib/src/iotconnect_event.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/iotc-c-lib/src/iotconnect_lib.o: iotc-c-lib/src/iotconnect_lib.c  .generated_files/45e1329c61f26695073afe178233b138baa56fd6.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/iotc-c-lib/src" 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_lib.o.d 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_lib.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/iotc-c-lib/src/iotconnect_lib.o.d" -o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_lib.o iotc-c-lib/src/iotconnect_lib.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/iotc-c-lib/src/iotconnect_telemetry.o: iotc-c-lib/src/iotconnect_telemetry.c  .generated_files/6e446553b9fc65b3a5a295e62b07e90e5b57425d.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/iotc-c-lib/src" 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_telemetry.o.d 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_telemetry.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/iotc-c-lib/src/iotconnect_telemetry.o.d" -o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_telemetry.o iotc-c-lib/src/iotconnect_telemetry.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/src/iotconnect.o: src/iotconnect.c  .generated_files/a8ccfc804c948d2a968d5cb84231b98377f768d9.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/iotconnect.o.d 
	@${RM} ${OBJECTDIR}/src/iotconnect.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/src/iotconnect.o.d" -o ${OBJECTDIR}/src/iotconnect.o src/iotconnect.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/src/iotconnect_certs.o: src/iotconnect_certs.c  .generated_files/9e452e86cdc33a95250f978ce892d4a191d1a0f5.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/iotconnect_certs.o.d 
	@${RM} ${OBJECTDIR}/src/iotconnect_certs.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/src/iotconnect_certs.o.d" -o ${OBJECTDIR}/src/iotconnect_certs.o src/iotconnect_certs.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
else
${OBJECTDIR}/azrtos-layer/nx-http-client/nx_web_http_client.o: azrtos-layer/nx-http-client/nx_web_http_client.c  .generated_files/77a01dcd47070adffc4dbdf261da8261f47a96b7.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/azrtos-layer/nx-http-client" 
	@${RM} ${OBJECTDIR}/azrtos-layer/nx-http-client/nx_web_http_client.o.d 
	@${RM} ${OBJECTDIR}/azrtos-layer/nx-http-client/nx_web_http_client.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/azrtos-layer/nx-http-client/nx_web_http_client.o.d" -o ${OBJECTDIR}/azrtos-layer/nx-http-client/nx_web_http_client.o azrtos-layer/nx-http-client/nx_web_http_client.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/azrtos-layer/src/azrtos_https_client.o: azrtos-layer/src/azrtos_https_client.c  .generated_files/87d754cf08fa40f5095d84e053410758cb3b6410.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/azrtos-layer/src" 
	@${RM} ${OBJECTDIR}/azrtos-layer/src/azrtos_https_client.o.d 
	@${RM} ${OBJECTDIR}/azrtos-layer/src/azrtos_https_client.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/azrtos-layer/src/azrtos_https_client.o.d" -o ${OBJECTDIR}/azrtos-layer/src/azrtos_https_client.o azrtos-layer/src/azrtos_https_client.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/azrtos-layer/src/azrtos_iothub_client.o: azrtos-layer/src/azrtos_iothub_client.c  .generated_files/68e73610450707d1a928eedf2e5f60ce99ba959e.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/azrtos-layer/src" 
	@${RM} ${OBJECTDIR}/azrtos-layer/src/azrtos_iothub_client.o.d 
	@${RM} ${OBJECTDIR}/azrtos-layer/src/azrtos_iothub_client.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/azrtos-layer/src/azrtos_iothub_client.o.d" -o ${OBJECTDIR}/azrtos-layer/src/azrtos_iothub_client.o azrtos-layer/src/azrtos_iothub_client.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/azrtos-layer/src/azrtos_time.o: azrtos-layer/src/azrtos_time.c  .generated_files/8551a31b2caa4422077604dbba6ea48b56b95ae.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/azrtos-layer/src" 
	@${RM} ${OBJECTDIR}/azrtos-layer/src/azrtos_time.o.d 
	@${RM} ${OBJECTDIR}/azrtos-layer/src/azrtos_time.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/azrtos-layer/src/azrtos_time.o.d" -o ${OBJECTDIR}/azrtos-layer/src/azrtos_time.o azrtos-layer/src/azrtos_time.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/azrtos-layer/src/nx_azure_iot_ciphersuites.o: azrtos-layer/src/nx_azure_iot_ciphersuites.c  .generated_files/5655d15c6a184eb8c1729c9450a0f27aeaecee90.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/azrtos-layer/src" 
	@${RM} ${OBJECTDIR}/azrtos-layer/src/nx_azure_iot_ciphersuites.o.d 
	@${RM} ${OBJECTDIR}/azrtos-layer/src/nx_azure_iot_ciphersuites.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/azrtos-layer/src/nx_azure_iot_ciphersuites.o.d" -o ${OBJECTDIR}/azrtos-layer/src/nx_azure_iot_ciphersuites.o azrtos-layer/src/nx_azure_iot_ciphersuites.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/cJSON/cJSON.o: cJSON/cJSON.c  .generated_files/98e00e5aaf47bc5438e0bf0dc428560f30fcc391.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/cJSON" 
	@${RM} ${OBJECTDIR}/cJSON/cJSON.o.d 
	@${RM} ${OBJECTDIR}/cJSON/cJSON.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/cJSON/cJSON.o.d" -o ${OBJECTDIR}/cJSON/cJSON.o cJSON/cJSON.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/iotc-c-lib/src/iotconnect_common.o: iotc-c-lib/src/iotconnect_common.c  .generated_files/b52eaa8d59af936db9d34dea1231129325fc5933.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/iotc-c-lib/src" 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_common.o.d 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_common.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/iotc-c-lib/src/iotconnect_common.o.d" -o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_common.o iotc-c-lib/src/iotconnect_common.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/iotc-c-lib/src/iotconnect_discovery.o: iotc-c-lib/src/iotconnect_discovery.c  .generated_files/3b31306f6a3dc78c70f111a3df4b539b45ac85cf.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/iotc-c-lib/src" 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_discovery.o.d 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_discovery.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/iotc-c-lib/src/iotconnect_discovery.o.d" -o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_discovery.o iotc-c-lib/src/iotconnect_discovery.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/iotc-c-lib/src/iotconnect_event.o: iotc-c-lib/src/iotconnect_event.c  .generated_files/a61e46f62f8878a2be35d1a54640814265c741bd.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/iotc-c-lib/src" 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_event.o.d 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_event.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/iotc-c-lib/src/iotconnect_event.o.d" -o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_event.o iotc-c-lib/src/iotconnect_event.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/iotc-c-lib/src/iotconnect_lib.o: iotc-c-lib/src/iotconnect_lib.c  .generated_files/c6367f453f470beb91984b3256c75103904f175d.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/iotc-c-lib/src" 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_lib.o.d 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_lib.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/iotc-c-lib/src/iotconnect_lib.o.d" -o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_lib.o iotc-c-lib/src/iotconnect_lib.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/iotc-c-lib/src/iotconnect_telemetry.o: iotc-c-lib/src/iotconnect_telemetry.c  .generated_files/81a4723797789b373f691c32378bdf9157ed07dc.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/iotc-c-lib/src" 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_telemetry.o.d 
	@${RM} ${OBJECTDIR}/iotc-c-lib/src/iotconnect_telemetry.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/iotc-c-lib/src/iotconnect_telemetry.o.d" -o ${OBJECTDIR}/iotc-c-lib/src/iotconnect_telemetry.o iotc-c-lib/src/iotconnect_telemetry.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/src/iotconnect.o: src/iotconnect.c  .generated_files/f7ea71924fcf2fae503f094edc5d353872b8ca62.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/iotconnect.o.d 
	@${RM} ${OBJECTDIR}/src/iotconnect.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/src/iotconnect.o.d" -o ${OBJECTDIR}/src/iotconnect.o src/iotconnect.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/src/iotconnect_certs.o: src/iotconnect_certs.c  .generated_files/b0ed0756a6ac38705fa14bb08bc3a443a0a415af.flag .generated_files/e18ece4edeec8d57332facee58ce85a43854923e.flag
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/iotconnect_certs.o.d 
	@${RM} ${OBJECTDIR}/src/iotconnect_certs.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"cJSON" -I"azrtos-layer/nx-http-client" -I"azrtos-layer/include" -I"iotc-c-lib/include" -I"include" -I"../netxduo/addons/dns" -I"../netxduo/addons/azure_iot" -I"../netxduo/addons/sntp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/cloud" -I"../threadx/common/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -DIOTC_NEEDS_GETTIMEOFDAY_OU -DNX_WEB_HTTPS_ENABLE -I"azrtos-layer/include" -I"include" -I"iotc-c-lib/include" -MP -MMD -MF "${OBJECTDIR}/src/iotconnect_certs.o.d" -o ${OBJECTDIR}/src/iotconnect_certs.o src/iotconnect_certs.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: archive
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/iotc-azrtos-sdk.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_AR} $(MP_EXTRA_AR_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  r dist/${CND_CONF}/${IMAGE_TYPE}/iotc-azrtos-sdk.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    
else
dist/${CND_CONF}/${IMAGE_TYPE}/iotc-azrtos-sdk.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_AR} $(MP_EXTRA_AR_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  r dist/${CND_CONF}/${IMAGE_TYPE}/iotc-azrtos-sdk.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
