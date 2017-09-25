CXX      = g++
CXXFLAGS  = -Wall -O
INCLUDEFLAGS = -Iinc
LDFLAGS = 
TARGETS = nfc_pn532

SOURCES := $(shell find . -name *.cpp)

OBJS := $(SOURCES:.cpp=.o)

.PHONY:all 
all : $(TARGETS)

nfc_pn532:$(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o:%.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INCLUDEFLAGS)

%.d:%.cpp
	@set -e; rm -f $@; $(CXX) -MM $< $(INCLUDEFLAGS) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(OBJS:.o=.d)

.PHONY:clean 
clean:
	find . -name *.o -or -name *.d -or -name *.d.* | xargs rm -f $(TARGETS)
