VPATH = ../src
out_test: m_alloc_test.o
	clang++ -std=c++11 -o out_test m_alloc_test.o ../src/m_alloc.o
m_alloc_test.o: m_alloc_test.cpp
	clang++ -c -std=c++11 m_alloc_test.cpp
m_alloc.o: ../src/m_alloc.h ../src/m_alloc.cpp
	clang++ -c -std=c++11 m_alloc.cpp
clean:
	rm out_test m_alloc_test.o
