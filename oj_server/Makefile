.PHONY:clean

oj_server:oj_server.cc
	g++ -o $@ $^ -I./include -L./lib -std=c++11 -lpthread -lctemplate -ljsoncpp -lmysqlclient

clean:
	rm -f oj_server
