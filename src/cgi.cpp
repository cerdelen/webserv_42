#include "../includes/server.hpp"

// void handleCGI is in serverHandlers.cpp

bool Server::checkCGIPaths(std::vector<connecData*>::iterator it)
{	
	cout << YELLOW << __func__ << RESET_LINE;
	std::string path = (*it)->request.URI;
	// cout << GREEN << "Path = URI = " << path << "  " << (*it)->request.URI << RESET_LINE;
	// cout << YELLOW << "Method: " << (*it)->request.method << RESET_LINE;
	if ((*it)->request.method.compare("GET") == 0)
	{
		std::vector<string> pathAndQuary = split((*it)->request.URI, '?');
		// cout << "IT IS GET: " << pathAndQuary.size() << endl << "Path is; " << pathAndQuary[0] << endl << "Quary is: " << pathAndQuary[1] << endl;
		if (pathAndQuary.size() != 2)
		{
			if (path.compare(DIR_LISTING_SCRIPT) == 0)
				return (true);
			// cout << RED << "CGI GET NO QUERY_STRING" << RESET_LINE;
			// return (false);
		}
		path = pathAndQuary[0];
		// cout << GREEN << "Path in GET " << path << "  " << (*it)->request.URI << RESET_LINE;
	}

	// cout << GREEN << "Path after GET " << path << "  " << (*it)->request.URI << RESET_LINE;
		// std::string path = split(pathSplit[1], '?')[1];
	std::ifstream file;
	std::string		filepath;
	filepath = PATH_TO_SCRIPTS + path;
	file.open(filepath.c_str());
	if (file.good())
	{
		cout << YELLOW << "FILE IS GOOD" << endl;
		std::vector<string> pathSplit = split(path, '.');
		if (pathSplit.size() != 2)
		{
			std::cerr << "Unsupported script type\n";
			(*it)->response.status_code = "500";
			return (false);
		}
		string extension = pathSplit[1];
		try
		{
			possibleCGIPaths.at(extension);
		}
		catch(const std::exception& e)
		{
			std::cerr << "The server does not recognise the script type\n";
			(*it)->response.status_code = "500";
			return (false);
		}
		
		file.close();
		cout << "FOUND A MATCH" << endl;
		return (true);
	}
	// for(int i = 0; i < scriptsCGI.size(); i++)
	// {
	// 	if (path.compare(scriptsCGI[i]) == 0)
	// 	{
	// 		cout << "Found a match: " << path << "==" << scriptsCGI[i] << endl;
	// 		return (true);
	// 	}
	// }
	file.close();
	cout << "DIDN'T FIND A MATCH" << endl;
	return (false);
}

std::string getBodyPostRequestCGI(std::vector<connecData*>::iterator it)
{
	cout << GREEN << __func__ << RESET_LINE;

	unsigned pos = (*it)->request.raw.find("\r\n\r\n");
	if(pos == (*it)->request.raw.npos || pos + 1 >= (*it)->request.raw.length())
	{
		return "empty_body=oh_no";
	}
	return (*it)->request.raw.substr(pos + 1, (*it)->request.raw.length() - (pos + 1));

}

void CGI::setEnvironment(std::vector<connecData*>::iterator it, config servConfig)
{
	cout << GREEN << __func__ << RESET_LINE;
	std::vector<string> URI_queryString = split((*it)->request.URI, '?');
	if ((*it)->request.method.compare("GET") == 0)
	{
		if (URI_queryString.size() > 1)
			env["QUERY_STRING"] = split((*it)->request.URI, '?')[1]; //gets everything after the question mark
		env["REQUEST_METHOD"] = "GET";
	}
	if ((*it)->request.method.compare("POST") == 0)
	{
		env["QUERY_STRING"] = getBodyPostRequestCGI(it);
		env["REQUEST_METHOD"] = "POST";
		
	}
	env["SCRIPT_NAME"] = split((*it)->request.URI, '?')[0]; //The virtual path (e.g., /cgi-bin/program.pl) of the script being executed.
	env["SERVER_NAME"] = getConfig((*it)->request.hostname).getServName();
	env["SERVER_PORT"] = getConfig((*it)->request.hostname).getPort();
	env["SERVER_PROTOCOL"] = HTTPVERSION;
}

char **CGI::envToDoubleCharacterArray()
{
	int envSize = env.size();
	char **environment = (char **)calloc(envSize + 1, sizeof(char *));

	if (environment == NULL)
	{
		exit(78);
	}
	;
	std::map<string, string>::iterator mapIter= env.begin();

	for(int i = 0; i < envSize; i++)
	{
		environment[i] = strdup((mapIter->first + "=" + mapIter->second).c_str());
		// cout << GREEN << environment[i] << RESET_LINE;
		mapIter++;
	}
	// cout << YELLOW << __func__ << "DO we ge here?" << RESET_LINE;

	return (environment);
	// environment[envSize + 1] = NULL;//not needed when using calloc?
}


