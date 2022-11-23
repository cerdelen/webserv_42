#include <sys/select.h>
#include <unistd.h>
#include <iostream>


/*
	WARNING: select() can monitor only file descriptors  numbers  that  are
       less  than  FD_SETSIZE (1024)—an unreasonably low limit for many modern
       applications—and this limitation will not change.  All modern  applica‐
       tions  should instead use poll(2) or epoll(7), which do not suffer this
       limitation.

*/
int main(void)
{
	fd_set			rd;
	struct timeval	tv;
	int				error;
	int				exampleFD = 0;
	/*
		FD_ZERO()
			This macro clears (removes all file descriptors from)  set.   It
			should  be employed as the first step in initializing a file de‐
			scriptor set.
	*/
	FD_ZERO(&rd);

	/*	
		FD_SET()
			This macro adds the file descriptor fd to set.   Adding  a  file
			descriptor  that  is  already present in the set is a no-op, and
			does not produce an error.
	*/
	FD_SET(exampleFD, &rd);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	error = select(exampleFD + 1, &rd, NULL, NULL, NULL); //if I put NULL as last argument it never timesout

	if (error == 0) //timeout
	{
		std::cout << "select() timeout!\n";
	}
	else if (error == -1) //failure
	{
		std::cout << "failed to select!\n";		
	}
	else //success
	{
		std::cout << "data is available: " << error << "!\n"; 
	}
}
