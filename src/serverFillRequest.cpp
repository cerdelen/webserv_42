#include "../includes/server.hpp"

bool	Server::parseRequest( struct epoll_event ev )
{
	cout << SKY << __func__ << RESET_LINE;
	std::vector<connecData*>::iterator	it = findStructVectorIt(ev);
	// validateRequest(ev);
	if ((*it)->request.raw.empty())
	{
		cout << RED << "EMPTY request" << RESET_LINE;
		return(false);
	}
	fillRequestLineItems(it);
	cout << "debugging uri 1 = " << (*it)->request.URI << endl;
	URIisDirectory((*it)->request);
	cout << "debugging uri 2 = " << (*it)->request.URI << endl;
	fillRequestHeaders(it);
	cout << "debugging uri 3 = " << (*it)->request.URI << endl;
	fillRequestBody(it);
	cout << GREEN << (*it)->request.cgi_data << RESET_LINE;
	cout << "Body length: " << (*it)->request.body.size() << endl;
	// if (KRISI_TESTING)
	// 	return(true);
	return (validateRequest(ev));
}

void Server::fillRequestLineItems(std::vector<connecData*>::iterator	it)
{
	cout << PURPLE << __func__ << RESET_LINE;

	std::string	requestLine = (*it)->request.raw.substr(0, (*it)->request.raw.find('\n'));
	if ((*it)->request.raw.empty())
	{
		(*it)->request.method		= "GET";
		(*it)->request.URI			= "/";
		(*it)->request.httpVers		= HTTP;
		return ;
	}
	std::vector<string> requestLineV = split(requestLine, ' ');
	(*it)->request.method		= requestLineV[0];
	(*it)->request.URI			= requestLineV[1];
	(*it)->request.httpVers		= requestLineV[2];
	if ((*it)->request.httpVers[(*it)->request.httpVers.size() - 1] == '\r')		// return '\r' of first line :)
		(*it)->request.httpVers.erase((*it)->request.httpVers.size() - 1);

	// print out request line
	// cout << YELLOW << "method = " << (*it)->request.method << endl;
	// cout << "URI = " << (*it)->request.URI << endl;
	// cout << "httpVers = " << (*it)->request.httpVers << RESET_LINE;
}


void	Server::fillRequestHeaders(std::vector<connecData*>::iterator	it)
{
	cout << PURPLE << __func__ << RESET_LINE;
	//get headers
	int begin	= (*it)->request.raw.find('\n') + 1;
	int size	= (*it)->request.raw.find("\r\n\r\n") - begin;
	std::string	headers = (*it)->request.raw.substr(begin, size);
	std::vector<string> headersVector = split(headers, '\n');
	std::vector<string> key_value;
	size_t vectorSize = headersVector.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		key_value = split(headersVector[i], ':', 1);
		if (key_value[1][key_value[1].size() - 1] == '\n' || key_value[1][key_value[1].size() - 1] == '\r')
			key_value[1].erase(key_value[1].size() - 1);
		if (key_value.size() != 2 )
			continue ;
		if (key_value[1].size() == 0 )
			continue ;		
		if (key_value[1][0] == ' ')
			key_value[1].erase(0, 1);
		// cout << "adding " << key_value[0] << " size " << key_value.size() << " val |" << key_value[1] << "|" << endl;
		(*it)->request.headers.insert(std::make_pair(key_value[0], key_value[1]));
	}
	try
	{
		(*it)->request.hostname = (*it)->request.headers.at(HOSTNAMEHEAD);
		cout << " this is what was put inbto hostname in headers fillrequestheaders \"" << (*it)->request.hostname  << endl;
	}
	catch(const std::exception& e)
	{
		cout << " git into exception in fillrequestheaders" << endl;
		(*it)->request.hostname = "default";
	}
	
	// print headers to terminal
	// for (auto i : (*it)->request.headers)
	// {
	// 	cout << PURPLE << i.first << ": " << i.second << RESET_LINE;
	// }
}
void Server::fillRequestBody(std::vector<connecData*>::iterator	it)
{
	cout << PURPLE << __func__ << RESET_LINE;
	//get body
	size_t	begin;
	size_t	size;
	if ((*it)->request.headers.end() != (*it)->request.headers.find("content-length")) // or 
	{
		begin		= (*it)->request.raw.find("\r\n\r\n") + 4;
		cout << "XD LOL " << endl;
		size		= atoi((*it)->request.headers.at("content-length").c_str());
		cout << "XD LOL " << endl;
		std::string	body = (*it)->request.raw.substr(begin, size);			//still have to do (what if big body .. idea is to char* to data(+size of headers))
		(*it)->request.body = body;
	}
	if ((*it)->request.headers.end() != (*it)->request.headers.find("Content-Length")) // or 
	{
		begin		= (*it)->request.raw.find("\r\n\r\n") + 4;
		cout << "XD LOL " << endl;
		size		= atoi((*it)->request.headers.at("Content-Length").c_str());
		cout << "XD LOL " << endl;
		std::string	body = (*it)->request.raw.substr(begin, size);			//still have to do (what if big body .. idea is to char* to data(+size of headers))
		(*it)->request.body = body;
	}
	// else if ((*it)->request.headers.end() != (*it)->request.headers.find("content-length"))

	// print body
	// cout << PURPLE << (*it)->request.body << RESET_LINE;
}
