#ifndef server_HPP
#define server_HPP
#include "webserv.hpp"
#include <fstream>
#include "config.hpp"


typedef struct t_request
{
	std::string								method;
	std::string								URI;
	std::string								httpVers;
	std::map<std::string, std::string>		headers;
	std::string								body; //for now string
}	s_request;

typedef struct t_response
{
	std::string								httpVers;
	std::string								statusMessage;			// number + message
	std::string								headers;
	std::string								body;					// getBinary()
}	s_response;

class server
{
	private:
		// const std::map<string, string> statusCodesAndMessages
		// {
		// 	{"100", "Continue"},
		// 	{"101", "Switching protocols"},
		// 	{"102", "Processing"},
		// 	{"103", "Early Hints"},
		// 	{"200", "OK"},
		// 	{"201", "Created"},
		// 	{"202", "Accepted"},
		// 	{"203", "Non-Authoritative Information"},
		// 	{"204", "No Content"},
		// 	{"205", "Reset Content"},
		// 	{"206", "Partial Content"},
		// 	{"207", "Multi-Status"},
		// 	{"208", "Already Reported"},
		// 	{"226", "IM Used"},
		// 	{"300", "Multiple Choices"},
		// 	{"301", "Moved Permanently"},
		// 	{"302", "Found"},
		// 	{"303", "See Other"},
		// 	{"304", "Not Modified"},
		// 	{"305", "Use Proxy"},
		// 	{"306", "Switch Proxy"},
		// 	{"307", "Temporary Redirect"},
		// 	{"308", "Permanent Redirect"},
		// 	{"400", "Bad Request"},
		// 	{"401", "Unauthorized"},
		// 	{"402", "Payment Required"},
		// 	{"403", "Forbidden"},
		// 	{"404", "Not Found"},
		// 	{"405", "Method Not Allowed"},
		// 	{"406", "Not Acceptable"},
		// 	{"407", "Proxy Authentication Required"},
		// 	{"408", "Request Timeout"},
		// 	{"409", "Conflict"},
		// 	{"410", "Gone"},
		// 	{"411", "Length Required"},
		// 	{"412", "Precondition Failed"},
		// 	{"413", "Payload Too Large"},
		// 	{"414", "URI Too Long"},
		// 	{"415", "Unsupported Media Type"},
		// 	{"416", "Range Not Satisfiable"},
		// 	{"417", "Expectation Failed"},
		// 	{"418", "I'm a Teapot"},
		// 	{"421", "Misdirected Request"},
		// 	{"422", "Unprocessable Entity"},
		// 	{"423", "Locked"},
		// 	{"424", "Failed Dependency"},
		// 	{"425", "Too Early"},
		// 	{"426", "Upgrade Required"},
		// 	{"428", "Precondition Required"},
		// 	{"429", "Too Many Requests"},
		// 	{"431", "Request Header Fields Too Large"},
		// 	{"451", "Unavailable For Legal Reasons"},
		// 	{"500", "Internal Server Error"},
		// 	{"501", "Not Implemented"},
		// 	{"502", "Bad Gateway"},
		// 	{"503", "Service Unavailable"},
		// 	{"504", "Gateway Timeout"},
		// 	{"505", "HTTP Version Not Supported"},
		// 	{"506", "Variant Also Negotiates"},
		// 	{"507", "Insufficient Storage"},
		// 	{"508", "Loop Detected"},
		// 	{"510", "Not Extended"},
		// 	{"511", "Network Authentication Required"},
		// };
		int					serverSocket;
		int					connectionSocket;
		struct sockaddr_in	serverAddress;
		int					sizeOfServerAddress = sizeof(serverAddress);
		std::string			fullRequest;
		s_request			currRequest;
		s_response			currResponse;
		std::map<std::string, std::string> possible_types;
		int					port;
		config				servConfig;
		// int					host;  // what is this???
		size_t				max_client_body_size;




		void			failTest( int check, std::string message );
		void			servAddressInit( void );
		void			fillRequestLineItems(string &fullRequest);
		void			fillRequestHeaders(string &fullRequest);
		void			fillRequestBody(std::string &fullRequest);
		void			fillRequestStruct(std::string &fullRequest);
		int				checkGetRequest(int requestSocket);
		int				checkRequestErrors(int requestSocket);
		void			handleRequest(int requestSocket, std::string &fullRequest);
		std::string		getBinary(std::string &path, long *size, int request_soc);
		std::string		makeHeader(long bodySize, std::string &path);									//prolly other stuff too
		void			fillResponseStructBinary(std::string &path, int request_soc);
		void			sendResponse(int requestSocket, std::string &path);			// im writing this with a get request in mind
		void			fillInPossibleTypes();
		void			handle_post(int requestSocket, std::string &path, std::string &fullRequest);
		
	public:
		server(): servConfig()
		{
			if(servConfig.getOutcome() == false)
			{
				// servConfig.~config();
				return ;
			}
			fillInPossibleTypes();
			servAddressInit();
		};
		server(char * confPath): servConfig(confPath)
		{
			if(servConfig.getOutcome() == false)
			{
				// servConfig.~config();
				cout << "returning " << endl;
				return ;
			}
			fillInPossibleTypes();
			servAddressInit();
		};
		~server() {
		};
		void	handleRequest( void )
		{
		};


		config		&getConfig( void );
		bool		getConfigOutcome( void );
		void		request( void );
};

#endif