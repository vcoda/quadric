CC=g++
MAGMA_DIR=../magma
BOOST_PFR_DIR=$(MAGMA_DIR)/src/third-party/pfr/include
INCLUDE_DIR=-I$(VULKAN_SDK)/include -I$(MAGMA_DIR) -I$(BOOST_PFR_DIR) -Iinclude -Iinclude/rapid
CONSTEXPR_DEPTH_FLAGS=-ftemplate-depth=2048 -fconstexpr-depth=2048
BASE_CFLAGS=-std=c++17 -m64 -msse4 $(CONSTEXPR_DEPTH_FLAGS) -Werror $(INCLUDE_DIR)

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CFLAGS=$(BASE_CFLAGS) -O0 -g -D_DEBUG
	BUILD_TARGET=libquadricd.a
else
	CFLAGS=$(BASE_CFLAGS) -O3 -DNDEBUG
	BUILD_TARGET=libquadric.a
endif

PCH_HEADER=include/pch.h
SRC_OBJS= \
	src/mesh/mesh.o \
	src/mesh/meshGL.o \
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

PCH=$(PCH_HEADER).gch
DEPS := $(SRC_OBJS:.o=.d)

-include $(DEPS)

%.o: %.cpp
	$(CC) $(CFLAGS) -include $(PCH_HEADER) -c -o $@ $<

quadric: $(PCH) $(SRC_OBJS)
	@echo $(BUILD_TARGET)
	@ar rcs $(BUILD_TARGET) $(SRC_OBJS)

$(PCH): $(PCH_HEADER)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	@find . -iregex '.*\.\(o\|a\)' -delete
	@rm -rf $(PCH)
	@rm -rf $(DEPS)
