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
	// std::string		confPathStr = confPath;

	std::ifstream							confStream;
	std::string								workingLine;
	std::pair<std::string, std::string>		pair_;
	std::vector<std::string>				vec_;


	confStream.open(confPath);
	while(std::getline(confStream, workingLine, '\n'))
	{
		cout << workingLine << "size == " << workingLine.size() << endl;

		if(workingLine.size() == 0)
			continue ;

		vec_ = split(workingLine, 9);
		
		// for (vec_it it = vec_.begin(); it != vec_.end(); it++)
		// {
		// 	cout << "\"" << *it << "\"" << endl;
		// }
		

		if (vec_[0].at(0) == '<')
		{
			vec_.clear();
			continue ;
		}
		if (vec_.size() != 2)
		{
			cerr << RED << "This line in the config File is not according to our standards! Please fix!" << RESET_LINE << workingLine << endl;
			configOutcome = false;
			return ;
		}
		configMap.insert(std::make_pair(vec_.at(0), vec_.at(1)));
		vec_.clear();
	}
}

config::~config()
{
	cerr << RED << "DECONST`rucCT" << RESET_LINE;
	// configMap.~map();
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
	// configMap[""] = "";
	// configMap[""] = "";
	// configMap[""] = "";
	// configMap[""] = "";
	// configMap[""] = "";
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
		cout << "\"" << it->first << ": " << it->second << "\"" << endl;
	}
	cout << "Done printing Config Map" << endl;
}
