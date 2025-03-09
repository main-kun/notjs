V8_ROOT = $(HOME)/v8

# Compiler
CXX = g++

# Include directories (assumes headers are under ~/v8/include)
INCLUDES = -I$(V8_ROOT) -I$(V8_ROOT)/include

# Compiler flags
CXXFLAGS = $(INCLUDES) -fno-rtti -std=c++20 -DV8_COMPRESS_POINTERS -DV8_ENABLE_SANDBOX -g

# Library directory (assumes V8 libraries are in ~/v8/out.gn/x64.release.sample/obj)
LIBDIR = -L$(V8_ROOT)/out.gn/x64.release.sample/obj

# Libraries and linker flags (using LLVM’s lld)
LDFLAGS = -fuse-ld=lld $(LIBDIR) -lv8_monolith -lv8_libbase -lv8_libplatform -ldl -pthread

SRC = simple-runner.cc

TARGET = simple-runner

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
