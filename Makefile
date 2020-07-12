CC=g++
INCLUDE_DIR=-I$(VULKAN_SDK)/include -Iinclude -Iinclude/rapid
BASE_CFLAGS=-std=c++14 -m64 -msse4 -Werror $(INCLUDE_DIR)

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CFLAGS=$(BASE_CFLAGS) -O0 -g -D_DEBUG
	TARGET=libquadricd.a
else
	CFLAGS=$(BASE_CFLAGS) -O3 -DNDEBUG
	TARGET=libquadric.a
endif

OBJS= \
	src/bezierPatch.o \
	src/cube.o \
	src/cylinder.o \
	src/knot.o \
	src/plane.o \
	src/quadric.o \
	src/sincosTable.o \
	src/sphere.o \
	src/teapot.o \
	src/torus.o

DEPS := $(OBJS:.o=.d)

-include $(DEPS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

quadric: $(OBJS)
	@echo $(TARGET)
	@ar rcs $(TARGET) $(OBJS)

clean:
	@find . -name '*.o' -delete
	@find . -name '*.a' -delete
	@rm -rf $(DEPS)
