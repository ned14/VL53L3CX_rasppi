CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
RM = rm

CFLAGS = -O1 -g -Wall -c

OUTPUT_DIR = bin
OBJ_DIR = obj

TARGET_LIB = $(OUTPUT_DIR)/libVL53L3CX_rasppi.a

INCLUDES = \
	-I. \
	-I./core/inc \
	-I./platform/inc

VPATH = \
	./core/src \
	./platform/src/

LIB_SRCS = \
	vl53lx_api.c \
	vl53lx_api_calibration.c \
	vl53lx_api_core.c \
	vl53lx_api_preset_modes.c \
	vl53lx_core.c \
	vl53lx_core_support.c \
	vl53lx_dmax.c \
	vl53lx_hist_algos_gen3.c \
	vl53lx_hist_algos_gen4.c \
	vl53lx_hist_char.c \
	vl53lx_hist_core.c \
	vl53lx_hist_funcs.c \
	vl53lx_nvm.c \
	vl53lx_register_funcs.c \
	vl53lx_sigma_estimate.c \
	vl53lx_silicon_core.c \
	vl53lx_wait.c \
	vl53lx_xtalk.c \
	vl53lx_platform.c

LIB_OBJS  = $(LIB_SRCS:%.c=$(OBJ_DIR)/%.o)

EXAMPLES_SRC = \
  example/Src/main.c

EXAMPLES_BIN = $(EXAMPLES_SRC:example/Src/%.c=$(OUTPUT_DIR)/%)


.PHONY: all
all: ${TARGET_LIB}

$(TARGET_LIB): $(LIB_OBJS)
	mkdir -p $(dir $@)
	$(AR) -rcs $@ $^

$(OBJ_DIR)/%.o:%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

$(EXAMPLES_BIN): bin/%:example/Src/%.c
	mkdir -p $(dir $@)
	$(CC) -L$(OUTPUT_DIR) $^ -lVL53L3CX_rasppi  $(INCLUDES) -o $@

example:${OUTPUT_DIR} ${TARGET_LIB} $(EXAMPLES_BIN)

.PHONY: clean
clean:
	-${RM} -rf ./$(OUTPUT_DIR)/*  ./$(OBJ_DIR)/*


