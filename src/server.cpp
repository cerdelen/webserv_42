#include "../includes/server.hpp"

void	Server::failTest( int check, string message )
{
	if (check < 0)
	{
		cerr << RED << message << " < 0! Abort!" << RESET_LINE;
		cerr << RED << "Errno message : " << strerror(errno) << RESET_LINE;
		exit(1);
	}
}

void	Server::acceptConnection( int epollFD )
{
	cout << SKY << __func__ << RESET_LINE;
	
	connecData			*tmp = new connecData();
	struct epoll_event	ev;

	tmp->request.already_sent = 0;
	tmp->request.fd = 0;
	tmp->isCGI = false;
	tmp->socket = accept(serverSocket, (SA *) NULL, NULL);
	failTest(tmp->socket, "accept new Socket");
	tmp->finishedResponse = false;
	tmp->finishedRequest = false;
	connections.push_back(tmp);
	ev = createEpollStruct(tmp->socket, EPOLLIN );
	epoll_ctl(epollFD, EPOLL_CTL_ADD, ev.data.fd, &ev);
}

void	Server::readconfigFile(char * confPath)
{
	std::ifstream							confStream;
	std::string								appendedString;
	std::string								workingLine;
	std::pair<std::string, std::string>		pair_;
	std::vector<std::string>				vec_;
	std::string								tmp;

	cout << confPath << endl;
	confStream.open(confPath);
	if(confStream.is_open())
		cout << "is open" << endl;
	cout << "readconfigFile before whole "<< endl;
	while(std::getline(confStream, workingLine, '\n'))
	{
		// cout << "in while " << workingLine << endl;	
		if(workingLine.size() == 0)
			continue ;
		if (workingLine.compare("</server>") == 0)
		{
			cout << "following is full appended string" << endl;
			cout << appendedString << endl;
			cout << "end of full appended string" << endl;
			config tmp(appendedString);
			servConfig.insert(std::make_pair(tmp.getServName(), tmp));
			appendedString.erase();
		}
		appendedString.append(workingLine + "\n");
	}
	// if (error)
	// {

	// 	exit(1);
	// }
}

void	Server::servAddressInit( void )
{
	cout << PINK << __func__ << RESET_LINE;
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);			// SOCK_STREAM == TCP

	int	option = 1;

	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option));

	serverAddress.sin_family		= AF_INET;							// means IPv4 / AD_INET6 =IPv6 
	// serverAddress.sin_port			= htons(PORT_NBR);					// Used port
	serverAddress.sin_port			= htons(ft_atoi(getConfig("default").getPort().c_str()));					// Used port
	// serverAddress.sin_addr.s_addr	= htonl(INADDR_LOOPBACK);			// Address this socket is litening to
	// serverAddress.sin_addr.s_addr	= htonl((unsigned int)ft_atoi(servConfig.getHost().c_str()));			// Address this socket is litening to
	serverAddress.sin_addr.s_addr	= inet_addr(getConfig("default").getHost().c_str());			// Address this socket is litening to

	cout << INADDR_LOOPBACK << endl;

	failTest(serverSocket, "Server Socket");

	failTest(bind(serverSocket, (SA *) &serverAddress, sizeof(serverAddress)),
				"Binding Server Socket");

	failTest(listen(serverSocket, SOCKET_BACKLOG),						// SOMAXCONN???
				"listen() of Server Socket");
}; // probably here address

string	Server::getPossibleType(string type, bool first)
{
	cout << PINK << __func__ << RESET_LINE;
	std::map<string, string>::iterator it = possibleTypes.find(type);

	if(first && it != possibleTypes.end())
	{
		return (*it).first;
	}else if (it != possibleTypes.end()){
		return (*it).second;
	}
	string n;
	return n;
}

void	Server::fillInPossibleTypes()
{
	cout << PINK << __func__ << RESET_LINE;

	possibleTypes.insert(make_pair(string(".aac"), string("audio/aac\r\n")));
	possibleTypes.insert(make_pair(string(".aac"), string("audio/aac\r\n")));
	possibleTypes.insert(make_pair(string(".aac"), string("audio/aac\r\n")));
	possibleTypes.insert(make_pair(string(".aac"), string("audio/aac\r\n")));
	possibleTypes.insert(make_pair(string(".mp4"), string("video/mp4\r\n")));
	possibleTypes.insert(make_pair(string(".mp4"), string("video/mp4\r\n")));
	possibleTypes.insert(make_pair(string(".mp4"), string("video/mp4\r\n")));
	possibleTypes.insert(make_pair(string(".abw"), string("application/x-abiword\r\n")));
	possibleTypes.insert(make_pair(string(".abw"), string("application/x-abiword\r\n")));
	possibleTypes.insert(make_pair(string(".arc"), string("application/octet-stream\r\n")));
	possibleTypes.insert(make_pair(string(".arc"), string("application/octet-stream\r\n")));
	possibleTypes.insert(make_pair(string(".avi"), string("video/x-msvideo\r\n")));
	possibleTypes.insert(make_pair(string(".avi"), string("video/x-msvideo\r\n")));
	possibleTypes.insert(make_pair(string(".azw"), string("application/vnd.amazon.ebook\r\n")));
	possibleTypes.insert(make_pair(string(".bin"), string("application/octet-stream\r\n")));
	possibleTypes.insert(make_pair(string(".bz" ), string("application/x-bzip\r\n")));
	possibleTypes.insert(make_pair(string(".bz2"), string("application/x-bzip2\r\n")));
	possibleTypes.insert(make_pair(string(".csh"), string("application/x-csh\r\n")));
	possibleTypes.insert(make_pair(string(".css"), string("text/css\r\n")));
	possibleTypes.insert(make_pair(string(".csv"), string("text/csv\r\n")));
	possibleTypes.insert(make_pair(string(".doc"), string("application/msword\r\n")));
	possibleTypes.insert(make_pair(string(".epub"), string("application/epub+zip\r\n")));
	possibleTypes.insert(make_pair(string(".gif" ), string("image/gif\r\n")));
	possibleTypes.insert(make_pair(string(".htm" ), string("text/html\r\n")));
	possibleTypes.insert(make_pair(string(".html"), string("text/html\r\n")));
	possibleTypes.insert(make_pair(string(".ico" ), string("image/x-icon\r\n")));
	possibleTypes.insert(make_pair(string(".ics" ), string("text/calendar\r\n")));
	possibleTypes.insert(make_pair(string(".jar" ), string("Temporary Redirect\r\n")));
	possibleTypes.insert(make_pair(string(".jpeg"), string("image/jpeg\r\n")));
	possibleTypes.insert(make_pair(string(".jpg"),  string("image/jpeg\r\n")));
	possibleTypes.insert(make_pair(string(".js"  ), string( "application/js\r\n")));
	possibleTypes.insert(make_pair(string(".json"), string( "application/json\r\n")));
	possibleTypes.insert(make_pair(string(".mid" ), string("audio/midi\r\n")));
	possibleTypes.insert(make_pair(string(".midi"), string("audio/midi\r\n")));
	possibleTypes.insert(make_pair(string(".mpeg"), string("video/mpeg\r\n")));
	possibleTypes.insert(make_pair(string(".mpkg"), string("application/vnd.apple.installer+xm)l\r\n")));
	possibleTypes.insert(make_pair(string(".odp" ), string("application/vnd.oasis.opendocument.pr)esentation\r\n")));
	possibleTypes.insert(make_pair(string(".ods" ), string( "application/vnd.oasis.opendocument.spreadsheet\r\n")));
	possibleTypes.insert(make_pair(string(".odt" ), string( "application/vnd.oasis.opendocument.text\r\n")));
	possibleTypes.insert(make_pair(string(".oga" ), string("audio/ogg\r\n")));
	possibleTypes.insert(make_pair(string(".ogv" ), string( "video/ogg\r\n")));
	possibleTypes.insert(make_pair(string(".ogx" ), string("application/ogg\r\n")));
	possibleTypes.insert(make_pair(string(".png" ), string("image/png\r\n")));
	possibleTypes.insert(make_pair(string(".pdf" ), string("application/pdf\r\n")));
	possibleTypes.insert(make_pair(string(".ppt" ), string( "application/vnd.ms-powerpoint\r\n")));
	possibleTypes.insert(make_pair(string(".rar" ), string( "application/x-rar-compressed\r\n")));
	possibleTypes.insert(make_pair(string(".rtf" ), string( "application/rtf\r\n")));
	possibleTypes.insert(make_pair(string(".sh"  ), string( "application/x-sh\r\n")));
	possibleTypes.insert(make_pair(string(".svg" ), string( "image/svg+xml\r\n")));
	possibleTypes.insert(make_pair(string(".swf" ), string("application/x-shockwave-flash\r\n")));
	possibleTypes.insert(make_pair(string(".tar" ), string( "application/x-tar\r\n")));
	possibleTypes.insert(make_pair(string(".tif" ), string( "image/tiff\r\n")));
	possibleTypes.insert(make_pair(string(".tiff"), string( "image/tiff\r\n")));
	possibleTypes.insert(make_pair(string(".ttf" ), string( "application/x-font-ttf\r\n")));
	possibleTypes.insert(make_pair(string(".txt" ), string( "text/plain\r\n")));
	possibleTypes.insert(make_pair(string(".vsd" ), string( "application/vnd.visio\r\n")));
	possibleTypes.insert(make_pair(string(".wav" ), string( "audio/x-wav\r\n")));
	possibleTypes.insert(make_pair(string(".weba"), string( "audio/webm\r\n")));
	possibleTypes.insert(make_pair(string(".webm"), string( "video/webm\r\n")));
	possibleTypes.insert(make_pair(string(".webp"), string( "image/webp\r\n")));
	possibleTypes.insert(make_pair(string(".woff"), string( "application/x-font-woff\r\n")));
	possibleTypes.insert(make_pair(string(".xhtml"), string( "application/xhtml+xml\r\n")));
	possibleTypes.insert(make_pair(string(".xls" ), string( "application/vnd.ms-excel\r\n")));
	possibleTypes.insert(make_pair(string(".xml" ), string( "application/xml\r\n")));
	possibleTypes.insert(make_pair(string(".xul" ), string( "application/vnd.mozilla.xul+xml\r\n")));
	possibleTypes.insert(make_pair(string(".zip" ), string( "application/zip\r\n")));
	possibleTypes.insert(make_pair(string(".3gp" ), string( "video/3gpp audio/3gpp\r\n")));
	possibleTypes.insert(make_pair(string(".3g2" ), string( "video/3gpp2 audio/3gpp2\r\n")));
	possibleTypes.insert(make_pair(string(".7z"), string("application/x-7z-compressed\r\n")));
	
	possibleReturnCode.insert(make_pair(string("100"), string("Continue")));
	possibleReturnCode.insert(make_pair(string("101"), string("Switching protocols")));
	possibleReturnCode.insert(make_pair(string("102"), string("Processing")));
	possibleReturnCode.insert(make_pair(string("103"), string("Early Hints")));
	possibleReturnCode.insert(make_pair(string("200"), string("OK")));
	possibleReturnCode.insert(make_pair(string("201"), string("Created")));
	possibleReturnCode.insert(make_pair(string("202"), string("Accepted")));
	possibleReturnCode.insert(make_pair(string("203"), string("Non-Authoritative Information")));
	possibleReturnCode.insert(make_pair(string("204"), string("No Content")));
	possibleReturnCode.insert(make_pair(string("205"), string("Reset Content")));
	possibleReturnCode.insert(make_pair(string("206"), string("Partial Content")));
	possibleReturnCode.insert(make_pair(string("207"), string("Multi-Status")));
	possibleReturnCode.insert(make_pair(string("208"), string("Already Reported")));
	possibleReturnCode.insert(make_pair(string("226"), string("IM Used")));
	possibleReturnCode.insert(make_pair(string("300"), string("Multiple Choices")));
	possibleReturnCode.insert(make_pair(string("301"), string("Moved Permanently")));
	possibleReturnCode.insert(make_pair(string("302"), string("Found")));
	possibleReturnCode.insert(make_pair(string("303"), string("See Other")));
	possibleReturnCode.insert(make_pair(string("304"), string("Not Modified")));
	possibleReturnCode.insert(make_pair(string("305"), string("Use Proxy")));
	possibleReturnCode.insert(make_pair(string("306"), string("Switch Proxy")));
	possibleReturnCode.insert(make_pair(string("307"), string("Temporary Redirect")));
	possibleReturnCode.insert(make_pair(string("308"), string("Permanent Redirect")));
	possibleReturnCode.insert(make_pair(string("400"), string("Bad Request")));
	possibleReturnCode.insert(make_pair(string("401"), string("Unauthorized")));
	possibleReturnCode.insert(make_pair(string("402"), string("Payment Required")));
	possibleReturnCode.insert(make_pair(string("403"), string("Forbidden")));
	possibleReturnCode.insert(make_pair(string("404"), string("Not Found")));
	possibleReturnCode.insert(make_pair(string("405"), string("Method Not Allowed")));
	possibleReturnCode.insert(make_pair(string("406"), string("Not Acceptable")));
	possibleReturnCode.insert(make_pair(string("407"), string("Proxy Authentication Required")));
	possibleReturnCode.insert(make_pair(string("408"), string("Request Timeout")));
	possibleReturnCode.insert(make_pair(string("409"), string("Conflict")));
	possibleReturnCode.insert(make_pair(string("410"), string("Gone")));
	possibleReturnCode.insert(make_pair(string("411"), string("Length Required")));
	possibleReturnCode.insert(make_pair(string("412"), string("Precondition Failed")));
	possibleReturnCode.insert(make_pair(string("413"), string("Payload Too Large")));
	possibleReturnCode.insert(make_pair(string("414"), string("URI Too Long")));
	possibleReturnCode.insert(make_pair(string("415"), string("Unsupported Media Type")));
	possibleReturnCode.insert(make_pair(string("416"), string("Range Not Satisfiable")));
	possibleReturnCode.insert(make_pair(string("417"), string("Expectation Failed")));
	possibleReturnCode.insert(make_pair(string("418"), string("I'm a Teapot")));
	possibleReturnCode.insert(make_pair(string("421"), string("Misdirected Request")));
	possibleReturnCode.insert(make_pair(string("422"), string("Unprocessable Entity")));
	possibleReturnCode.insert(make_pair(string("423"), string("Locked")));
	possibleReturnCode.insert(make_pair(string("424"), string("Failed Dependency")));
	possibleReturnCode.insert(make_pair(string("425"), string("Too Early")));
	possibleReturnCode.insert(make_pair(string("426"), string("Upgrade Required")));
	possibleReturnCode.insert(make_pair(string("428"), string("Precondition Required")));
	possibleReturnCode.insert(make_pair(string("429"), string("Too Many Requests")));
	possibleReturnCode.insert(make_pair(string("431"), string("Request Header Fields Too Large")));
	possibleReturnCode.insert(make_pair(string("451"), string("Unavailable For Legal Reasons")));
	possibleReturnCode.insert(make_pair(string("500"), string("Internal Server Error")));
	possibleReturnCode.insert(make_pair(string("501"), string("Not Implemented")));
	possibleReturnCode.insert(make_pair(string("502"), string("Bad Gateway")));
	possibleReturnCode.insert(make_pair(string("503"), string("Service Unavailable")));
	possibleReturnCode.insert(make_pair(string("504"), string("Gateway Timeout")));
	possibleReturnCode.insert(make_pair(string("505"), string("HTTP Version Not Supported")));
	possibleReturnCode.insert(make_pair(string("506"), string("Variant Also Negotiates")));
	possibleReturnCode.insert(make_pair(string("507"), string("Insufficient Storage")));
	possibleReturnCode.insert(make_pair(string("508"), string("Loop Detected")));
	possibleReturnCode.insert(make_pair(string("510"), string("Not Extended")));
	possibleReturnCode.insert(make_pair(string("511"), string("Network Authentication Required")));

	possibleCGIPaths.insert(make_pair(string("py"), string("/usr/bin/python3")));
	possibleCGIPaths.insert(make_pair(string("sh"), string("/bin/sh")));
	possibleCGIPaths.insert(make_pair(string("pl"), string("/usr/bin/perl")));
	possibleCGIPaths.insert(make_pair(string("php"), string("/usr/bin/php")));

}

void Server::fillScriptsCGI(void)
{
	cout << PINK << __func__ << RESET_LINE;
	scriptsCGI.insert(scriptsCGI.end(), "/cgi-bin/i_stole_your_password.py");
	scriptsCGI.insert(scriptsCGI.end(), "/cgi-bin/magic.py");
	scriptsCGI.insert(scriptsCGI.end(), "/cgi-bin/perl.pl");
	scriptsCGI.insert(scriptsCGI.end(), "/cgi-bin/print_env_variables.py");
	scriptsCGI.insert(scriptsCGI.end(), "/cgi-bin/put_photo_in_cat.py");
	scriptsCGI.insert(scriptsCGI.end(), "/cgi-bin/directory_listing.sh");
}

config	&Server::getConfig( std::string hostName)
{
	cout << PINK << __func__ << RESET_LINE;
	try
	{
		return(servConfig.at(hostName));
	}
	catch(const std::exception& e)
	{
		return(servConfig["default"]);
	}
}


// bool	Server::getConfigOutcome( void )
// {
// 	cout << PINK << __func__ << RESET_LINE;
// 	// return(servConfig.getOutcome());
// 	return true;
// }

