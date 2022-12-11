#include "../includes/server.hpp"

// #include <filesystem>

std::vector<connecData*>::iterator		Server::findStructVectorIt( struct epoll_event ev)
{
	std::vector<connecData*>::iterator	it = connections.begin();
	std::vector<connecData*>::iterator	it_e = connections.end();

	for (; it != it_e; it++)
	{
		if(ev.data.fd == (*it)->socket)
			break ;
	}
	return (it);
}

void	Server::stopInvaldiRequest( struct epoll_event ev )
{
	cout << GREEN << __func__ << RESET_LINE;
	std::vector<connecData*>::iterator	it = findStructVectorIt(ev);

	// close((*it)->response.body_fd);		// fd to body of response
	if(it != connections.end())
	{
		delete (*it);
		connections.erase(it);
	}
	epoll_ctl(epollFD, EPOLL_CTL_DEL, ev.data.fd, &ev);
	close(ev.data.fd);
}

void	Server::endResponse( struct epoll_event ev )
{
	cout << SKY << __func__ << RESET_LINE;
	
	std::vector<connecData*>::iterator	it = findStructVectorIt(ev);
	if((*it)->response.body_fd != 0)
	{
		close((*it)->response.body_fd);	
	}
	delete (*it);
	connections.erase(it);
	epoll_ctl(epollFD, EPOLL_CTL_DEL, ev.data.fd, &ev);
	close(ev.data.fd);					// fd for the response socket
	if((*it)->request.file_two != 0)
	{
		fclose((*it)->request.file_two);
	}
	// cout << PURPLE << (*it)->response.body_fd << RESET_LINE;
}

void	Server::confusedEpoll( struct epoll_event ev )
{
	std::vector<connecData*>::iterator	it = findStructVectorIt(ev);

	if ((*it)->finishedRequest && (*it)->finishedResponse)
		endResponse(ev);
	else if ((*it)->finishedRequest)
		doneReadingRequest(ev, it);
	cout << RED << "confusedEpoll i dont know what to do!" << RESET_LINE;	
}


/*
	I THINK THIS ONE IS NOT BEING USED

	string Server::makeHeader(long bodySize, string &path) //prolly other stuff too
	{
		string		out;
		string extension;


		int i = path.length() - 1;
		while(path[i] && path[i] != '.')
		{
			i--;
		}
		if(!path[i])
		{
			//Default extension if there is none 
			extension = "text";
		}else{
			extension.append(path.substr(i, path.length() - i));
		}
		std::cout << "extension thingy " << extension << std::endl; 

		// out.append(path);
		out = "Content-Type: " + extension + " ; Content-Transfer-Encoding: binary; content-length: " + std::to_string(bodySize) + ";charset=ISO-8859-4 ";
		return (out);
	}

*/

/*
	I THINK THIS ONE IS NOT BEING USED
	
	string		Server::getBinary(string &path, long *size, int request_soc)
	{

		FILE	*file_stream;
		if(path.compare("/") == 0)
		{
			//Root path for welcome page
			file_stream = fopen(DEFAULT_PATH , "rb");
		}else if(path.compare("/favicon.ico") == 0)
		{
			//Favicon for now streamlined
			file_stream = fopen(FAV_ICON_PATH, "rb");
		}
		else{
			//If there is a different file user wants to open
			file_stream = fopen(("."+path).c_str(), "rb");
		}
		if(file_stream == nullptr)
		{
			//For errors
			file_stream = fopen(ERROR_404_PATH, "rb");
			path = "/database/Error_404.png";
		}
		string binaryString;
		fseek(file_stream, 0, SEEK_END);
		*size = ftell(file_stream);
		cout << YELLOW << *size << RESET_LINE;
		rewind(file_stream);
		std::vector<char>  binaryVector;
		binaryVector.resize(*size);
		fread(&binaryVector[0], 1, *size, file_stream);
		
		for(long i = 0; i < *size; i++)
		{
			binaryString.push_back(binaryVector[i]);
		}
		cout << GREEN << binaryString.size() << RESET_LINE;
		return binaryString;
	}
*/