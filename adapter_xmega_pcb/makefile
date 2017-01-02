SUBMISSION = adapter_xmega
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
	zip ./archiv/$(SUBMISSION)_v1.zip $(SUBMISSION).pcb $(SUBMISSION).prj $(SUBMISSION).sch *.gbr *.drl *.pdf

#make clean
clean:
	rm -f ./*.gbr
	rm -f ./*.drl
	rm -f ./*.txt
	rm -f ./*.mop
	rm -f ./*.pdf


#The dependencies:
-include $(wildcard *.d)
