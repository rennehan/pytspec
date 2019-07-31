# If " final link failed: nonrepresentable section on output" happens
# uncomment line below and comment CC = gcc.
# CC = gcc -fPIC
CC = gcc  

default: libtspec.a

libtspec.a: tspec.o
	ar rcs $@ $^
    
tspec.o: tspec.c tspec.h 
	$(CC) -c $<

clean:
	rm *.o *.a
