default: all

GOOGLE_TEST_LIB = gtest
GOOGLE_TEST_INCLUDE = /usr/local/include
JSONLA_INCLUDE = ../
G++ = g++
G++_FLAGS = -c -Wall -I$(GOOGLE_TEST_INCLUDE) -I$(JSONLA_INCLUDE)
LD_FLAGS = -L /usr/local/lib -l $(GOOGLE_TEST_LIB) -l pthread

OBJECTS = main.o jsonla_test.o jsonla.o
TARGET = jsonla_test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(G++) -o $(TARGET) $(OBJECTS) $(LD_FLAGS)
	./jsonla_test

%.o: %.cc
	$(G++) $(G++_FLAGS) $<

jsonla.o:
	$(G++) $(G++_FLAGS) ../jsonla.cc -o jsonla.o

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean
