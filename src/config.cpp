#include "../includes/config.hpp"
#include <fstream> //for opening files
#include <signal.h>
 #include <unistd.h>

#define vec_it std::vector<std::string>::iterator
#define map_it std::map<std::string, std::string>::iterator

config::config(): configOutcome(true)
{
	initDefaultConfig();
}

config::config(std::string configBlock): configOutcome(true)
{
	std::vector<std::string>				inputLines = split(configBlock, '\n');
	std::pair<std::string, std::string>		pair_;
	std::vector<std::string>				vec_;
	std::string								tmp;
	std::vector<std::string>::iterator		lineIt = inputLines.begin();
	std::vector<std::string>::iterator		lineIt_e = inputLines.end();

	for(;lineIt != lineIt_e;lineIt++)
	{
		if((*lineIt).size() == 0)
			continue ;
		vec_ = split((*lineIt), 9);// this is a tab
		if (vec_.at(0).at(0) == '<')
		{
			vec_.clear();
			continue ;
		}
		tmp = vec_.at(0);
		if(tmp.at(tmp.size() - 1) == ':')
			tmp.erase(tmp.size() - 1);
		if (vec_.size() != 2)
		{
			cerr << RED << "This line in the config File is not according to our standards! Please fix!" << RESET_LINE << (*lineIt) << endl;
			exit(-1);
		}
		// configMap.insert(std::make_pair(tmp, vec_.at(1))); // doesnt work if key already exists
		configMap[tmp] = vec_.at(1);
		if (tmp.compare("serverName") == 0)
			cout << vec_.at(1);
		tmp.clear();
		vec_.clear();
	}
	validateConfig();
}

// config::config(char *confPath): configOutcome(true)
// {
// 	std::ifstream							confStream;
// 	std::string								workingLine;
// 	std::pair<std::string, std::string>		pair_;
// 	std::vector<std::string>				vec_;
// 	std::string								tmp;

// 	initDefaultConfig();
// 	confStream.open(confPath);
// 	while(std::getline(confStream, workingLine, '\n'))
// 	{
// 		if(workingLine.size() == 0)
// 			continue ;
// 		vec_ = split(workingLine, 9);
// 		if (vec_.at(0).at(0) == '<')
// 		{
// 			vec_.clear();
// 			continue ;
// 		}
// 		tmp = vec_.at(0);
// 		if(tmp.at(tmp.size() - 1) == ':')
// 			tmp.erase(tmp.size() - 1);
// 		// cout << "after test : " << tmp.at(tmp.size() - 1) << endl;

// 		// for (vec_it it = vec_.begin(); it != vec_.end(); it++)
// 		// {
// 		// 	cout << "\"" << *it << "\"" << endl;
// 		// }

// 		if (vec_.size() != 2)
// 		{
// 			cerr << RED << "This line in the config File is not according to our standards! Please fix!" << RESET_LINE << workingLine << endl;
// 			exit(-1);
// 		}
// 		// configMap.insert(std::make_pair(tmp, vec_.at(1))); // doesnt work if key already exists
// 		configMap[tmp] = vec_.at(1);
// 		tmp.clear();
// 		vec_.clear();
// 	}
// 	validateConfig();
// }

config::~config()
{
	// cerr << RED << "DECONST`rucCT" << RESET_LINE;
	// configMap.~map();
}

void			config::validateHost( void )
{
	std::vector<std::string>	list = split(configMap[HOST], '.');

	if (list.size() != 4)
	{
		cout << RED << "Invalid IP Format: " << configMap[HOST] << RESET_LINE;
		exit(-1);
	}
	std::vector<std::string>::iterator	it = list.begin();
	std::vector<std::string>::iterator	it_e = list.end();
	for (; it != it_e; it++)
	{
		if (it->size() > 3 || it->size() < 1)
		{
			cout << RED << "Invalid IP Format: " << configMap[HOST] << RESET_LINE;
			exit(-1);
		}
		
		for (size_t i = 0; i < it->size(); i++)
		{
			if (isdigit((*it)[i]) == 0)
			{
				cout << RED << "Invalid IP Format: " << configMap[HOST] << RESET_LINE;
				exit(-1);
			}
		}
		
		if (ft_atoi(it->c_str()) > 255)
		{
			cout << RED << "Invalid IP Format: " << configMap[HOST] << RESET_LINE;
			exit(-1);
		}
	}
}

void			config::validatePort( void )
{
	std::string		port = configMap[PORT];
	for (size_t i = 0; i < port.size(); i++)
	{
		if (isdigit((port[i]) == 0))
		{
			cout << RED << "Invalid Port Format: " << configMap[PORT] << RESET_LINE;
			exit(-1);
		}
	}
	if (ft_atoi(port.c_str()) > 65535)
	{
		cout << RED << "Invalid Port Format: " << configMap[PORT] << RESET_LINE;
		exit(-1);
	}
}

void			config::validateCMBS( void )
{
	std::string		CMBS = configMap[CLIENTMAXBODY];

	if (CMBS.size() == 0)
	{
		cout << RED << "Invalid Port Format: " << RESET_LINE;
		exit(-1);
	}
	for (size_t i = 0; i < CMBS.size(); i++)
	{
		if (isdigit((CMBS[i]) == 0))
		{
			cout << RED << "Invalid Port Format: " << configMap[CLIENTMAXBODY] << RESET_LINE;
			exit(-1);
		}
	}
	// if (ft_atoi(port.c_str()) > 65535)				// is there a max / mmin
	// {
	// 	cout << RED << "Invalid Port Format: " << configMap[PORT] << RESET_LINE;
	// 	exit(-1);
	// }			
}

void			config::validateMethods( void )
{
	try
	{
		if (configMap.at("GET").compare("yes") == 0)
		{
			methods["GET"] = true;
		}
		else if (configMap.at("GET").compare("no") == 0)
			methods["GET"] = false;
		else
		{
			cerr << RED << "Invalid Method Format" << configMap.at("GET") << endl;
			exit(-1);
		}
	}
	catch(const std::exception& e)
	{
		cerr << "Missing config for Method: GET" << endl;
		exit(-1);
	}
	
	try
	{
		if (configMap.at("POST").compare("yes") == 0)
		{
			methods["POST"] = true;
		}
		else if (configMap.at("POST").compare("no") == 0)
			methods["POST"] = false;
		else
		{
			cerr << RED << "Invalid Method Format" << configMap.at("POST") << endl;
			exit(-1);
		}
	}
	catch(const std::exception& e)
	{
		cerr << "Missing config for Method: POST" << endl;
		exit(-1);
	}
	
	try
	{
		if (configMap.at("DELETE").compare("yes") == 0)
		{
			methods["DELETE"] = true;
		}
		else if (configMap.at("GET").compare("no") == 0)
			methods["DELETE"] = false;
		else
		{
			cerr << RED << "Invalid Method Format" << configMap.at("DELETE") << endl;
			exit(-1);
		}
	}
	catch(const std::exception& e)
	{
		cerr << "Missing config for Method: DELETE" << endl;
		exit(-1);
	}
}

void			config::validatePath( std::string path, std::string target , int flags)
{
	std::string		tmp = path;
	if (path.compare(configMap[ROOT]) != 0)
		tmp = "." + path;
	if (access(tmp.c_str(), flags) != 0)
	{
		cout << RED << "Invalid Path for " << target << ": " << tmp << endl;
		perror("");
		cout << RESET;
		exit(-1);
	}
	if (target != DIR && target != UPLOADDIR)
		configMap[target] = tmp;
}

void			config::validateConfig( void )
{
	validateHost();
	validatePort();
	validateCMBS();
	validateMethods();
	validatePath(configMap[ROOT], ROOT, X_OK);
	validatePath(configMap[ERROR404], ERROR404, W_OK | R_OK);		// this doesnt work for some reason
	// cout << "this is valdiate config dir :" << configMap[DIR] << endl;
	validatePath(configMap[DIR], DIR, X_OK);
	// cout << "this is valdiate config dir :" << configMap[DIR] << endl;
	validatePath(configMap[UPLOADDIR], UPLOADDIR, X_OK);
}

confMapType&	config::getConfigMap( void )
{
	return (configMap);
} 

void	config::initDefaultConfig( void )
{
	configMap[SERVNAME] = "defaultServerName";
	configMap[HOST] = "0.0.0.0";
	configMap[PORT] = "3332";
	configMap[CLIENTMAXBODY] = "1000000";
	configMap[ERROR404] = "/database/Error_404.png";
	configMap[ROOT] = "/workspaces/webserv_42";
	configMap["CGIDIR"] = "/cgi-bin";
	configMap[DIR] = "/database";
	configMap[UPLOADDIR] = "/uploads"; 
	configMap[HTTP] = HTTPVERSION;
	configMap[DIR_LISTING] = "yes";
	methods["GET"] = true;
	methods["POST"] = true;
	methods["DELETE"] = true;
}


bool			config::getOutcome( void )
{
	return (configOutcome);
}

void			config::printMap( void )
{
	cout << "Printing Config Map" << endl;
	for (map_it it = configMap.begin(); it != configMap.end(); it++)
	{
		cout << "\"" << it->first << "->" << it->second << "\"" << endl;
	}
	cout << "Done printing Config Map" << endl;
}

std::string		config::getServName( void )
{
	return(configMap.at(SERVNAME));
}

std::string		config::getHost( void )
{
	return(configMap.at(HOST));
}

std::string		config::getPort( void )
{
	return(configMap.at(PORT));
}

int				config::getClientMaxBody( void )
{
	return(ft_atoi(configMap.at(CLIENTMAXBODY).c_str()));
}

std::string		config::getRoot( void )
{
	return(configMap.at(ROOT));
}

std::string		config::getDir( void )
{
	return(configMap.at(DIR));
}

std::string		config::getUploadDir( void )
{
	return(configMap.at(UPLOADDIR));
}

std::string		config::getMethods( void )
{
	return(configMap.at(METHODS));
}

std::string		&config::getDirectoryListing( void )
{
	return(configMap.at(DIR_LISTING));
}

bool			config::allowedMETHOD( std::string meth )
{
	if (meth.compare("GET") != 0 && meth.compare("POST") != 0 && meth.compare("DELETE") != 0)
	{
		if (DEBUG)
			cout << "return in if" << endl;
		return (false);
	}
	return (methods[meth]);
}

bool			config::allowedURI( std::string URI, std::string method )
{
	cout << ON_PURPLE << __func__ << RESET_LINE;
	// cout << "we are before get if "<< endl;

	if (method.compare("GET") == 0)
	{
		// cout << configMap[DIR] << " " << URI.substr(0, configMap[DIR].size())<< endl;

		if (DEBUG)
			cout << "comparing paths in get" << endl;
		if (configMap[DIR].compare(URI.substr(0, configMap[DIR].size())) == 0)
		{
			if (DEBUG)
				cout << "return true" << endl;
			return (true);
		}
		if (configMap[CGIDIR].compare(URI.substr(0, configMap[CGIDIR].size())) == 0)
		{
			if (DEBUG)
				cout << "return true" << endl;
			return (true);
		}
		for (vec_it it = postedFilePaths.begin(); it != postedFilePaths.end(); it++)
		{
			if((*it).compare(URI) == 0)
				return(true);
		}
		if (URI.compare("/favicon.ico") == 0)
			return (true);
		if (DEBUG)
			cout << "return false 1" << endl;
		return (false);
	}
	else if (method.compare("POST") == 0)
	{
		cout << URI << " " << configMap[UPLOADDIR] << endl;
		if(configMap[UPLOADDIR].compare(URI.substr(0, configMap[UPLOADDIR].size()))== 0)
		{
			postedFilePaths.push_back(URI);
			return (true);
		}
		if (configMap[CGIDIR].compare(URI.substr(0, configMap[CGIDIR].size()))== 0)
			return (true);
	}
	else if (method.compare("DELETE") == 0)
	{
		cout << URI << " " << configMap[UPLOADDIR] << endl;
		if(configMap[UPLOADDIR].compare(URI.substr(0, configMap[UPLOADDIR].size()))== 0)
		{
			for (vec_it it = postedFilePaths.begin(); it != postedFilePaths.end(); it++)
			{
				if((*it).compare(URI) == 0)
				{
					cout << "erasing path from vector" << endl;	
					postedFilePaths.erase(it);
					break ;
				}
			}	
			return (true);
		}
	}
	if (DEBUG)
		cout << "return false 2" << endl;
	return (false);
}

bool			config::validateDirectoryListing( void )
{
	if ((configMap.at(DIR_LISTING).compare("yes") == 0)
		|| (configMap.at(DIR_LISTING).compare("no") == 0))
		return (true) ;

	return (false);
}
