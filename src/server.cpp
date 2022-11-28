#include "../includes/server.hpp"
	//Request-Line				= Method SP Request-URI SP HTTP-Version CRLF
	//CR = Carriage return		= \r
	//LF = Line Feed			= \n


//register events of fd to epfd
static void epoll_ctl_add(int epfd, int fd, uint32_t events)
{
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
	{
		cout << "Error number: " << errno << " - " << strerror(errno) << endl;
		

		exit(1);
	}
}

void		server::requestLoop( void )
{
	int		idx;
	int		requestSocket;
	int		recvReturn;
	char	receivingBuffer[MAX_LINE + 1];
	
	
	// int epollFD = epoll_create1(0);
	// failTest(epollFD, "epoll_create1");

	// struct epoll_event events[MAX_EVENTS]; //should be a define
	// epoll_ctl_add(epollFD, serverSocket, EPOLLIN | EPOLLOUT | EPOLLET);

	// int nfds;
	while(true)
	{
		// nfds = epoll_wait(epollFD, events, MAX_EVENTS, -1); //-1 blocks forever, 0 never blocks in both examples they use -1? 
		// cout << YELLOW << "Number of fds ready: " << nfds << RESET_LINE;
		// for (idx = 0; idx < nfds; idx++)
		{
			// cout << RED << "IDX: " << idx << RESET_LINE;
 			// if (events[idx].data.fd == serverSocket) //ready to accept a new connection
			{
				cout << "Waiting for a connection on PORT: " << PORT_NBR << endl;
				requestSocket = accept(serverSocket, (SA *) NULL, NULL);
				failTest(requestSocket, "accept() Socket");
				// epoll_ctl_add(epollFD, requestSocket, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP);
			}
			// else if(events[idx].events & EPOLLIN) /* handle EPOLLIN event */
			{
				// cout << RED << "ELSE IF" << RESET_LINE;
				memset(receivingBuffer, 0, MAX_LINE + 1);	
				fullRequest.clear();
				// while(((recvReturn = recv(events[idx].data.fd , receivingBuffer, MAX_LINE, 0)) > 0))
				while(((recvReturn = recv(requestSocket , receivingBuffer, MAX_LINE, 0)) > 0))
				{
					fullRequest.append(receivingBuffer, recvReturn);
					if (recvReturn < MAX_LINE)
					{
						memset(receivingBuffer, 0, MAX_LINE);
						break;
					}
					memset(receivingBuffer, 0, MAX_LINE);
					cout << PURPLE << "Inside else if: " << recvReturn << RESET_LINE;
				}
				failTest(recvReturn, "recv() call to read from requestSocket");
				// handleRequest(events[idx].data.fd , fullRequest);
				handleRequest(requestSocket, fullRequest);
				// failTest(close(events[idx].data.fd ), "Sending answer to Request to requestSocket");
				// epoll_ctl(epollFD, EPOLL_CTL_DEL, events[idx].data.fd, NULL);
				failTest(close(requestSocket), "Sending answer to Request to requestSocket");
			}

			/* check if the connection is closing */
			// if (events[idx].events & (EPOLLRDHUP | EPOLLHUP))
			// {
			// 	printf("[+] connection closed\n");
			// 	epoll_ctl(epollFD, EPOLL_CTL_DEL, events[idx].data.fd, NULL);
			// 	close(events[idx].data.fd);
			// 	continue;
			// }


		//printing the request
		cout << "This is the full Request" << RESET_LINE;
		// cout << endl << fullRequest << RED << "<<here is the end>>" << RESET_LINE;
		}
	}
}

void	server::sendResponse(int requestSocket, std::string &path)					// im writing this with a get request in mind
{
	std::string		outie;

	fillResponseStructBinary(path,requestSocket);
	outie.append(currResponse.httpVers + " " + 
				currResponse.statusMessage + "\r\n" +
				currResponse.headers + "\r\n\r\n" +
				currResponse.body);

	const char *	responsy = outie.c_str();

	failTest(send(requestSocket, responsy, outie.size(), 0),
				"Sending answer to Request to requestSocket");
}



void	server::failTest( int check, std::string message )
{
	if (check < 0)
	{
		cerr << RED << message << " < 0! Abort!" << RESET_LINE;
		cerr << RED << "Errno message : " << strerror(errno) << RESET_LINE;
		exit(-1);
	}
}

void	server::servAddressInit( void )
{
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);			// SOCK_STREAM == TCP

	int	option = 1;
	
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option));

	serverAddress.sin_family		= AF_INET;							// means IPv4 / AD_INET6 =IPv6 
	serverAddress.sin_port			= htons(PORT_NBR);					// Used port
	serverAddress.sin_addr.s_addr	= htonl(INADDR_LOOPBACK);			// Address this socket is litening to


	failTest(serverSocket, "Server Socket");

	failTest(bind(serverSocket, (SA *) &serverAddress, sizeof(serverAddress)),
				"Binding Server Socket");

	failTest(listen(serverSocket, SOCKET_BACKLOG),						// SOMAXCONN???
				"listen() of Server Socket");
}; // probably here address



int	server::checkGetRequest(int requestSocket)
{
	if(file_exists( currRequest.URI) == false)
	{
		// sendResponse(requestSocket, servConfig.getConfigMap().at(ERROR404));
		return (-1);
	}
	return (0);
}


void server::fillResponseStructBinary(std::string &path, int request_soc)
{
	long		bodyLength;
	currResponse.httpVers = HTTPVERSION;
	currResponse.statusMessage = "200 Everything is A-Ok";// still have to do
	currResponse.body = getBinary(path, &bodyLength, request_soc);
	cout << YELLOW << "filleResponseStructBInary: " << path << RESET_LINE;
	currResponse.headers = makeHeader(bodyLength, path); // still have to do
}

int	server::checkRequestErrors(int requestSocket)
{
	// if (currRequest.method.compare("GET") == 0)
	// 	return (checkGetRequest(requestSocket));
	// else if (currRequest.method.compare("POST") == 0)
	// 	return (checkPostRequest());
	// else if (currRequest.method.compare("DELETE") == 0)
	// 	return (checkDeleteRequest());
	return (-1);
}
void		server::handle_post(int requestSocket, std::string &path, std::string &fullrequest)
{
	//Content-type deduction 
	size_t index = fullRequest.find("Content-Type:");
	std::string content_type;
	// file_name.append(content_type)
	int start = index + 8;
	index = index + 8;
	while(fullRequest[index] && fullRequest[index] != '\n')
	{
		index++;
	}
	content_type = fullRequest.substr(start, index);
	std::cout << content_type << std::endl;
	if(path.compare(0, 8,"/uploads") == 0)
	{
		std::string file_name = "file";
		std::string extension;
		int i;
		for(i = content_type.length() - 1; content_type[i - 1] && content_type[i] && content_type[i - 1] != '/'; i--)
		{
		}
		extension = content_type.substr(i, content_type.length() - 1);
		// std::ofstream file("./uploads/" + file_name + "." + extension,  std::ios::out | std::ios::binary);
		std::ofstream file("./uploads/file.png",  std::ios::out | std::ios::binary);
		file << currRequest.body;
		write(requestSocket, "200 OK\r\n", 7);
		file.close();
		// std::cout << "tak tutaj mozesz wrzucac " << extension <<  std::endl;

	}else{
		std::cout << "Wrong path mate" << std::endl;
	}
}

#include <stdlib.h> //for putenv
 #include <sys/wait.h> //for wait
void		server::checkURI(int requestSocket)
{
	string CGIPath = "/cgi-bin/put_photo_in_cat.py?intra_login=";
	int equalStatus = currRequest.URI.compare(0, CGIPath.length(), CGIPath);

	if (equalStatus == 0)
	{
		string intraName = split(currRequest.URI, '=')[1];
		cout << GREEN << intraName << RESET_LINE;
		string envVariable = "INTRA_NAME=" + intraName;
		putenv((char *)envVariable.c_str());
		cout<< RED << getenv("INTRA_NAME") << RESET_LINE;
		pid_t pid = fork();
		failTest(pid, "forking for cgi");
		if (pid == 0)
		{
			// string path = "/workspaces/webserv_42/cgi-bin/" + intraName + ".html";

			// std::ofstream file("./cgi-bin/cerdelen.html");
			// file.open(path,std::ios_base::out);
			// if (file.is_open())
				// cout << RED << "OPEN" << RESET_LINE;
			// else
				// cout <<RED << path << RESET_LINE;
			// file.close();
			char **env;
			env = (char **)calloc(2, sizeof(char *));
			env[0] = strdup(("INTRA_NAME=" + intraName).c_str());
			int err = execve("./cgi-bin/put_photo_in_cat.py", NULL, env);
			perror("CGI");
			cout << RED  << strerror(errno) << RESET_LINE;
			exit(-69);
		}
		wait(NULL);
		string path = "/cgi-bin/" + intraName + ".html";
		currRequest.URI = path;
		
	}
	cout << PURPLE << currRequest.URI.compare(0, CGIPath.length(), CGIPath) << RESET_LINE;
}

void		server::handleRequest(int requestSocket, std::string &fullRequest)
{
	fillRequestStruct(fullRequest);
	// if (checkRequestErrors(requestSocket) != 0)
		// return ;
	if (currRequest.method.compare("GET") == 0)
	{
		// test for errors
		// send response
		checkURI(requestSocket);
		cout << YELLOW << currRequest.URI << RESET_LINE;
		sendResponse(requestSocket, currRequest.URI);
		cout << "its a get request :)" << endl;
	}
	else if (currRequest.method.compare("POST") == 0)
	{
		// test for errors
		// send response
		handle_post(requestSocket, currRequest.URI, fullRequest);
		cout << "its a post request :)" << endl;
	}
	else if (currRequest.method.compare("DELETE") == 0)
	{
		// test for errors
		// send response
		cout << "its a delete request :)" << endl;
	}
	else
	{
		cout << RED << currRequest.method << RESET_LINE;
		cout << "We should throw an error code with a message that we do not support this method" << endl;
	}
	currRequest.headers.clear();
	currRequest.body.clear();
	// currRequest.httpVers.clear();
	// currRequest.method.clear();
	// currRequest.URI.clear();
}

std::string		server::getBinary(std::string &path, long *size, int request_soc)
{

	FILE	*file_stream;
	std::string def_path("./database/default_index.html");
	std::string fav_path("./database/favicon.ico");
	std::string err_path("./database/Error_404.png");
	if(path.compare("/") == 0)
	{
		//Root path for welcome page
		file_stream = fopen(def_path.c_str() , "rb");
	}else if(path.compare("/favicon.ico") == 0)
	{
		//Favicon for now streamlined
		file_stream = fopen(fav_path.c_str(), "rb");
	}
	else{
		//If there is a different file user wants to open
		cout << YELLOW << "From getBInary: " << path << RESET_LINE;
		file_stream = fopen(("."+path).c_str(), "rb");
	}
	if(file_stream == nullptr)
	{
		//For errors
		cout << RED << "COULDN'T OPEN" << RESET_LINE;
		perror("open: ");
		file_stream = fopen(err_path.c_str(), "rb");
		path = "/database/Error_404.png";
	}
	std::string binaryString;
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

std::string server::makeHeader(long bodySize, std::string &path) //prolly other stuff too
{
	std::string		out;
	std::string extension;


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
	cout << YELLOW << path << RESET_LINE;
	std::cout << "extension thingy " << extension << std::endl; 
	// out.append(path);
	out = "Content-Type: " + extension + " ; Content-Transfer-Encoding: binary; Content-Length: " + std::to_string(bodySize) + ";charset=ISO-8859-4 ";
	return (out);
}


void			server::fillInPossibleTypes()
{
	//Text types
	// possible_types.insert(".html","text/html");
	// possible_types.insert(".css","text/css");
	// possible_types.insert(".csv","text/csv");
	// possible_types.insert(".js","text/javascript");
	// possible_types.insert(".txt","text/plain");
	// possible_types.insert(".xml","text/xml");

	// //Video
	// possible_types.insert(".mpeg","video/mpeg");
	// possible_types.insert(".mp4","video/mp4");
	// possible_types.insert(".mov","video/quicktime");
	// possible_types.insert(".wmv","video/x-ms-wmv");
	// possible_types.insert(".avi","x-msvideo");


}


config	&server::getConfig( void )
{
	return (servConfig);
}


bool	server::getConfigOutcome( void )
{
	return(servConfig.getOutcome());
}
