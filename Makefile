CC ?= cc
CFLAGS ?= -O2
GTK_FLAGS := $(shell pkg-config --cflags --libs gtk+-3.0)

libgtk-save-enter.so: gtk-save-enter.c
	$(CC) $(CFLAGS) -std=c11 -Wall -Wextra -Werror -fPIC -shared -Wl,-z,relro,-z,now $< -o $@ $(GTK_FLAGS)
