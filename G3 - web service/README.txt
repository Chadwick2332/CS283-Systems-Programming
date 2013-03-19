Christian Benincasa
CS283
Assignment 3 - Web Service
2/27/13

This assignment tasked us with creating a web service in C that instead of returning the contents of a file to a client, the result of a function would be returned. 

This program was tested on both my local machine (running OS X 10.8.2) and Drexel's Tux server cluster. The csapp.h and stringly.h libraries are required for the web service server to run. In order for the program to run correctly, it should be ran on an open port. The default port of the web service is 4567, which is very likely to be open. Other ports tested include 3000, 5000, and 8000. Open ports are circumstantial; to find which ports are open on your current (UNIX) machine, one can run this command: 

	`cat /etc/services | grep Unassigned`
	
In addition, unless you are the root user on the system, you cannot run on any ports <1024. The web service server experiences issues when run on a port that is already assigned.

To run the web service server, the provided Makefile can be used. `make build` will build web_service.o, `make run` will run web_service.o on the default port (4567), and `PORT=<port> make run_port` will run web_service.o on the specified port number.

Provided services:

GET services
	/watch	- loads an embedded youtube video
		Optional parameters:
			vid - specify a YouTube ID to load into the embedded player
			w		- specify the width of the embedded player
			h		- specify the height of the embedded player
	
	/bananas - produces a plain text message stating you have a random number of bananas
	
	/fibonacci - list fibonacci(0) to fibonacci(25)
		Optional parameters
			n 	- specify which fibonacci number to calculate to (will overflow n > 92)

POST services
	/echo - returns a barebones HTML document stating "My name is <name>!" within a header tag.
		Required parameters:
			name - specify which name to echo
	
I sincerely enjoyed this assignment as my beginnings of programming were with the web and I found it fascinating to be learning and coding the services that power the modern web. 