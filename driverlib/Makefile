CROSS_PREFIX=arm-none-eabi
CC=$(CROSS_PREFIX)-gcc
LD=$(CROSS_PREFIX)-ld
AR=$(CROSS_PREFIX)-ar
OBJCOPY=$(CROSS_PREFIX)-objcopy

LIBRARY=libdriver.a

all:$(LIBRARY)
CFLAGS=-mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -ffunction-sections -fdata-sections -MD -std=c99 -Wall -pedantic -DPART_ -c -I.. -Dgcc

#
# Rules for building the driver library.
#
C_SRCS=
C_SRCS+=adc.c
C_SRCS+=aes.c
C_SRCS+=can.c
C_SRCS+=comp.c
C_SRCS+=cpu.c
C_SRCS+=crc.c
C_SRCS+=des.c
C_SRCS+=eeprom.c
C_SRCS+=emac.c
C_SRCS+=epi.c
C_SRCS+=flash.c
C_SRCS+=fpu.c
C_SRCS+=gpio.c
C_SRCS+=hibernate.c
C_SRCS+=i2c.c
C_SRCS+=interrupt.c
C_SRCS+=lcd.c
C_SRCS+=mpu.c
C_SRCS+=pwm.c
C_SRCS+=qei.c
C_SRCS+=shamd5.c
C_SRCS+=ssi.c
C_SRCS+=sw_crc.c
C_SRCS+=sysctl.c
C_SRCS+=sysexc.c
C_SRCS+=systick.c
C_SRCS+=timer.c
C_SRCS+=uart.c
C_SRCS+=udma.c
C_SRCS+=usb.c
C_SRCS+=watchdog.c

OBJDIR=obj

OBJS=$(addprefix $(OBJDIR)/,$(addsuffix .o,$(C_SRCS)))

$(OBJS):$(OBJDIR)/%.c.o:%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^

$(LIBRARY): $(OBJS)
	$(AR) -cr $@ $^

.phony:clean
clean:
	rm -rf $(LIBRARY) $(OBJDIR)
#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard $(OBJ)/*.d}
endif
