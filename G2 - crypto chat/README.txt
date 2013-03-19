Christian Benincasa
CS283
Assignment 2 - Encrypted Chat
README

This task entailed creating a rudimentary version of the RSA public key cryptography algorithm. In addition, to show that our implementation works, we were to create a chat server program and a chat client program that exchange public keys and can take part in encrypted chat with each other. 

This program was tested on my local machine running OS X 10.8.2 as well as on the Drexel Tux machines. When compiled, this program requires links to the pthread and math libraries in order to function properly. There are several run configurations for the server and client programs. Those are detailed below.

The server program can take a few command line arguments in when running. The most important of these is the -p option. When set, this option specifies which port the server should open chat communications on. If this argument is not passed, the default port is 4567. In addition, as per the requirements, values for E, C, D, and DC can be passed into the server. Although one can pass these parameters when running the chat server, I personally did not implement any functionality with these parameters and chose to leave all the encrypting and decrypting of messages on the client-side.

The client program takes in several parameters that are important, as well. The two most important parameters for the chat client are the -s (server/host) and -p (port) parameters. The -s parameter is required as it tells the client where to find the chat server. The -p option, like the -p option for the server, takes in an integer that will be used as the port number to connect to the server and defaults to 4567. The client program can also take in values for the public and private key values: E, C, D, and DC. If these values aren't specified when running the program, the user will be prompted to enter two numbers which will then be used to generate a new public and private key for the session.

This program was tested in Xcode extensively to find any possible breakpoints. The program itself can be a bit flimsy at times in terms of getting two clients to connect. The task is not as simple as it sounds. At this time, two clients can connect, but after one client disconnects, I was unable to get a new client to connect to the already online client. I know this is possible, but it began to get extremely complex and I realized that the program instructions didn't detail this functionality. For all intents and purposes, this is a functioning chat program in that it can connect two clients together. The server must be running before clients connect. The program runs best when one client is initiated until no more prompts are given and then the next client is started. 

Also, a supplementary program is provided that can generate or crack keys. This program can be run via the required make targets and will return either a generated public/private key set based on two numbers inputted or a cracked private key when ran in crack mode.

In addition, when having the second client connect, sometimes the program will terminate and return a Floating Point exception. I could not locate the cause of the issue nor could I pinpoint which parameters would cause this to happen. There is no logic within the crypt.c library that would result in floating point numbers being tossed around or division by zero. 

Overall, this part of the assignment took me an extremely long time to complete (>10 hours) and I would hope there would be a bit more guidance (maybe some skeleton code) for students in the future.