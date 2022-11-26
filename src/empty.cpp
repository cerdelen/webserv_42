#include "../includes/server.hpp"

s_connecData	initConnectionStruct(int fd)
{
	s_connecData	out;

	out.socket = fd;
	return (out);
}

void	server::acceptConnection( void )
{
	s_connecData	tmp = initConnectionStruct(accept(serverSocket, (SA *) NULL, NULL));
	failTest(tmp.socket, "accept new Socket");
	connections[currConnections] = tmp;
	currConnections++;
}


struct epoll_event	createEpollStruct(int fdSocket, uint32_t flags)
{
	struct	epoll_event	ev;

	ev.events = flags;
	ev.data.fd = fdSocket;

	return (ev);
}

void		server::requestLoop( void )
{
	struct epoll_event	ev;
	struct epoll_event	events[MAX_EVENTS];

	int					epollFD = epoll_create1(0);
	int					readyFDs;
	failTest(epollFD, "epoll_create1");

	ev = createEpollStruct(serverSocket, EPOLLIN);

	epoll_ctl(epollFD, EPOLL_CTL_ADD, ev.data.fd, &ev);

	while(1)
	{
		cout << "calling epoll_wait" << endl;
		readyFDs = epoll_wait(epollFD, events, MAX_EVENTS, -1);
		failTest(readyFDs, "epoll_wait");

		cout << "epoll_wait call successful" << endl;
		for (int idx = 0; idx < readyFDs; idx++)
		{
			if (events[idx].data.fd == serverSocket)				//accepting a connection
			{
				acceptConnection();
			}
			else if (events[idx].events & (EPOLLRDHUP | EPOLLHUP))	// check for end of connection
			{
				// closeAndRemoveFromEpoll();
			}
			else if (events[idx].events & ( EPOLLOUT ))				// check for writing fd
			{

			}
			else if (events[idx].events & ( EPOLLIN ))				// check for reading fd
			{

			}
			else
			{
				// beConfused()
				cout << RED << "Why did we get here. FD: " << events[idx].data.fd << RESET_LINE;
			}
			cout << "currConnections: " << currConnections << endl;
		}
	}

}