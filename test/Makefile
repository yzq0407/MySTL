CC = clang++
CFLAGS = -Wall -O3 -std=c++14 

EXECUTABLES = main
OBJECTS = test_main.o test_objects.o m_vector_test.o m_alloc_test.o m_list_test.o m_traits_test.o

main.o: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECUTABLES) $(OBJECTS) -lgtest -lpthread
test_main.o: test_main.cpp
	$(CC) $(CFLAGS) -c test_main.cpp
m_alloc_test.o: m_alloc_test.cpp  ../src/m_alloc.h
	$(CC) $(CFLAGS) -c m_alloc_test.cpp
m_vector_test.o: m_vector_test.cpp ../src/m_vector.h
	$(CC) $(CFLAGS) -c m_vector_test.cpp
m_list_test.o: m_list_test.cpp
	$(CC) $(CFLAGS) -c m_list_test.cpp
m_traits_test.o: m_traits_test.cpp
	$(CC) $(CFLAGS) -c m_traits_test.cpp

test_objects.o: test_objects.h test_objects.cpp
	$(CC) $(CFLAGS) -c test_objects.cpp
clean:
	rm $(EXECUTABLES) $(OBJECTS)
