FTP_Server Multithreaded

BY: 
Abhishek Sarkar		Roll No: 11010101

Ajay Kumar Kilaka	Roll No: 11010103



This project is a simple command line FTP Server.

--------------------------------------------------------------------------------------------------------------------------------------

Features :

1> Multithreaded : The server is multithreaded. It creates a new thread for every client.
					Atmost 10 clients can connect at a time.

2> cd implementaion : The command cd which changes directory at server is implemented
						by storing the absolute path of the server for a particular thread
						in the path attribute of struct THREADINFO.

						This was done because simple chdir(char *) is not thread-safe.
						However even with the current implementation race condition errors may arise.

3> supported commands :	
						The commands supported by the client.out binary (provided server.out is also running) are
						a> !pwd
							Present working directory of client
						
						b> !ls
							Show directory contents of client

						c> !cd path
							Change client directory

						d> pwd
							Present working directory of server. May differ from client to client.

						e> ls
							Show server directory contents

						f> cd path
							Change working server directory to that indicated by path

						g> get fileName
							Download file with FileName at the client

						h> put fileName
							Upload file with fileName at the server

						i> quit
							Exit client, and delete related threadinfo

4> Proper error checking : Comprehensive error checking has been performed. In cases of Errors custom messages
							explaining the cause are generated to some extent. Beyond that only <errno> has
							been used.


-----------------------------------------------------------------------------------------------------------------------------------

Usage :
		unzip the given file and navigate to extracted directory with the terminal. Use given Makefile to
		compile.

		$make

		Now navigate to bin/Server : <cd bin/Server>. Use 	<./server.out>	 to start the server.

		$cd bin/Server
		$./server.out


		Open a new terminal and navigate to the extracted directory. Navigate into bin/Client  : <cd bin/Client>
		Use <./client.out> to start the client.

		$cd bin/Client
		$./client.out


		Now use the supported commands in the client terminal.

		NOTE: <make clean> clears all binaries and object files for rebuilding solution.

------------------------------------------------------------------------------------------------------------------------------------

Sample input :

	ftp>get Assignment_3.doc

Sample output :
	At server terminal:	

		29184 bytes read
		59 packets sent

	At client terminal: 	
				
		29184 bytes written	
		60 packets received




