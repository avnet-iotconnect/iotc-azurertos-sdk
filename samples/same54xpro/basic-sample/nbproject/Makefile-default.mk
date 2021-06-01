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
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/basic-sample.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/basic-sample.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
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
SOURCEFILES_QUOTED_IF_SPACED=../common_hardware_code/common_hardware_code.c nx_driver_same54.c ../common_hardware_code/tx_initialize_low_level.S src/iotconnect_app.c src/main.c src/sample_device_identity.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/364979329/common_hardware_code.o ${OBJECTDIR}/nx_driver_same54.o ${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o ${OBJECTDIR}/src/iotconnect_app.o ${OBJECTDIR}/src/main.o ${OBJECTDIR}/src/sample_device_identity.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/364979329/common_hardware_code.o.d ${OBJECTDIR}/nx_driver_same54.o.d ${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o.d ${OBJECTDIR}/src/iotconnect_app.o.d ${OBJECTDIR}/src/main.o.d ${OBJECTDIR}/src/sample_device_identity.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/364979329/common_hardware_code.o ${OBJECTDIR}/nx_driver_same54.o ${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o ${OBJECTDIR}/src/iotconnect_app.o ${OBJECTDIR}/src/main.o ${OBJECTDIR}/src/sample_device_identity.o

# Source Files
SOURCEFILES=../common_hardware_code/common_hardware_code.c nx_driver_same54.c ../common_hardware_code/tx_initialize_low_level.S src/iotconnect_app.c src/main.c src/sample_device_identity.c

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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/basic-sample.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

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
${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o: ../common_hardware_code/tx_initialize_low_level.S  .generated_files/faf17e58d14d8ec5b82fa0991ba6001f69ebfc9e.flag .generated_files/b00dc28b57e1ed16cd20acf6a70469bb96bf94af.flag
	@${MKDIR} "${OBJECTDIR}/_ext/364979329" 
	@${RM} ${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o.d 
	@${RM} ${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o 
	@${RM} ${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o.ok ${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o.err 
	${MP_CC} $(MP_EXTRA_AS_PRE)  -D__DEBUG  -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o.d"  -o ${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o ../common_hardware_code/tx_initialize_low_level.S  -DXPRJ_default=$(CND_CONF)    -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o.asm.d",--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--gdwarf-2,--defsym=__DEBUG=1 -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -I"../netxduo" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/common/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../same54_lib/same54_lib" -I"../netxduo/addons/dhcp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/dns" -I"../netxduo/addons/sntp" -I"../netxduo/addons/cloud" -I"../netxduo/addons/azure_iot" -I"../iotc-azrtos-sdk/iotc-c-lib/include" -I"../iotc-azrtos-sdk/azrtos-layer/include" -I"../iotc-azrtos-sdk/include" -I"include" -I"src" -mdfp="${DFP_DIR}"
	@${FIXDEPS} "${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o.d" "${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o.asm.d" -t $(SILENT) -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o: ../common_hardware_code/tx_initialize_low_level.S  .generated_files/3eaee51684fcad57991fb1413d3969253dc6d456.flag .generated_files/b00dc28b57e1ed16cd20acf6a70469bb96bf94af.flag
	@${MKDIR} "${OBJECTDIR}/_ext/364979329" 
	@${RM} ${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o.d 
	@${RM} ${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o 
	@${RM} ${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o.ok ${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o.err 
	${MP_CC} $(MP_EXTRA_AS_PRE)  -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o.d"  -o ${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o ../common_hardware_code/tx_initialize_low_level.S  -DXPRJ_default=$(CND_CONF)    -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o.asm.d",--gdwarf-2 -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -I"../netxduo" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/common/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../same54_lib/same54_lib" -I"../netxduo/addons/dhcp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/dns" -I"../netxduo/addons/sntp" -I"../netxduo/addons/cloud" -I"../netxduo/addons/azure_iot" -I"../iotc-azrtos-sdk/iotc-c-lib/include" -I"../iotc-azrtos-sdk/azrtos-layer/include" -I"../iotc-azrtos-sdk/include" -I"include" -I"src" -mdfp="${DFP_DIR}"
	@${FIXDEPS} "${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o.d" "${OBJECTDIR}/_ext/364979329/tx_initialize_low_level.o.asm.d" -t $(SILENT) -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/364979329/common_hardware_code.o: ../common_hardware_code/common_hardware_code.c  .generated_files/1d78c9f35caa12cd4fb17bd6543f3c8116929b14.flag .generated_files/b00dc28b57e1ed16cd20acf6a70469bb96bf94af.flag
	@${MKDIR} "${OBJECTDIR}/_ext/364979329" 
	@${RM} ${OBJECTDIR}/_ext/364979329/common_hardware_code.o.d 
	@${RM} ${OBJECTDIR}/_ext/364979329/common_hardware_code.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -I"../netxduo" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/common/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../same54_lib/same54_lib" -I"../netxduo/addons/dhcp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/dns" -I"../netxduo/addons/sntp" -I"../netxduo/addons/cloud" -I"../netxduo/addons/azure_iot" -I"../iotc-azrtos-sdk/iotc-c-lib/include" -I"../iotc-azrtos-sdk/azrtos-layer/include" -I"../iotc-azrtos-sdk/include" -I"include" -I"src" -I"../same54_lib/same54_lib" -I"../same54_lib/same54_lib/component" -I"../same54_lib/same54_lib/instance" -I"../same54_lib/same54_lib/pio" -I"../azure_iot" -I"../azure_iot/azure-sdk-for-c/sdk/inc/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/storage/" -MP -MMD -MF "${OBJECTDIR}/_ext/364979329/common_hardware_code.o.d" -o ${OBJECTDIR}/_ext/364979329/common_hardware_code.o ../common_hardware_code/common_hardware_code.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/nx_driver_same54.o: nx_driver_same54.c  .generated_files/96a5d50530e93351f002d5dd9e6e5a02e3bc36e6.flag .generated_files/b00dc28b57e1ed16cd20acf6a70469bb96bf94af.flag
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/nx_driver_same54.o.d 
	@${RM} ${OBJECTDIR}/nx_driver_same54.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -I"../netxduo" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/common/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../same54_lib/same54_lib" -I"../netxduo/addons/dhcp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/dns" -I"../netxduo/addons/sntp" -I"../netxduo/addons/cloud" -I"../netxduo/addons/azure_iot" -I"../iotc-azrtos-sdk/iotc-c-lib/include" -I"../iotc-azrtos-sdk/azrtos-layer/include" -I"../iotc-azrtos-sdk/include" -I"include" -I"src" -I"../same54_lib/same54_lib" -I"../same54_lib/same54_lib/component" -I"../same54_lib/same54_lib/instance" -I"../same54_lib/same54_lib/pio" -I"../azure_iot" -I"../azure_iot/azure-sdk-for-c/sdk/inc/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/storage/" -MP -MMD -MF "${OBJECTDIR}/nx_driver_same54.o.d" -o ${OBJECTDIR}/nx_driver_same54.o nx_driver_same54.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/src/iotconnect_app.o: src/iotconnect_app.c  .generated_files/d851b430907b1b55e1bca499998aef5b07bef67c.flag .generated_files/b00dc28b57e1ed16cd20acf6a70469bb96bf94af.flag
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/iotconnect_app.o.d 
	@${RM} ${OBJECTDIR}/src/iotconnect_app.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -I"../netxduo" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/common/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../same54_lib/same54_lib" -I"../netxduo/addons/dhcp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/dns" -I"../netxduo/addons/sntp" -I"../netxduo/addons/cloud" -I"../netxduo/addons/azure_iot" -I"../iotc-azrtos-sdk/iotc-c-lib/include" -I"../iotc-azrtos-sdk/azrtos-layer/include" -I"../iotc-azrtos-sdk/include" -I"include" -I"src" -I"../same54_lib/same54_lib" -I"../same54_lib/same54_lib/component" -I"../same54_lib/same54_lib/instance" -I"../same54_lib/same54_lib/pio" -I"../azure_iot" -I"../azure_iot/azure-sdk-for-c/sdk/inc/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/storage/" -MP -MMD -MF "${OBJECTDIR}/src/iotconnect_app.o.d" -o ${OBJECTDIR}/src/iotconnect_app.o src/iotconnect_app.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/src/main.o: src/main.c  .generated_files/8c8b7a5f39aff1b6cb9fe1b3626f16b94e7716b5.flag .generated_files/b00dc28b57e1ed16cd20acf6a70469bb96bf94af.flag
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/main.o.d 
	@${RM} ${OBJECTDIR}/src/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -I"../netxduo" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/common/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../same54_lib/same54_lib" -I"../netxduo/addons/dhcp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/dns" -I"../netxduo/addons/sntp" -I"../netxduo/addons/cloud" -I"../netxduo/addons/azure_iot" -I"../iotc-azrtos-sdk/iotc-c-lib/include" -I"../iotc-azrtos-sdk/azrtos-layer/include" -I"../iotc-azrtos-sdk/include" -I"include" -I"src" -I"../same54_lib/same54_lib" -I"../same54_lib/same54_lib/component" -I"../same54_lib/same54_lib/instance" -I"../same54_lib/same54_lib/pio" -I"../azure_iot" -I"../azure_iot/azure-sdk-for-c/sdk/inc/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/storage/" -MP -MMD -MF "${OBJECTDIR}/src/main.o.d" -o ${OBJECTDIR}/src/main.o src/main.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/src/sample_device_identity.o: src/sample_device_identity.c  .generated_files/3e8fcdebce570ace559884ff4f4458b455d55e8e.flag .generated_files/b00dc28b57e1ed16cd20acf6a70469bb96bf94af.flag
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/sample_device_identity.o.d 
	@${RM} ${OBJECTDIR}/src/sample_device_identity.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -I"../netxduo" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/common/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../same54_lib/same54_lib" -I"../netxduo/addons/dhcp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/dns" -I"../netxduo/addons/sntp" -I"../netxduo/addons/cloud" -I"../netxduo/addons/azure_iot" -I"../iotc-azrtos-sdk/iotc-c-lib/include" -I"../iotc-azrtos-sdk/azrtos-layer/include" -I"../iotc-azrtos-sdk/include" -I"include" -I"src" -I"../same54_lib/same54_lib" -I"../same54_lib/same54_lib/component" -I"../same54_lib/same54_lib/instance" -I"../same54_lib/same54_lib/pio" -I"../azure_iot" -I"../azure_iot/azure-sdk-for-c/sdk/inc/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/storage/" -MP -MMD -MF "${OBJECTDIR}/src/sample_device_identity.o.d" -o ${OBJECTDIR}/src/sample_device_identity.o src/sample_device_identity.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
else
${OBJECTDIR}/_ext/364979329/common_hardware_code.o: ../common_hardware_code/common_hardware_code.c  .generated_files/c72672603b0a1e27db602b5d553868593384cd0a.flag .generated_files/b00dc28b57e1ed16cd20acf6a70469bb96bf94af.flag
	@${MKDIR} "${OBJECTDIR}/_ext/364979329" 
	@${RM} ${OBJECTDIR}/_ext/364979329/common_hardware_code.o.d 
	@${RM} ${OBJECTDIR}/_ext/364979329/common_hardware_code.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -I"../netxduo" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/common/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../same54_lib/same54_lib" -I"../netxduo/addons/dhcp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/dns" -I"../netxduo/addons/sntp" -I"../netxduo/addons/cloud" -I"../netxduo/addons/azure_iot" -I"../iotc-azrtos-sdk/iotc-c-lib/include" -I"../iotc-azrtos-sdk/azrtos-layer/include" -I"../iotc-azrtos-sdk/include" -I"include" -I"src" -I"../same54_lib/same54_lib" -I"../same54_lib/same54_lib/component" -I"../same54_lib/same54_lib/instance" -I"../same54_lib/same54_lib/pio" -I"../azure_iot" -I"../azure_iot/azure-sdk-for-c/sdk/inc/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/storage/" -MP -MMD -MF "${OBJECTDIR}/_ext/364979329/common_hardware_code.o.d" -o ${OBJECTDIR}/_ext/364979329/common_hardware_code.o ../common_hardware_code/common_hardware_code.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/nx_driver_same54.o: nx_driver_same54.c  .generated_files/fa4188e865798e569aec4e9deb9687383241391a.flag .generated_files/b00dc28b57e1ed16cd20acf6a70469bb96bf94af.flag
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/nx_driver_same54.o.d 
	@${RM} ${OBJECTDIR}/nx_driver_same54.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -I"../netxduo" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/common/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../same54_lib/same54_lib" -I"../netxduo/addons/dhcp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/dns" -I"../netxduo/addons/sntp" -I"../netxduo/addons/cloud" -I"../netxduo/addons/azure_iot" -I"../iotc-azrtos-sdk/iotc-c-lib/include" -I"../iotc-azrtos-sdk/azrtos-layer/include" -I"../iotc-azrtos-sdk/include" -I"include" -I"src" -I"../same54_lib/same54_lib" -I"../same54_lib/same54_lib/component" -I"../same54_lib/same54_lib/instance" -I"../same54_lib/same54_lib/pio" -I"../azure_iot" -I"../azure_iot/azure-sdk-for-c/sdk/inc/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/storage/" -MP -MMD -MF "${OBJECTDIR}/nx_driver_same54.o.d" -o ${OBJECTDIR}/nx_driver_same54.o nx_driver_same54.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/src/iotconnect_app.o: src/iotconnect_app.c  .generated_files/c59821c2c98b01d6704dc9af215c5a58173eebed.flag .generated_files/b00dc28b57e1ed16cd20acf6a70469bb96bf94af.flag
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/iotconnect_app.o.d 
	@${RM} ${OBJECTDIR}/src/iotconnect_app.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -I"../netxduo" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/common/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../same54_lib/same54_lib" -I"../netxduo/addons/dhcp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/dns" -I"../netxduo/addons/sntp" -I"../netxduo/addons/cloud" -I"../netxduo/addons/azure_iot" -I"../iotc-azrtos-sdk/iotc-c-lib/include" -I"../iotc-azrtos-sdk/azrtos-layer/include" -I"../iotc-azrtos-sdk/include" -I"include" -I"src" -I"../same54_lib/same54_lib" -I"../same54_lib/same54_lib/component" -I"../same54_lib/same54_lib/instance" -I"../same54_lib/same54_lib/pio" -I"../azure_iot" -I"../azure_iot/azure-sdk-for-c/sdk/inc/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/storage/" -MP -MMD -MF "${OBJECTDIR}/src/iotconnect_app.o.d" -o ${OBJECTDIR}/src/iotconnect_app.o src/iotconnect_app.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/src/main.o: src/main.c  .generated_files/9877abc20b2c3abb6d5332f1972708bd3a2b53a.flag .generated_files/b00dc28b57e1ed16cd20acf6a70469bb96bf94af.flag
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/main.o.d 
	@${RM} ${OBJECTDIR}/src/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -I"../netxduo" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/common/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../same54_lib/same54_lib" -I"../netxduo/addons/dhcp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/dns" -I"../netxduo/addons/sntp" -I"../netxduo/addons/cloud" -I"../netxduo/addons/azure_iot" -I"../iotc-azrtos-sdk/iotc-c-lib/include" -I"../iotc-azrtos-sdk/azrtos-layer/include" -I"../iotc-azrtos-sdk/include" -I"include" -I"src" -I"../same54_lib/same54_lib" -I"../same54_lib/same54_lib/component" -I"../same54_lib/same54_lib/instance" -I"../same54_lib/same54_lib/pio" -I"../azure_iot" -I"../azure_iot/azure-sdk-for-c/sdk/inc/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/storage/" -MP -MMD -MF "${OBJECTDIR}/src/main.o.d" -o ${OBJECTDIR}/src/main.o src/main.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/src/sample_device_identity.o: src/sample_device_identity.c  .generated_files/6076f33d39310c8f9866cc1b96d4e9f2b1415c96.flag .generated_files/b00dc28b57e1ed16cd20acf6a70469bb96bf94af.flag
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/sample_device_identity.o.d 
	@${RM} ${OBJECTDIR}/src/sample_device_identity.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../netxduo/addons/azure_iot/azure-sdk-for-c/sdk/inc" -I"../netxduo" -I"../netxduo/common/inc" -I"../netxduo/ports/cortex_m4/gnu/inc" -I"../netxduo/nx_secure/inc" -I"../netxduo/nx_secure/ports" -I"../netxduo/crypto_libraries/inc" -I"../threadx/common/inc" -I"../threadx/ports/cortex_m4/gnu/inc" -I"../same54_lib/same54_lib" -I"../netxduo/addons/dhcp" -I"../netxduo/addons/mqtt" -I"../netxduo/addons/dns" -I"../netxduo/addons/sntp" -I"../netxduo/addons/cloud" -I"../netxduo/addons/azure_iot" -I"../iotc-azrtos-sdk/iotc-c-lib/include" -I"../iotc-azrtos-sdk/azrtos-layer/include" -I"../iotc-azrtos-sdk/include" -I"include" -I"src" -I"../same54_lib/same54_lib" -I"../same54_lib/same54_lib/component" -I"../same54_lib/same54_lib/instance" -I"../same54_lib/same54_lib/pio" -I"../azure_iot" -I"../azure_iot/azure-sdk-for-c/sdk/inc/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/core/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/iot/internal" -I"../azure_iot/azure-sdk-for-c/sdk/inc/azure/storage/" -MP -MMD -MF "${OBJECTDIR}/src/sample_device_identity.o.d" -o ${OBJECTDIR}/src/sample_device_identity.o src/sample_device_identity.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}" ${PACK_COMMON_OPTIONS} 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/basic-sample.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../iotc-azrtos-sdk/dist/default/debug/iotc-azrtos-sdk.a ../netxduo/dist/default/debug/netxduo.a ../same54_lib/dist/default/debug/same54_lib.a ../threadx/dist/default/debug/threadx.a  
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -g   -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/basic-sample.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    ..\iotc-azrtos-sdk\dist\default\debug\iotc-azrtos-sdk.a ..\netxduo\dist\default\debug\netxduo.a ..\same54_lib\dist\default\debug\same54_lib.a ..\threadx\dist\default\debug\threadx.a      -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D=__DEBUG_D,--defsym=_min_heap_size=32768,--gc-sections,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml -mdfp="${DFP_DIR}"
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/basic-sample.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../iotc-azrtos-sdk/dist/default/production/iotc-azrtos-sdk.a ../netxduo/dist/default/production/netxduo.a ../same54_lib/dist/default/production/same54_lib.a ../threadx/dist/default/production/threadx.a 
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/basic-sample.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    ..\iotc-azrtos-sdk\dist\default\production\iotc-azrtos-sdk.a ..\netxduo\dist\default\production\netxduo.a ..\same54_lib\dist\default\production\same54_lib.a ..\threadx\dist\default\production\threadx.a      -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=32768,--gc-sections,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml -mdfp="${DFP_DIR}"
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/basic-sample.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:
	cd ../iotc-azrtos-sdk && ${MAKE}  -f Makefile CONF=default
	cd ../netxduo && ${MAKE}  -f Makefile CONF=default
	cd ../same54_lib && ${MAKE}  -f Makefile CONF=default
	cd ../threadx && ${MAKE}  -f Makefile CONF=default


# Subprojects
.clean-subprojects:
	cd ../iotc-azrtos-sdk && rm -rf "build/default" "dist/default"
	cd ../netxduo && rm -rf "build/default" "dist/default"
	cd ../same54_lib && rm -rf "build/default" "dist/default"
	cd ../threadx && rm -rf "build/default" "dist/default"

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
