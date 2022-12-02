#ifndef CONFIG_HPP
#define CONFIG_HPP
#include "webserv.hpp"

#define	confMapType	std::map<std::string, std::string>
#define	methodmap	std::map<std::string, bool>


class config
{
	private:
		confMapType		configMap;
		bool			configOutcome;
		methodmap		methods;

		void			initDefaultConfig( void );
		void			validateConfig( void );
		void			validatePort( void );
		void			validateHost( void );
		void			validateCMBS( void );
		void			validateMethods( void );
		void			validatePath( std::string path, std::string target , int flags );
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
		bool			allowedMETHOD( std::string );
		bool			allowedURI( std::string, std::string );
		// bool			allowedGET(  );
		// bool			allowedPOST( void );
		// bool			allowedDELETE( void );
};

#endif