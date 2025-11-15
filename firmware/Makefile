# Standard STM32 Make file for BMS controller
# Toolchain: arm-none-eabi-gcc

TARGET = bms_firmware

# Compiler configuration
CC = arm-none-eabi-gcc
CP = arm-none-eabi-objcopy
SZ = arm-none-eabi-size

# Directories
BUILD_DIR = build
SRC_DIR = Core/Src
INC_DIR = Core/Inc

# Source Files
C_SOURCES = \
$(SRC_DIR)/main.c \
$(SRC_DIR)/bq76920.c \
$(SRC_DIR)/mock_hal.c

# Include Paths
C_INCLUDES = \
-I$(INC_DIR)

# Compiler Flags for STM32G0 (Cortex-M0+)
CPU = -mcpu=cortex-m0plus
FPU = 
FLOAT-ABI = 
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

CFLAGS = $(MCU) -Wall -Wextra -O2 -fdata-sections -ffunction-sections $(C_INCLUDES)

# Linker Flags
LDSCRIPT = 
LDFLAGS = $(MCU) -specs=nano.specs -Wl,--gc-sections

# Build Rules
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin

OBJS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(dir $(C_SOURCES))

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "Compiling $<"
	@$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	@echo "Linking $@"
	@$(CC) $(OBJS) $(LDFLAGS) -o $@
	@$(SZ) $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	@$(CP) -O binary $< $@

$(BUILD_DIR):
	mkdir $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
