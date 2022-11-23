#ifndef CONFIG_HPP
#define CONFIG_HPP
#include "webserv.hpp"

#define	confMapType	std::map<std::string, std::string>

class config
{
	private:
		confMapType		configMap;
		bool			configOutcome;

		void			initDefaultConfig( void );
	public:
		config();
		config(char *confPath);
		~config();
		confMapType&	getConfigMap( void );
		bool			getOutcome( void );
		void			printMap( void );
		std::string		getServName( void );
		std::string		getHost( void );
		std::string		getPort( void );
		std::string		getClientMaxBody( void );
		std::string		getRoot( void );
		std::string		getDir( void );
		std::string		getUploadDir( void );
		std::string		getMethods( void );
};

#endif