SUBMISSION = control_code
EXECUTABLE = $(SUBMISSION)
SOURCES    = $(wildcard *.cpp)
OBJECTS    = $(patsubst %,%,${SOURCES:.cpp=.o})
CXX        = g++
CXXFLAGS   = -Wall -g -c -std=c++11 -o
LDFLAGS    = -static-libstdc++
LDLIBS     =
#-------------------------------------------------------------------------------

#make submission zip
release:
	zip ./archiv/$(SUBMISSION)_S1.2.2.zip *.hex

#make clean
clean:
	rm -f ./*.o
	rm -f ./*.o.d
	rm -f $(EXECUTABLE)
	rm -f $(EXECUTABLE).hex
	rm -f ./*.eep
	rm -f ./*.fuse
	rm -f ./*.lock
	rm -f ./*.lss
	rm -f ./*.map
	rm -f ./*.sig

#make cleanw for windows
cleanw:
	del /Q .\*.o
	del /Q .\*.o.d
	del /Q $(EXECUTABLE).exe

#phony
.PHONY: clean cleanw submission

#The dependencies:
-include $(wildcard *.d)
