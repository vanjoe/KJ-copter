
CROSS_PREFIX=arm-none-eabi
CC=$(CROSS_PREFIX)-gcc
LD=$(CROSS_PREFIX)-ld
AR=$(CROSS_PREFIX)-ar
OBJCOPY=$(CROSS_PREFIX)-objcopy

CFLAGS= -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -DTARGET_IS_BLIZZARD_RB1 -Dgcc
CFLAGS+=-mfloat-abi=softfp -ffunction-sections -fdata-sections
CFLAGS+=-MD -std=c99 -Wall -pedantic -DPART_TM4C123GH6PM -c -I.

LD_FLAGS=-T lscript.ld --entry ResetISR --gc-sections

LIBS=
LIBS+=driverlib/libdriver.a

C_SRCS=
C_SRCS+=hello.c
C_SRCS+=startup_gcc.c
C_SRCS+=utils/uartstdio.c


AFX=test.afx
BIN=$(patsubst %.afx,%.bin,$(AFX))
OBJDIR=obj
OBJS=$(addprefix $(OBJDIR)/,$(addsuffix .o,$(C_SRCS)))
.phony:all flash clean

all:$(BIN)

$(OBJS):$(OBJDIR)/%.c.o:%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^

.phony:$(LIBS)
$(LIBS):
	make -C $(dir $@)

$(AFX):$(OBJS) $(LIBS)
	$(LD) $(LD_FLAGS) -o $@ $^


$(BIN): $(AFX)
	$(OBJCOPY) -O binary  $^ $@

clean:
	rm -rf $(OBJDIR) $(BIN) $(AFX)
	for d in $(LIBS);do make -C $$(dirname $$d) clean;done
flash:$(BIN)
	lm4flash $^


#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard $(OBJ)/*.d}
endif
