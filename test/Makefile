CC = clang++
CFLAGS = -Wall -std=c++11 

EXECUTABLES = main
OBJECTS = test_main.o test_objects.o m_vector_test.o m_alloc_test.o 

main.o: test_main.o m_alloc_test.o m_vector_test.o test_objects.o 
	$(CC) $(CFLAGS) -o $(EXECUTABLES) $(OBJECTS) -lgtest -lpthread
test_main.o: test_main.cpp
	$(CC) $(CFLAGS) -c test_main.cpp
m_alloc_test.o: m_alloc_test.cpp  ../src/m_alloc.h
	$(CC) $(CFLAGS) -c m_alloc_test.cpp
m_vector_test.o: m_vector_test.cpp ../src/m_vector.h
	$(CC) $(CFLAGS) -c m_vector_test.cpp
test_objects.o: test_objects.h test_objects.cpp
	$(CC) $(CFLAGS) -c test_objects.cpp
clean:
	rm $(EXECUTABLES) $(OBJECTS)
