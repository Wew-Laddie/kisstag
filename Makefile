ifeq ($(OS),Windows_NT)
	LINK_LIBS = -lshlwapi
	OUTPUT_FILE = kisstag.exe
else
	OUTPUT_FILE = kisstag
endif

OUTPUT_LIB = libkisstag.a

CFLAGS = -static-libgcc -g -Wall -Wextra -Iinclude -L.
LIB_OBJS = src/libkisstag/image.o src/libkisstag/crc.o src/libkisstag/os.o src/libkisstag/png.o src/libkisstag/util.o
KT_OBJS = src/kisstag/main.o

.PHONY: all
all: $(OUTPUT_FILE)

$(OUTPUT_LIB): $(LIB_OBJS)
	$(AR) rcs -o $@ $^

$(OUTPUT_FILE): $(OUTPUT_LIB) $(KT_OBJS)
	$(CC) $(CFLAGS) $(KT_OBJS) -lkisstag $(LINK_LIBS) -o $@

.PHONY: clean
clean:
	-rm -rf $(LIB_OBJS) $(KT_OBJS) $(OUTPUT_FILE) $(OUTPUT_LIB)