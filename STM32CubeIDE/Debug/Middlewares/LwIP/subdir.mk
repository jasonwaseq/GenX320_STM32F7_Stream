################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

LWIP_BASE := C:/Users/jason/Documents/GenX320_stream/Middlewares/Third_Party/LwIP

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
$(LWIP_BASE)/src/api/api_lib.c \
$(LWIP_BASE)/src/api/api_msg.c \
$(LWIP_BASE)/src/api/err.c \
$(LWIP_BASE)/src/api/netbuf.c \
$(LWIP_BASE)/src/api/netdb.c \
$(LWIP_BASE)/src/api/netifapi.c \
$(LWIP_BASE)/src/api/tcpip.c \
$(LWIP_BASE)/src/core/def.c \
$(LWIP_BASE)/src/core/dns.c \
$(LWIP_BASE)/src/core/inet_chksum.c \
$(LWIP_BASE)/src/core/init.c \
$(LWIP_BASE)/src/core/ip.c \
$(LWIP_BASE)/src/core/ipv4/autoip.c \
$(LWIP_BASE)/src/core/ipv4/dhcp.c \
$(LWIP_BASE)/src/core/ipv4/etharp.c \
$(LWIP_BASE)/src/core/ipv4/icmp.c \
$(LWIP_BASE)/src/core/ipv4/igmp.c \
$(LWIP_BASE)/src/core/ipv4/ip4.c \
$(LWIP_BASE)/src/core/ipv4/ip4_addr.c \
$(LWIP_BASE)/src/core/ipv4/ip4_frag.c \
$(LWIP_BASE)/src/core/mem.c \
$(LWIP_BASE)/src/core/memp.c \
$(LWIP_BASE)/src/core/netif.c \
$(LWIP_BASE)/src/core/pbuf.c \
$(LWIP_BASE)/src/core/raw.c \
$(LWIP_BASE)/src/core/stats.c \
$(LWIP_BASE)/src/core/sys.c \
$(LWIP_BASE)/src/core/tcp.c \
$(LWIP_BASE)/src/core/tcp_in.c \
$(LWIP_BASE)/src/core/tcp_out.c \
$(LWIP_BASE)/src/core/timeouts.c \
$(LWIP_BASE)/src/core/udp.c \
$(LWIP_BASE)/src/netif/ethernet.c \
$(LWIP_BASE)/system/OS/sys_arch.c

C_DEPS += \
./Middlewares/LwIP/api_lib.d \
./Middlewares/LwIP/api_msg.d \
./Middlewares/LwIP/err.d \
./Middlewares/LwIP/netbuf.d \
./Middlewares/LwIP/netdb.d \
./Middlewares/LwIP/netifapi.d \
./Middlewares/LwIP/tcpip.d \
./Middlewares/LwIP/def.d \
./Middlewares/LwIP/dns.d \
./Middlewares/LwIP/inet_chksum.d \
./Middlewares/LwIP/init.d \
./Middlewares/LwIP/ip.d \
./Middlewares/LwIP/autoip.d \
./Middlewares/LwIP/dhcp.d \
./Middlewares/LwIP/etharp.d \
./Middlewares/LwIP/icmp.d \
./Middlewares/LwIP/igmp.d \
./Middlewares/LwIP/ip4.d \
./Middlewares/LwIP/ip4_addr.d \
./Middlewares/LwIP/ip4_frag.d \
./Middlewares/LwIP/mem.d \
./Middlewares/LwIP/memp.d \
./Middlewares/LwIP/netif.d \
./Middlewares/LwIP/pbuf.d \
./Middlewares/LwIP/raw.d \
./Middlewares/LwIP/stats.d \
./Middlewares/LwIP/sys.d \
./Middlewares/LwIP/tcp.d \
./Middlewares/LwIP/tcp_in.d \
./Middlewares/LwIP/tcp_out.d \
./Middlewares/LwIP/timeouts.d \
./Middlewares/LwIP/udp.d \
./Middlewares/LwIP/ethernet.d \
./Middlewares/LwIP/sys_arch.d

OBJS += \
./Middlewares/LwIP/api_lib.o \
./Middlewares/LwIP/api_msg.o \
./Middlewares/LwIP/err.o \
./Middlewares/LwIP/netbuf.o \
./Middlewares/LwIP/netdb.o \
./Middlewares/LwIP/netifapi.o \
./Middlewares/LwIP/tcpip.o \
./Middlewares/LwIP/def.o \
./Middlewares/LwIP/dns.o \
./Middlewares/LwIP/inet_chksum.o \
./Middlewares/LwIP/init.o \
./Middlewares/LwIP/ip.o \
./Middlewares/LwIP/autoip.o \
./Middlewares/LwIP/dhcp.o \
./Middlewares/LwIP/etharp.o \
./Middlewares/LwIP/icmp.o \
./Middlewares/LwIP/igmp.o \
./Middlewares/LwIP/ip4.o \
./Middlewares/LwIP/ip4_addr.o \
./Middlewares/LwIP/ip4_frag.o \
./Middlewares/LwIP/mem.o \
./Middlewares/LwIP/memp.o \
./Middlewares/LwIP/netif.o \
./Middlewares/LwIP/pbuf.o \
./Middlewares/LwIP/raw.o \
./Middlewares/LwIP/stats.o \
./Middlewares/LwIP/sys.o \
./Middlewares/LwIP/tcp.o \
./Middlewares/LwIP/tcp_in.o \
./Middlewares/LwIP/tcp_out.o \
./Middlewares/LwIP/timeouts.o \
./Middlewares/LwIP/udp.o \
./Middlewares/LwIP/ethernet.o \
./Middlewares/LwIP/sys_arch.o

LWIP_CFLAGS := -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DUSE_BPP=16 -DSTM32F746xx -DDEBUG -c \
 -I../../Core/Inc \
 -I../../Drivers/STM32F7xx_HAL_Driver/Inc \
 -I../../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy \
 -I../../Middlewares/Third_Party/FreeRTOS/Source/include \
 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 \
 -I../../Drivers/CMSIS/Device/ST/STM32F7xx/Include \
 -I../../Drivers/CMSIS/Include \
 -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 \
 -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS \
 -I../../Drivers/BSP/STM32746G-Discovery \
 -I../../Drivers/BSP/Components/ft5336 \
 -I"C:/Users/jason/Documents/GenX320_stream/STM32CubeIDE/Application/User" \
 -I"C:/Users/jason/Documents/GenX320_stream/STM32CubeIDE/Middlewares/FreeRTOS" \
 -I"C:/Users/jason/Documents/GenX320_stream/STM32CubeIDE/Application/User/rtos" \
 -I"C:/Users/jason/Documents/GenX320_stream/STM32CubeIDE/Application/User/Core" \
 -I"C:/Users/jason/Documents/GenX320_stream/STM32CubeIDE/Application/User/firmware" \
 -I../../LWIP/App \
 -I../../LWIP/Target \
 -I../../Middlewares/Third_Party/LwIP/src/include \
 -I../../Middlewares/Third_Party/LwIP/system \
 -I../../Drivers/BSP/Components/lan8742 \
 -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity \
 -MMD -MP --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb

# Each subdirectory must supply rules for building sources it contributes
./Middlewares/LwIP/%.o: $(LWIP_BASE)/src/api/%.c Middlewares/LwIP/subdir.mk
	arm-none-eabi-gcc "$<" $(LWIP_CFLAGS) -MF"$(@:%.o=%.d)" -MT"$@" -o "$@"

./Middlewares/LwIP/%.o: $(LWIP_BASE)/src/core/%.c Middlewares/LwIP/subdir.mk
	arm-none-eabi-gcc "$<" $(LWIP_CFLAGS) -MF"$(@:%.o=%.d)" -MT"$@" -o "$@"

./Middlewares/LwIP/%.o: $(LWIP_BASE)/src/core/ipv4/%.c Middlewares/LwIP/subdir.mk
	arm-none-eabi-gcc "$<" $(LWIP_CFLAGS) -MF"$(@:%.o=%.d)" -MT"$@" -o "$@"

./Middlewares/LwIP/%.o: $(LWIP_BASE)/src/netif/%.c Middlewares/LwIP/subdir.mk
	arm-none-eabi-gcc "$<" $(LWIP_CFLAGS) -MF"$(@:%.o=%.d)" -MT"$@" -o "$@"

./Middlewares/LwIP/sys_arch.o: $(LWIP_BASE)/system/OS/sys_arch.c Middlewares/LwIP/subdir.mk
	arm-none-eabi-gcc "$<" $(LWIP_CFLAGS) -MF"$(@:%.o=%.d)" -MT"$@" -o "$@"

clean: clean-Middlewares-2f-LwIP

clean-Middlewares-2f-LwIP:
	-$(RM) ./Middlewares/LwIP/*.cyclo ./Middlewares/LwIP/*.d ./Middlewares/LwIP/*.o ./Middlewares/LwIP/*.su

.PHONY: clean-Middlewares-2f-LwIP
