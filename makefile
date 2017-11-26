c=g++ -std=c++11
web_server:handle_request.o error.o handle_file.o respond.o w_s.o threadpool.o php_parse.o 
	g++ -o web_server threadpool.o handle_request.o error.o handle_file.o respond.o w_s.o php_parse.o -l pthread
threadpool.o:threadpool.cpp 
	g++ -c threadpool.cpp
handle_request.o:handle_request.cpp
	g++ -c handle_request.cpp 
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
zy_server:zy_server.o Socket.o 
	$c -o zy_server zy_server.o Socket.o 
zy_server.o:zy_server.cpp
	$c -c zy_server.cpp 
Socket.o:Socket.cpp Socket.h
	$c -c Socket.cpp
clean:
	rm -rf *.o
