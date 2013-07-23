CFLAGS := -Wall -Wextra -std=c99 -D_XOPEN_SOURCE=500 -pedantic -O2
LDFLAGS := -Wl,-O1
LIBS = -lsqlite3

OBJECTS = db.o gitfuncs.o main.o
BINARY = watchgit

$(BINARY): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) $(OBJECTS) $(BINARY)

