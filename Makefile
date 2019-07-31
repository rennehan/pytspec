LIB_DIR = lib

default: pytspec

pytspec: setup.py pytspec.pyx $(LIB_DIR)/libtspec.a
	python setup.py build && python setup.py install && rm -f pytspec.c && rm -rf build

$(LIB_DIR)/libtspec.a:
	make -C $(LIB_DIR) libtspec.a

clean:
	rm *.so
