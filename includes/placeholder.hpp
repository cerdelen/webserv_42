#ifndef PLACEHOLDER_HPP
#define PLACEHOLDER_HPP
#include "webserv.hpp"
// #include <iostream>

typedef struct t_request
{
	std::string								method;
	std::string								URI;
	std::string								httpVers;
	std::map<std::string, std::string>		headers;
	std::string								body; //for now string
}	s_request;

class placeholder
{
	private:
		int					serverSocket;
		int					connectionSocket;
		struct sockaddr_in	serverAddress;
		int					sizeOfServerAddress = sizeof(serverAddress);
		std::string			fullRequest;
		s_request			currRequest;


		void	failTest( int check, std::string message )
		{
			// if (serverSocket < 0)
			// {
			// 	cerr << RED << "Server Socket < 0! Abort!" << endl;
			// 	cerr << "Errno message: " << strerror(errno) << RESET_LINE;
			// 	exit(-1);
			// }
			if (check < 0)
			{
				cerr << RED << message << " < 0! Abort!" << RESET_LINE;
				cerr << RED << "Errno message : " << strerror(errno) << RESET_LINE;
				exit(-1);
			}
		}

		void	servAddressInit( void )
		{
			serverSocket = socket(AF_INET, SOCK_STREAM, 0);			// SOCK_STREAM == TCP

			int	option = 1;
			
			setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option));

			serverAddress.sin_family		= AF_INET;							// means IPv4 / AD_INET6 =IPv6 
			serverAddress.sin_port			= htons(PORT_NBR);					// Used port
			serverAddress.sin_addr.s_addr	= htonl(INADDR_LOOPBACK);			// Address this socket is litening to


			failTest(serverSocket, "Server Socket");

			failTest(bind(serverSocket, (SA *) &serverAddress, sizeof(serverAddress)),
						"Binding Server Socket");

			failTest(listen(serverSocket, SOCKET_BACKLOG),						// SOMAXCONN???
						"listen() of Server Socket");
		}; // probably here address

		void	fillRequestStruct(std::string &fullRequest)
		{
			//get request line
			std::string	requestLine = fullRequest.substr(0, fullRequest.find('\n'));
			std::vector<string> requestLineV = split(requestLine, ' ');
			currRequest.method		= requestLineV[0];
			currRequest.URI			= requestLineV[1];
			currRequest.httpVers	= requestLineV[2];

			//print out request line
			// cout << "method = " << currRequest.method << endl;
			// cout << "URI = " << currRequest.URI << endl;
			// cout << "httpVers = " << currRequest.httpVers << endl;

			//get headers
			int begin	= fullRequest.find('\n') + 1;
			int size	= fullRequest.find("\r\n\r\n") - begin;
			std::string	headers = fullRequest.substr(begin, size);
			cout << YELLOW << headers << RESET_LINE;
			cout << YELLOW << headers[127] << RESET_LINE;
			std::vector<string> headersVector = split(headers, '\n');
			std::vector<string> key_value;
			size_t vectorSize = headersVector.size();
			for (size_t i = 0; i < vectorSize - 1; i++)
			{
				key_value = split(headersVector[i], ':', 1);
				currRequest.headers.insert(std::make_pair(key_value[0], key_value[1]));
			}
			
			//print headers to terminal
			// for (auto i : currRequest.headers)
			// {
			// 	cout << RED << i.first << ": " << i.second << RESET_LINE;
			// }

			//get body
			

			if (currRequest.headers.end() != currRequest.headers.find("Content-Length")) // or 
			{
				begin		= fullRequest.find("\r\n\r\n") + 4;
				size		= stoi(currRequest.headers.at("Content-Length"));
				cout << begin << endl;
				cout << size << endl;
				std::string	body = fullRequest.substr(begin, size);
				cout << RED << body << RESET_LINE;
				// cout << RED << "BODY" << fullRequest[149] << RESET_LINE;
			}
			

			

		}

		void		handleRequest(std::string &fullRequest)
		{
			fillRequestStruct(fullRequest);
			if (currRequest.method.compare("GET") == 0)
			{
				// fillHeadersMap(fullRequest);
				cout << "its a get request :)" << endl;
			}
			else if (currRequest.method.compare("POST") == 0)
			{
				cout << "its a post request :)" << endl;
			}
			else if (currRequest.method.compare("DELETE") == 0)
			{
				cout << "its a delete request :)" << endl;
			}
			else
			{
				cout << "We should throw an error code with a message that we do not support this method" << endl;
			}




		}




	public:
		placeholder()
		{
			servAddressInit();
		};
		~placeholder() {
		};
		void	handleRequest( void )
		{
		};

		void		request( void )
		{
			//Request-Line				= Method SP Request-URI SP HTTP-Version CRLF
			//CR = Carriage return		= \r
			//LF = Line Feed			= \n
			int		requestSocket;
			int		recvReturn;
			char	receivingBuffer[MAX_LINE + 1];
			char	sendingBuffer[MAX_LINE + 1] = "HTTP/1.0 200 OK\r\n\r\nThis is a welcoming message version 2!"; // example answer for request

			requestSocket = accept(serverSocket, (SA *) NULL, NULL);
			failTest(requestSocket, "accept() Socket");
			memset(receivingBuffer, 0, MAX_LINE + 1);	
			fullRequest.clear();
			while(((recvReturn = recv(requestSocket, receivingBuffer, MAX_LINE, 0)) > 0))
			{
				failTest(recvReturn, "Reading into receivingBuffer out of requestSocket");
				fullRequest.append(receivingBuffer);
				if (receivingBuffer[recvReturn - 1] == '\n' && receivingBuffer[recvReturn - 2] == '\r')
					break;
				memset(receivingBuffer, 0, MAX_LINE);
			}

			handleRequest(fullRequest);

			// funciton to determine what kind of request

			// here we call the corresponding request funciton

			// proccess the request here to get an answer

			failTest(send(requestSocket, sendingBuffer, strlen(sendingBuffer), 0),
						"Sending answer to Request to requestSocket");

			failTest(close(requestSocket),
						"Sending answer to Request to requestSocket");
			cout << "This is the full Request" << RESET_LINE;
			cout << endl << fullRequest << RED << "<<here is the end>>" << RESET_LINE;
		}
};

#endif