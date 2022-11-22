#include "../includes/server.hpp"

//Request-Line				= Method SP Request-URI SP HTTP-Version CRLF
//CR = Carriage return		= \r
//LF = Line Feed			= \n

void		server::request( void )
{
	int		requestSocket;
	int		recvReturn;
	char	receivingBuffer[MAX_LINE + 1];

	//select stuff test
	// {
	// 	fd_set	fds, readfds;
	// 	FD_ZERO (&fds);
	// 	FD_SET(serverSocket, &fds);
	// 	int fdmax = serverSocket;

	// }
	while (1)
	{
	cout << "Waiting for a connection on PORT: " << PORT_NBR << endl;
	requestSocket = accept(serverSocket, (SA *) NULL, NULL);
	failTest(requestSocket, "accept() Socket");
	memset(receivingBuffer, 0, MAX_LINE + 1);	
	fullRequest.clear();
	while(((recvReturn = recv(requestSocket, receivingBuffer, MAX_LINE, 0)) > 0))
	{
		failTest(recvReturn, "Reading into receivingBuffer out of requestSocket");
		fullRequest.append(receivingBuffer);
		if (receivingBuffer[recvReturn - 1] == '\n' && receivingBuffer[recvReturn - 2] == '\r')
			break;
		memset(receivingBuffer, 0, MAX_LINE);
	}
	handleRequest(requestSocket, fullRequest);
	failTest(	close(requestSocket),
				"Closing the socket");
	cout << "This is the full Request" << RESET_LINE;
	cout << endl << fullRequest << RED << "<<here is the end>>" << RESET_LINE;
	}
}

config	server::getConfig( void )
{
	return (servConfig);
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

void server::fillResponseStructBinary(std::string &path, int request_soc)
{
	long		bodyLength;
	currResponse.httpVers = HTTPVERSION;
	currResponse.statusMessage = "200 Everything is A-Ok";// still have to do
	currResponse.body = getBinary(path, &bodyLength, request_soc);
	currResponse.headers = makeHeader(bodyLength, path); // still have to do
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

void server::fillRequestLineItems(string &fullRequest)
{
	//get request line
	std::string	requestLine = fullRequest.substr(0, fullRequest.find('\n'));
	std::vector<string> requestLineV = split(requestLine, ' ');
	currRequest.method		= requestLineV[0];
	currRequest.URI			= requestLineV[1];
	currRequest.httpVers	= requestLineV[2];

	// print out request line
	cout << YELLOW << "method = " << currRequest.method << endl;
	cout << "URI = " << currRequest.URI << endl;
	cout << "httpVers = " << currRequest.httpVers << RESET_LINE;
}

void	server::fillRequestHeaders(string &fullRequest)
{
	//get headers
	int begin	= fullRequest.find('\n') + 1;
	int size	= fullRequest.find("\r\n\r\n") - begin;
	std::string	headers = fullRequest.substr(begin, size);
	std::vector<string> headersVector = split(headers, '\n');
	std::vector<string> key_value;
	size_t vectorSize = headersVector.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		key_value = split(headersVector[i], ':', 1);
		currRequest.headers.insert(std::make_pair(key_value[0], key_value[1]));
	}
	
	// print headers to terminal
	for (auto i : currRequest.headers)
	{
		cout << RED << i.first << ": " << i.second << RESET_LINE;
	}
}

void server::fillRequestBody(std::string &fullRequest)
{
	//get body
	size_t	begin;
	size_t	size;
	if (currRequest.headers.end() != currRequest.headers.find("Content-Length")) // or 
	{
		begin		= fullRequest.find("\r\n\r\n") + 4;
		size		= stoi(currRequest.headers.at("Content-Length"));
		std::string	body = fullRequest.substr(begin, size);
		currRequest.body = body;
	}
	// else if (currRequest.headers.end() != currRequest.headers.find("Content-Length"))

	// print body
	cout << PURPLE << currRequest.body << RESET_LINE;
}

void	server::fillRequestStruct(std::string &fullRequest)
{
	fillRequestLineItems(fullRequest);
	fillRequestHeaders(fullRequest);
	fillRequestBody(fullRequest);
}

int	server::checkGetRequest(int requestSocket)
{
	if(file_exists( currRequest.URI) == false)
	{
		// sendResponse(requestSocket, servConfig.getConfigMap().at(ERROR404));
		return (-1);
	}
	return (0);
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

void		server::handleRequest(int requestSocket, std::string &fullRequest)
{
	fillRequestStruct(fullRequest);
	// if (checkRequestErrors(requestSocket) != 0)
		// return ;
	if (currRequest.method.compare("GET") == 0)
	{
		// test for errors
		// send response
		sendResponse(requestSocket, currRequest.URI);
		cout << "its a get request :)" << endl;
	}
	else if (currRequest.method.compare("POST") == 0)
	{
		// test for errors
		// send response
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
		file_stream = fopen(("."+path).c_str(), "rb");
	}
	if(file_stream == nullptr)
	{
		//For errors
		file_stream = fopen(err_path.c_str(), "rb");
		path = "/database/Error_404.png";
	}
	std::string binaryString;
	fseek(file_stream, 0, SEEK_END);
	*size = ftell(file_stream);
	cout << RED << *size << RESET_LINE;
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

bool		server::getConfigOutcome( void )
{
	return(servConfig.getOutcome());
}
