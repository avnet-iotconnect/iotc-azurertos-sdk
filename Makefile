#
# There are often missing header files, etc. Use "make -i -k" to power past compilation problems (not necessarily errors).
#

srcdir            = ./iotc-azrtos-sdk/src
CC_SOURCES        += $(wildcard $(srcdir)/*.c)
srcdir1            = ./iotc-azrtos-sdk/azrtos-layer/src
CC_SOURCES        += $(wildcard $(srcdir1)/*.c)
srcdir2            = ./iotc-azrtos-sdk/azrtos-layer/azrtos-ota/src
CC_SOURCES        += $(wildcard $(srcdir2)/*.c)
srcdir3            = ./iotc-azrtos-sdk/azrtos-layer/nx-http-client
CC_SOURCES        += $(wildcard $(srcdir3)/*.c)
srcdir4            = ./samples/stm32l4/sensors-demo/src
CC_SOURCES        += $(wildcard $(srcdir4)/*.c)
srcdir5            = ./samples/stm32l4/sensors-demo/BSPv2/B-L475E-IOT01
CC_SOURCES        += $(wildcard $(srcdir5)/*.c)
srcdir6            = ./samples/stm32l4/sensors-demo/BSPv2/Components/lps22hb
CC_SOURCES        += $(wildcard $(srcdir6)/*.c)
srcdir7            = ./samples/stm32l4/sensors-demo/BSPv2/Components/lis3mdl
CC_SOURCES        += $(wildcard $(srcdir7)/*.c)
srcdir8            = ./samples/stm32l4/sensors-demo/BSPv2/Components/lsm6dsl
CC_SOURCES        += $(wildcard $(srcdir8)/*.c)
srcdir9            = ./samples/stm32l4/sensors-demo/BSPv2/Components/hts221
CC_SOURCES        += $(wildcard $(srcdir9)/*.c)
srcdir10            = ./samples/stm32l4/basic-sample/src
CC_SOURCES        += $(wildcard $(srcdir10)/*.c)
srcdir11            = ./samples/stm32l4/common_hardware_code
CC_SOURCES        += $(wildcard $(srcdir11)/*.c)
srcdir12            = ./samples/mimxrt1060/basic-sample/src
CC_SOURCES        += $(wildcard $(srcdir12)/*.c)
srcdir13            = ./samples/mimxrt1060/basic-sample/startup
CC_SOURCES        += $(wildcard $(srcdir13)/*.c)
srcdir14            = ./samples/mimxrt1060/basic-sample
CC_SOURCES        += $(wildcard $(srcdir14)/*.c)
srcdir15            = ./samples/maaxboardrt/basic-sample/xip
CC_SOURCES        += $(wildcard $(srcdir15)/*.c)
srcdir16            = ./samples/maaxboardrt/basic-sample/src
CC_SOURCES        += $(wildcard $(srcdir16)/*.c)
srcdir17            = ./samples/maaxboardrt/basic-sample/board
CC_SOURCES        += $(wildcard $(srcdir17)/*.c)
srcdir18            = ./samples/maaxboardrt/basic-sample/drivers
CC_SOURCES        += $(wildcard $(srcdir18)/*.c)
srcdir19            = ./samples/maaxboardrt/basic-sample/phy
CC_SOURCES        += $(wildcard $(srcdir19)/*.c)
srcdir20            = ./samples/nxpdemos/overlay/maaxboardrt/xip
CC_SOURCES        += $(wildcard $(srcdir20)/*.c)
srcdir21            = ./samples/nxpdemos/overlay/maaxboardrt/board
CC_SOURCES        += $(wildcard $(srcdir21)/*.c)
srcdir22            = ./samples/nxpdemos/overlay/maaxboardrt/iotconnectdemo
CC_SOURCES        += $(wildcard $(srcdir22)/*.c)
srcdir23            = ./samples/nxpdemos/overlay/maaxboardrt/phy
CC_SOURCES        += $(wildcard $(srcdir23)/*.c)
srcdir24            = ./samples/nxpdemos/overlay/lpc55s69/iotconnectdemo
CC_SOURCES        += $(wildcard $(srcdir24)/*.c)
srcdir25            = ./samples/nxpdemos/overlay/rt1060/iotconnectdemo
CC_SOURCES        += $(wildcard $(srcdir25)/*.c)
srcdir26            = ./samples/nxpdemos/iotconnectdemo
CC_SOURCES        += $(wildcard $(srcdir26)/*.c)
srcdir27            = ./samples/same54xpro/basic-sample/src
CC_SOURCES        += $(wildcard $(srcdir27)/*.c)
srcdir28            = ./samples/same54xpro/basic-sample
CC_SOURCES        += $(wildcard $(srcdir28)/*.c)
srcdir29            = ./samples/same54xpro/common_hardware_code
CC_SOURCES        += $(wildcard $(srcdir29)/*.c)
srcdir30	    = iotc-azrtos-sdk/authentication/driver/
CC_SOURCES        += $(wildcard $(srcdir30)/*.c)
srcdir31	    = samples/same54xprov2/basic-sample
CC_SOURCES        += $(wildcard $(srcdir31)/*.c)
srcdir32	    = samples/same54xprov2/src/azure_rtos_demo/
CC_SOURCES        += $(wildcard $(srcdir32)/*.c)
srcdir33	    = samples/same54xprov2/src/config/sam_e54_xpro/
CC_SOURCES        += $(wildcard $(srcdir33)/*.c)
srcdir34	    = samples/ck-rx65n/basic-sample/src/
CC_SOURCES        += $(wildcard $(srcdir34)/*.c)
srcdir35	    = samples/rx65ncloudkit/basic-sample/src/
CC_SOURCES        += $(wildcard $(srcdir35)/*.c)

CC_OBJS           = $(CC_SOURCES:%.c=%.o)
CC ?= gcc
INCLUDE_FLAGS += -Iiotc-azrtos-sdk/authentication/driver/ -Isamples/same54xprov2/basic-sample/include/ -I../libTO/Sources/include/ -I./iotc-azrtos-sdk/authentication/include/ -I./iotc-azrtos-sdk/authentication/driver/ -I../netxduo/addons/azure_iot/ -I../netxduo/nx_secure/ports -I../netxduo/nx_secure/inc/ -I../netxduo/crypto_libraries/inc/ -I../netxduo/common/inc -I../netxduo/ports/linux/gnu/inc/ -I../netxduo/addons/sntp/ -I../netxduo/addons/dns/ -I../netxduo/addons/dhcp/ -I../netx/common/inc/ -I../netx/ports/linux/gnu/inc/ -I../threadx/common/inc -I../threadx/ports/linux/gnu/inc/ -I../iotc-c-lib/include -I../iotc-generic-c-sdk/lib/cJSON -I./iotc-azrtos-sdk/azrtos-layer/azrtos-ota/include -I./iotc-azrtos-sdk/azrtos-layer/nx-http-client -I./iotc-azrtos-sdk/azrtos-layer/include -I./iotc-azrtos-sdk/include -I./samples/stm32l4/sensors-demo/src -I./samples/stm32l4/sensors-demo/BSPv2/Include -I./samples/stm32l4/sensors-demo/BSPv2/B-L475E-IOT01 -I./samples/stm32l4/sensors-demo/BSPv2/Components/lps22hb -I./samples/stm32l4/sensors-demo/BSPv2/Components/lis3mdl -I./samples/stm32l4/sensors-demo/BSPv2/Components/lsm6dsl -I./samples/stm32l4/sensors-demo/BSPv2/Components/Common -I./samples/stm32l4/sensors-demo/BSPv2/Components/hts221 -I./samples/stm32l4/sensors-demo/include -I./samples/stm32l4/basic-sample/src -I./samples/stm32l4/basic-sample/include -I./samples/mimxrt1060/basic-sample -I./samples/mimxrt1060/basic-sample/include -I./samples/maaxboardrt/basic-sample/board -I./samples/maaxboardrt/basic-sample/drivers -I./samples/maaxboardrt/basic-sample/include -I./samples/maaxboardrt/basic-sample/phy -I./samples/nxpdemos/overlay/maaxboardrt/board -I./samples/nxpdemos/overlay/maaxboardrt/phy -I./samples/nxpdemos/include -I./samples/same54xpro/basic-sample -I./samples/same54xpro/basic-sample/include -I./samples/same54xpro/common_hardware_code -Isamples/ck-rx65n/basic-sample/include -Isamples/rx65ncloudkit/basic-sample/include -I./samples/rx65ncloudkit/basic-sample/src/smc_gen/general/ -I./samples/rx65ncloudkit/basic-sample/src/smc_gen/r_config/ -I./samples/rx65ncloudkit/basic-sample/src/smc_gen/r_gpio_rx/ -I./samples/rx65ncloudkit/basic-sample/src/smc_gen/r_wifi_sx_ulpgn/ -I./samples/ck-rx65n/netxduo_addons/addons/azure_iot/azure-sdk-for-c/sdk/inc/ -I./samples/ck-rx65n/netxduo_addons/addons/cloud/ -I./samples/ck-rx65n/netxduo_addons/addons/mqtt/ -I./samples/rx65ncloudkit/basic-sample/src/smc_gen/r_bsp/
#CFLAGS += -O -g -Wall -Werror -Wextra -Wshadow -Werror=padded -pedantic
CFLAGS += -O -g -Wall -Werror -Wextra -Wshadow -pedantic
#CPPCHECK_FLAGS = --enable=all --force --inconclusive --library=posix
CPPCHECK_FLAGS = --enable=all --force --inconclusive

.PHONY: cppcheck

all: $(CC_OBJS)

.c.o: $(CC_SOURCES)
	-$(CC) $(INCLUDE_FLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(CC_OBJS)

cppcheck:
	cppcheck $(CPPCHECK_FLAGS) $(INCLUDE_FLAGS) $(CC_SOURCES)

clang-tidy:
	clang-tidy $(CC_SOURCES)
