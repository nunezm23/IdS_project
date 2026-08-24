APP := ads1263_app
BUILD_DIR := build

SRC := \
	main/main.c \
	components/logger/src/app_log.c \
	components/ads1263/src/ads1263.c \
	components/ads1263/arch/linux/src/ads1263_spi_linux.c \
	components/ads1263/arch/linux/src/ads1263_gpio_linux.c \
	components/acquisition/src/acquisition.c

OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))
DEP := $(OBJ:.o=.d)

CC ?= gcc
PKG_CONFIG ?= pkg-config

CPPFLAGS += \
	-Icomponents/logger/include \
	-Icomponents/ads1263/include \
	-Icomponents/ads1263/arch/linux/include \
	-Icomponents/acquisition/include

CFLAGS ?= -O2 -g
CFLAGS += -std=c11 -Wall -Wextra -pedantic -MMD -MP -D_POSIX_C_SOURCE=200809L

ifeq ($(NO_GPIOD),1)
CPPFLAGS += -DADS1263_NO_GPIOD=1
GPIOD_CFLAGS :=
GPIOD_LIBS :=
else
GPIOD_CFLAGS := $(shell $(PKG_CONFIG) --cflags libgpiod 2>/dev/null)
GPIOD_LIBS := $(shell $(PKG_CONFIG) --libs libgpiod 2>/dev/null || echo -lgpiod)
endif

CPPFLAGS += $(GPIOD_CFLAGS)
LDLIBS += $(GPIOD_LIBS)

.PHONY: all clean deploy print-toolchain

all: $(BUILD_DIR)/$(APP)

$(BUILD_DIR)/$(APP): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

deploy: $(BUILD_DIR)/$(APP)
	@if [ -z "$(TARGET)" ]; then \
		echo "Usage: make deploy TARGET=root@192.168.0.9"; \
		exit 2; \
	fi
	scp $(BUILD_DIR)/$(APP) $(TARGET):~/

print-toolchain:
	@echo "CC=$(CC)"
	@echo "CFLAGS=$(CFLAGS)"
	@echo "CPPFLAGS=$(CPPFLAGS)"
	@echo "LDLIBS=$(LDLIBS)"

clean:
	rm -rf $(BUILD_DIR)

-include $(DEP)
