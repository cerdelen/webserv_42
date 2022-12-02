#include "../includes/config.hpp"
#include <fstream> //for opening files
#include <signal.h>

#define vec_it std::vector<std::string>::iterator
#define map_it std::map<std::string, std::string>::iterator

config::config(): configOutcome(true)
{
	initDefaultConfig();
}

config::config(char *confPath): configOutcome(true)
{
	std::ifstream							confStream;
	std::string								workingLine;
	std::pair<std::string, std::string>		pair_;
	std::vector<std::string>				vec_;
	std::string								tmp;

	initDefaultConfig();
	confStream.open(confPath);
	while(std::getline(confStream, workingLine, '\n'))
	{
		if(workingLine.size() == 0)
			continue ;
		vec_ = split(workingLine, 9);
		if (vec_.at(0).at(0) == '<')
		{
			vec_.clear();
			continue ;
		}
		tmp = vec_.at(0);
		if(tmp.at(tmp.size() - 1) == ':')
			tmp.erase(tmp.size() - 1);
		// cout << "after test : " << tmp.at(tmp.size() - 1) << endl;

		// for (vec_it it = vec_.begin(); it != vec_.end(); it++)
		// {
		// 	cout << "\"" << *it << "\"" << endl;
		// }

		if (vec_.size() != 2)
		{
			cerr << RED << "This line in the config File is not according to our standards! Please fix!" << RESET_LINE << workingLine << endl;
			configOutcome = false;
			return ;
		}
		// configMap.insert(std::make_pair(tmp, vec_.at(1))); // doesnt work if key already exists
		configMap[tmp] = vec_.at(1);
		tmp.clear();
		vec_.clear();
	}
	validateConfig();
}

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
	std::string		port = configMap[PORT];
	for (size_t i = 0; i < port.size(); i++)
	{
		if (isdigit((port[i]) == 0))
		{
			cout << RED << "Invalid Port Format: " << configMap[PORT] << RESET_LINE;
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
	std::vector<std::string>	list = split(configMap[METHODS], '/');

	if (list.size() > 3 || list.size() < 1)
	{
		cout << RED << "Not enough Methods: " << configMap[METHODS] << endl << "Allowed Methods are \"GET\", \"POST\" and \"DELETE\" "<< RESET_LINE;
		exit(-1);
	}
	std::vector<std::string>::iterator	it = list.begin();
	std::vector<std::string>::iterator	it_e = list.end();
	for (; it != it_e; it++)
	{
		if (it->compare("GET") != 0 && it->compare("POST") != 0 && it->compare("DELETE") != 0)
		{
			cout << RED << "Invalid Methods: " << configMap[METHODS] << endl << "Allowed Methods are only \"GET\", \"POST\" and \"DELETE\" "<< RESET_LINE;
			exit(-1);
		}
		if (it->compare("GET") == 0 )
			methods["GET"] = true;
		else if (it->compare("POST") == 0 )
			methods["POST"] = true;
		else if (it->compare("DELETE") == 0 )
			methods["DELETE"] = true;
	}
}

void			config::validatePath( std::string path, std::string target , int flags)
{
	std::string		tmp = path;
	if (path.compare(configMap[ROOT]) != 0)
		tmp = configMap[ROOT] + path;
	if (access(tmp.c_str(), flags) != 0)
	{
		cout << RED << "Invalid Path for " << target << ": " << tmp << endl;
		perror("");
		cout << RESET;
		exit(-1);
	}
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
	validatePath(configMap[DIR], DIR, X_OK);
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
	configMap[PORT] = "1999";
	configMap[CLIENTMAXBODY] = "1000000";
	configMap[ERROR404] = "/database/Error_404.png";
	configMap[METHODS] = "GET";
	configMap[ROOT] = "/workspaces/webserv_42";
	configMap[DIR] = "/";
	configMap[UPLOADDIR] = "/database/uploads"; 
	configMap[HTTP] = HTTPVERSION;
	methods["GET"] = false;
	methods["POST"] = false;
	methods["DELETE"] = false;
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

std::string		config::getClientMaxBody( void )
{
	return(configMap.at(CLIENTMAXBODY));
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

bool			config::allowedMETHOD( std::string meth )
{
	if (meth.compare("GET") != 0 && meth.compare("POST") != 0 && meth.compare("DELETE") != 0)
	{
		cout << "return in if" << endl;
		return (false);
	}
	return (methods[meth]);
}

bool			config::allowedURI( std::string URI, std::string method )
{
	if (method.compare("GET") == 0)
		return (true);
	if (configMap[UPLOADDIR].compare(URI.substr(0, configMap[UPLOADDIR].size()))== 0)
		return (true);
	return (false);
}
