web_server:handle_request.o error.o handle_file.o respond.o w_s.o 
	g++ -o web_server handle_request.o error.o handle_file.o respond.o w_s.o 

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

clean:
	rm -rf *.o
