#FX3_LIB = ./lib
FX3_DIR = ./include

CXXFLAGS += -std=c++11 -Wall -Wextra -Wfatal-errors

CXXFLAGS += -fPIC
CXXFLAGS += -I../include -I../common -I../../hw -I$(FX3_DIR) -I$(LD_LIBRARY_PATH)
CXXFLAGS += $(CONFIGS)
CXXFLAGS += -DDUMP_PERF_STATS

LDFLAGS += -shared -pthread
LDFLAGS += -L. -lusb-1.0

SRCS = vortex.cpp ../common/vx_utils.cpp ./lib/libcyusb.cpp ./fx3_memory.cpp ./fx3_fpga_wrapper.cpp

# Debugigng
ifdef DEBUG
	CXXFLAGS += -g -O0
else    
	CXXFLAGS += -O2 -DNDEBUG
endif

PROJECT = libvortex.so

all: $(PROJECT)

$(PROJECT): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

clean:
	DESTDIR=../../driver/fx3 $(MAKE) -C $(FX3_DIR) clean
	rm -rf libfx3.so $(PROJECT) *.o