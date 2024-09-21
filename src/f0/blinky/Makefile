BOARD= nucleo_f091rc
OPTIONS= -p always
BUILD_DIR= build/

.PHONY: all write clean

all:
	@west build $(OPTIONS) -b $(BOARD) .

write:
	@west flash

clean:
	@rm -rf $(BUILD_DIR)/*
