CXFLAGS+=-std=c++11 -O3 -fPIC -Wall -DOMX_SKIP64BIT -DRPI3=1 -DVWALL=1 -Wdeprecated-declarations
LDFLAGS+=-L/opt/vc/lib -lbcm_host -lvchostif -lopenmaxil -lbrcmGLESv2 -lbrcmEGL
LDFLAGS+=-L/usr/lib/arm-linux-gnueabihf
LIBS+=-ldl -lpthread
INCLUDES+=-I/opt/vc/include/
INCLUDES+=-I/usr/include/arm-linux-gnueabihf

PHONY: clean demo

demo: demo.cpp dispmanx.cpp
	g++ -o demo $(CXFLAGS) $(INCLUDES) $(LIBS) $(LDFLAGS) demo.cpp dispmanx.cpp

clean:
	rm -f demo
