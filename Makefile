
CROSS_PREFIX=arm-none-eabi
CC=$(CROSS_PREFIX)-gcc
LD=$(CROSS_PREFIX)-ld
OBJCOPY=$(CROSS_PREFIX)-objcopy
TI_SDK_DIR=$(shell readlink -f ~/Downloads/TM4C/)
INCLUDE_DIRS= $(addprefix -I,$(TI_SDK_DIR))
CFLAGS= -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -ffunction-sections -fdata-sections -MD -std=c99 -Wall -pedantic -DPART_TM4C123GH6PM -c $(INCLUDE_DIRS)


LD_FLAGS=-T lscript.ld --entry ResetISR --gc-sections

C_SRCS=
C_SRCS+=blinky.c
C_SRCS+=startup_gcc.c

OBJDIR=obj
AFX=test.afx
BIN=$(patsubst %.afx,%.bin,$(AFX))

OBJS=$(addprefix $(OBJDIR)/,$(addsuffix .o,$(C_SRCS)))
.phony:all flash clean

all:$(BIN)

$(OBJS):$(OBJDIR)/%.c.o:%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^


$(AFX):$(OBJS)
	$(LD) $(LD_FLAGS) -o $@ $^


$(BIN): $(AFX)
	$(OBJCOPY) -O binary  $^ $@

clean:
	rm -rf $(OBJDIR) $(BIN) $(AFX)
flash:$(BIN)
	lm4flash $^
