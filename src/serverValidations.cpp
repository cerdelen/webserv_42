#include "../includes/server.hpp"

void	Server::URIisDirectory( s_request &req )
{
	std::vector<std::string>			dirs;

	dirs.push_back("/database/intraPictures");
	dirs.push_back("/database/forms");
	dirs.push_back("/uploads");
	dirs.push_back("/database");
	std::vector<std::string>::iterator	DirIt = dirs.begin();
	std::vector<std::string>::iterator	DirIt_e = dirs.end();
	for (; DirIt != DirIt_e; DirIt++)
	{
		if(req.URI.compare(*DirIt) == 0)
		{
			req.method = "GET";
			req.URI = DIRERRORPATH;
		}
	}
}

bool	Server::validateRequest( struct epoll_event ev )
{
	cout << GREEN << __func__ << RESET_LINE;
	std::vector<connecData*>::iterator	it = findStructVectorIt(ev);

	// print headers to terminal

	std::map<string, string>::iterator	itt = (*it)->request.headers.begin();
	std::map<string, string>::iterator	it_e = (*it)->request.headers.end();
	cout << (*it)->request.URI << " " <<  (*it)->request.method << endl;
	for (; itt != it_e; itt++)
	{
		cout << PURPLE << itt->first << ": " << itt->second << RESET_LINE;
	}
	if (getConfig((*it)->request.hostname).allowedMETHOD((*it)->request.method) == false)
	{
		cerr << RED << "Request rejected because of Invalid Method: " << (*it)->request.method << RESET_LINE;
		// setErrorStatusCodeAndRespond(it, "405");
		setErrorStatusCodeAndRespond(ev, it, "501");	// dont know which of these 2 to use tbh
		// stopInvaldiRequest(ev); // stop request because illegal
		return (false);
	}
	// cout << "this is before expansion" << (*it)->request.URI << endl;
	if((*it)->request.URI == "/")
	{
		if (getConfig((*it)->request.hostname).getDirectoryListing().compare("yes") == 0)
			(*it)->request.URI = DIR_LISTING_SCRIPT;
		else
			(*it)->request.URI = DEFAULTPAGE;
		cout << "got into if" << endl;
	}
	// cout << "this is after expansion" << (*it)->request.URI << endl;
	if (getConfig((*it)->request.hostname).allowedURI((*it)->request.URI, (*it)->request.method) == false)
	{
		cerr << RED << "Request rejected because of Invalid URI: " << (*it)->request.URI << RESET_LINE;
		setErrorStatusCodeAndRespond(ev, it, "403");
		// stopInvaldiRequest(ev);
		return (false);
	}
	if ((*it)->request.httpVers.compare((std::string)HTTPVERSION) != 0)
	{
		cout << RED << "Request rejected because of Invalid HTTP Version: " << (*it)->request.httpVers << RESET_LINE;
		setErrorStatusCodeAndRespond(ev, it, "505");
		// stopInvaldiRequest(ev);
		return (false);
	}
	try
	{
		if (ft_atoi((*it)->request.headers.at("Content-Length").c_str()) > getConfig((*it)->request.hostname).getClientMaxBody())
		{
			cout << RED << "Request rejected because of CONTENT-LENGTH IS TOO BIG: " << ft_atoi((*it)->request.headers.at("Content-Length").c_str()) << RESET_LINE;
			setErrorStatusCodeAndRespond(ev, it, "413");
			// stopInvaldiRequest(ev);
			return (false);

		}
	}
	catch(const std::exception& e)
	{
	}
	try
	{
		cout << "hieeeeloo" << (*it)->request.headers.at("content-length") << endl;
		if (ft_atoi((*it)->request.headers.at("content-length").c_str()) > getConfig((*it)->request.hostname).getClientMaxBody())
		{
			cout << RED << "Request rejected because of CONTENT-LENGTH IS TOO BIG: " << ft_atoi((*it)->request.headers.at("content-length").c_str()) << RESET_LINE;
			setErrorStatusCodeAndRespond(ev, it, "413");
			// stopInvaldiRequest(ev);
			return (false);

		}
	}
	catch(const std::exception& e)
	{
	}
	return (true);
}