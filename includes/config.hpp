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
};

#endif