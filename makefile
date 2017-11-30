c=g++ -std=c++11
web_server:Parse.o error.o respond.o php_parse.o w_s.o threadpool.o Socket.o Epoll.o tool.o
	g++ -o web_server threadpool.o Parse.o error.o php_parse.o respond.o w_s.o Socket.o Epoll.o tool.o -l pthread
threadpool.o:threadpool.cpp 
	g++ -c threadpool.cpp
Parse.o:Parse.cpp
	g++ -c Parse.cpp 
error.o:error.cpp 
	g++ -c error.cpp 
handle_file.o:handle_file.cpp 
	g++ -c handle_file.cpp 
respond.o:respond.cpp 
	g++ -c respond.cpp 
w_s.o:w_s.cpp 
	g++ -c w_s.cpp
php_parse.o:php_parse.cpp 
	g++ -c php_parse.cpp 
Socket.o:Socket.cpp Socket.h
	gcc -c Socket.cpp
Epoll.o:Epoll.cpp Epoll.h 
	gcc -c Epoll.cpp
tool.o:tool.cpp tool.h
	gcc -c tool.cpp
clean:
	rm -rf *.o
