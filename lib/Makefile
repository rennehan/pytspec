CC = gcc

default: libtspec.a

libtspec.a: tspec.o
	ar rcs $@ $^
    
tspec.o: tspec.c tspec.h 
	$(CC) -c $<

clean:
	rm *.o *.a
