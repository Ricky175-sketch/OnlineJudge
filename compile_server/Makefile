.PHONY:clean

compile_server:compile_server.cc
	g++ -o $@ $^ -std=c++11 -ljsoncpp -lpthread

clean:
	rm -f compile_server
