#include "../includes/server.hpp"

void	server::fillRequestStruct(std::vector<connecData*>::iterator	it)
{
	fillRequestLineItems(it);
	fillRequestHeaders(it);
	fillRequestBody(it);
}

void server::fillRequestLineItems(std::vector<connecData*>::iterator	it)
{

	//get request line
	std::string	requestLine = (*it)->request.raw.substr(0, (*it)->request.raw.find('\n'));
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


void	server::fillRequestHeaders(std::vector<connecData*>::iterator	it)
{
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
		// cout << "adding " << key_value[0] << " size " << key_value.size() << " val |" << key_value[1] << "|" << endl;
		(*it)->request.headers.insert(std::make_pair(key_value[0], key_value[1]));
	}
	
	// print headers to terminal
	for (auto i : (*it)->request.headers)
	{
		cout << PURPLE << i.first << ": " << i.second << RESET_LINE;
	}
}
void server::fillRequestBody(std::vector<connecData*>::iterator	it)
{
	//get body
	size_t	begin;
	size_t	size;
	if ((*it)->request.headers.end() != (*it)->request.headers.find("content-length")) // or 
	{
		begin		= (*it)->request.raw.find("\r\n\r\n") + 4;
		cout << "XD LOL " << endl;
		size		= stoi((*it)->request.headers.at("content-length"));
		cout << "XD LOL " << endl;
		std::string	body = (*it)->request.raw.substr(begin, size);			//still have to do (what if big body .. idea is to char* to data(+size of headers))
		(*it)->request.body = body;
	}
	// else if ((*it)->request.headers.end() != (*it)->request.headers.find("content-length"))

	// print body
	// cout << PURPLE << (*it)->request.body << RESET_LINE;
}
