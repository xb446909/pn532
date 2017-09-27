CC      = gcc
CCFLAGS  = -g -Wall -O
INCLUDEFLAGS = -Iinc
LDFLAGS = 
TARGETS = nfc_pn532

SOURCES := $(shell find . -name *.c)

OBJS := $(SOURCES:.c=.o)

.PHONY:all 
all : $(TARGETS)

nfc_pn532:$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o:%.c
	$(CC) -o $@ -c $< $(CCFLAGS) $(INCLUDEFLAGS)

%.d:%.c
	@set -e; rm -f $@; $(CC) -MM $< $(INCLUDEFLAGS) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(OBJS:.o=.d)

.PHONY:clean 
clean:
	find . -name *.o -or -name *.d -or -name *.d.* | xargs rm -f $(TARGETS)
