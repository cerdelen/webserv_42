#include "../includes/server.hpp"

void		Server::requestLoop( void )
{
	cout << SKY << __func__ << RESET_LINE;
	
	struct epoll_event	ev;
	struct epoll_event	events[MAX_EVENTS];
	int					readyFDs;

	epollFD = epoll_create1(0);
	failTest(epollFD, "epoll_create1");

	ev = createEpollStruct(serverSocket, EPOLLIN);
	epoll_ctl(epollFD, EPOLL_CTL_ADD, ev.data.fd, &ev);

	// cout << "epollFD: " << epollFD << endl << "serverSocket: " << serverSocket << endl;
	while(1)
	{
		// cout << "calling epoll_wait" << endl;
		readyFDs = epoll_wait(epollFD, events, MAX_EVENTS, -1);
		failTest(readyFDs, "epoll_wait");

		// cout << "epoll_wait call successful" << endl;
		for (int idx = 0; idx < readyFDs; idx++)
		{
			if (events[idx].data.fd == serverSocket)				//accepting a connection
			{
				// cout << "IDX: " << idx << " socket: " << events[idx].data.fd << " case 1" << endl;
				acceptConnection(epollFD);
			}
			else if (events[idx].events & (EPOLLRDHUP | EPOLLHUP))	// check for end of connection
			{
				// cout << "IDX: " << idx << " socket: " << events[idx].data.fd << " case 2" << endl;
				// closeAndRemoveFromEpoll(events[idx]);
				endConnection(events[idx]);
			}
			else if (events[idx].events & ( EPOLLOUT ))				// check for  write() fd
			{
				// cout << PURPLE << "IDX: " << idx << " socket: " << events[idx].data.fd << " case 3" << endl;
				sendResponse(events[idx]);

			}
			else if (events[idx].events & ( EPOLLIN ))				// check for read() fd
			{
				// cout << "IDX: " << idx << " socket: " << events[idx].data.fd << " case 4" << endl;
				readRequest(events[idx]);
				// closeAndRemoveFromEpoll(events[idx], epollFD);  //testing purposes
			}
			else
			{
				// beConfused()
				cout << RED << "Why did we get here. FD: " << events[idx].data.fd << RESET_LINE;
			}
			// cout << "currConnections: " << connections.size() << endl;
			// for (size_t i = 0; i < connections.size(); i++)
			// {
			// 	cout << "ind: " << i << " socket: " << connections[i]->socket << endl;
			// }
		}
		if(DEBUG)
			cout << "end of main loop" << endl;
	}
}

void	Server::readRequest( struct epoll_event ev )
{
	cout << SKY << __func__ << RESET_LINE;

	std::vector<connecData*>::iterator	it = findStructVectorIt(ev);
	char								recBuffer[MAX_LINE];
	int									recReturn;


	memset(recBuffer, 0, MAX_LINE);	
	// fullRequest.clear();
	failTest(recReturn = recv(ev.data.fd , recBuffer, MAX_LINE, 0), "recReturn in do requeststuff");
	if (recReturn < 0)
	{
		endConnection(ev);
	}
	(*it)->request.raw.append(recBuffer, recReturn);
	if (recReturn < MAX_LINE) 
	{
		// std::cout << "Raw Souce " << endl << (*it)->request.raw;
		// std::cout << "Body " << (*it)->request.body << " End of body"<<std::endl;
		if(doneReadingRequest(ev, it) == true)
		{
			if ((*it)->request.URI.compare(0, strlen(CGI_FOLDER_PATH), CGI_FOLDER_PATH) == 0) //CGI
			{
				handleCGI(ev, it);
				cout << "DONE WITH CGI " << endl;
			}
			else
			{
				prepareResponseHeader(it, ev); //CAN WE GET THIS IN THE SEND RESPONSE
			}
		}//CAN WE GET THIS BLOCK IN THE SEND RESPONSE
	}
	// done reading close event and open new writing event
}



bool	Server::doneReadingRequest( struct epoll_event ev, std::vector<connecData*>::iterator it )
{
	cout << SKY << __func__ << RESET_LINE;
	epoll_ctl(epollFD, EPOLL_CTL_DEL, ev.data.fd, &ev);
	if (parseRequest(ev) == false)
	{
		// if (DEBUG)
			cout << "parseRequest == false" << endl;
		return false;
	}
			cout << "parseRequest == true" << endl;
	ev = createEpollStruct((*it)->socket, EPOLLOUT);
	epoll_ctl(epollFD, EPOLL_CTL_ADD, ev.data.fd, &ev);
	(*it)->finishedRequest = true;

	return true;
	// cout << RED << "doneReadingRequest and this is the uri: " << (*(findStructVectorIt(ev)))->request.URI << RESET_LINE;
	// connections.push_back(ev);
	return true;
}

void	Server::prepareResponseHeader( std::vector<connecData*>::iterator it ,struct epoll_event	ev)
{
	cout << SKY << __func__ << RESET_LINE;
	// parse and send header to client
	// open fd into the (*it)->response.body_fd for the body
	if((*it)->request.method.compare("DELETE") == 0)
	{
		handleDelete(it, ev);
		endConnection(ev);
	}
	else if((*it)->request.method.compare("POST") == 0)
	{
		handlePost(it, ev);
	}else if((*it)->request.method.compare("GET") == 0){
		// cout << "response header get if" << endl;
		handleGet(it, ev);
	}
	
}

void	Server::setErrorStatusCodeAndRespond(struct epoll_event	ev, std::vector<connecData*>::iterator it, std::string err)
{
	cout << SKY << __func__ << RESET_LINE;
	(*it)->response.status_code = err;
	createAndSendResponseHeaders(ev, it);
	endConnection(ev);
}
//The function expectecs (*it)->response.status_code 
//From it it will create the html page and send it back to the client 
void	Server::createAndSendResponseHeaders(struct epoll_event	ev, std::vector<connecData*>::iterator it)
{
	cout << SKY << __func__ << RESET_LINE;
	
	cout << (*it)->request.URI << endl;
	// (*it)->response.status_code = statusCode;
	(*it)->response.statusMessage = possibleReturnCode[(*it)->response.status_code];
	(*it)->response.headers = "HTTP/1.1 ";
	(*it)->response.headers.append((*it)->response.status_code);
	(*it)->response.headers.append(" ");
	(*it)->response.headers.append((*it)->response.statusMessage);
	(*it)->response.headers.append("\n");
	std::string response_page ;
	if((*it)->response.status_code.compare("200") == 0)
	{
		(*it)->response.headers.append("Content-Length: ");
		(*it)->response.headers.append((*it)->response.content_lenght_str);
		(*it)->response.headers.append("\n");
		(*it)->response.headers.append("Connection: Closed\n");
		(*it)->response.headers.append("Content-Type: ");
		(*it)->response.headers.append((*it)->response.content_type);
		(*it)->response.headers.append("\n");
	}else if((*it)->response.status_code.compare("404") == 0)
    {
    
        std::ifstream t("./database/error.html");
        std::stringstream buffer;
        buffer << t.rdbuf();
        std::string s = buffer.str();
        (*it)->response.headers.append("Content-Length: ");
        (*it)->response.headers.append("\n");
        std::stringstream ss;
        ss << s.length();
        t.close();
        (*it)->response.headers.append(ss.str());
        (*it)->response.headers.append("Connection: Closed\n");
        (*it)->response.headers.append("Content-Type: ");
        (*it)->response.headers.append("text/html");
        (*it)->response.headers.append("\r\n\r\n");    
        (*it)->response.headers.append(s);
    }else{
		response_page = "<!DOCTYPE html>";
		response_page.append("\n");
		response_page.append("<html lang=\"en\">\n");
		response_page.append("<head>\n");
		response_page.append("<meta charset=\"UTF-8\">\n");
		response_page.append("<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n");
		response_page.append("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
		response_page.append("<title>Status code page</title>\n");
		response_page.append("</head>\n");
		response_page.append("<body>\n");
		response_page.append("<center> <h1>");
		response_page.append((*it)->response.status_code);
		response_page.append(" ");
		response_page.append((*it)->response.statusMessage);
		response_page.append("</h1> </center>");
		response_page.append("</body>\n");
		response_page.append("</html>");
		(*it)->response.headers.append("Content-Length: ");
		std::stringstream ss;
		ss << response_page.length();
		(*it)->response.headers.append(ss.str());
		(*it)->response.headers.append("\n");
		(*it)->response.headers.append("Connection: Closed\n");
		(*it)->response.headers.append("Content-Type: ");
		(*it)->response.headers.append("text/html\r\n\r\n");
		(*it)->response.headers.append(response_page);
	}

	//So we need a simple html page that we send in the body of this response
	//If the response is different than 200 change html 
	//
	// std::cout << "Sending stuff" << std::endl;
	if (send((*it)->socket, (*it)->response.headers.c_str(), (*it)->response.headers.length(), 0) < 0)
	{
		endConnection(ev);
	}

	// if(!((*it)->response.status_code.compare("200") == 0 || (*it)->response.status_code.compare("404") == 0))
	// 	endConnection(*(*it)->ev_p);
	// std::cout << (*it)->response.headers << std::endl;
}

void	Server::sendResponse( struct epoll_event ev )
{
	// cout << SKY << __func__ << RESET_LINE;
	
	std::vector<connecData*>::iterator	it = findStructVectorIt(ev);
	char								sendBuffer[MAX_LINE];
	int									sendReturn;
	
	if ((*it)->request.fd != 0) // process request body
	{
		// std::cout << (*it)->request.body << std::endl;
		// sendReturn = write((*it)->request.fd, (*it)->request.body_in_char, (*it)->request.content_size);

		if((*it)->request.content_size - MAX_LINE <= 0)
		{

			(*it)->request.content_size -= write((*it)->request.fd, (*it)->request.body.c_str() + (*it)->request.already_sent, (*it)->request.content_size);
			endConnection(ev);
		}
		sendReturn = write((*it)->request.fd, (*it)->request.body.c_str() + (*it)->request.already_sent, MAX_LINE);
		(*it)->request.already_sent += sendReturn;
		(*it)->request.content_size -= sendReturn;
	
	}
	else
	{
		memset(sendBuffer, 0, MAX_LINE);
		// cout << "this is do response stuff else  with fd = " << (*it)->response.body_fd << endl;
		sendReturn = read((*it)->response.body_fd, sendBuffer, MAX_LINE);

		// cout << "sendreturn = " << sendReturn << endl <<  sendBuffer << endl << "end of buffer print" <<  endl;

		// failTest(
		sendReturn = send((*it)->socket, sendBuffer, sendReturn, 0);
		if(sendReturn < MAX_LINE)
		{
			endConnection(ev);
		}
	}
}



