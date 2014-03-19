CC=gcc

OBJECTDIR=objects
BINDIR=bin

OBJECTS=$(OBJECTDIR)/includes.o

all: clean client server

client: bdir includes
	$(CC) client.c $(OBJECTS) -o $(BINDIR)/$@ -pthread

server: bdir includes
	$(CC) server.c $(OBJECTS) -o $(BINDIR)/$@ -pthread

test: bdir
	$(CC) test.c $(OBJECTS) -o $(BINDIR)/$@ -pthread

includes: odir
	$(CC) includes.c -c -o $(OBJECTDIR)/$@.o -pthread

clean:
	rm -f \
		$(OBJECTDIR)/includes.o \
		$(BINDIR)/server \
		$(BINDIR)/client \
		$(BINDIR)/test

odir:
	mkdir -p $(OBJECTDIR)

bdir:
	mkdir -p $(BINDIR)
