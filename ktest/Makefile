CFLAGS +=	-I/usr/local/include
LDFLAGS+=	-L/usr/local/lib -lvga

keytest: keytest.o key.o log.o
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	-rm key.o log.o keytest.o
