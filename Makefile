CFLAGS := -Wall -Wextra -std=c99 -D_XOPEN_SOURCE=500 -pedantic -O2
LDFLAGS := -Wl,-O1 -lsqlite3

OBJECTS = db.o main.o
BINARY = watchgit

$(BINARY): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) $(OBJECTS) $(BINARY)

