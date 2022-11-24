#ifndef WERBSERV_HPP
#define WERBSERV_HPP

#include <sys/socket.h>		// Needed for socket creating and binding
#include <netinet/in.h>		// Needed to use struct sockaddr_in
#include <iostream>
#include <cerrno>
#include <cstring>
#include <sstream>			//for the split function
#include <vector>			// to store token when splitting
#include <map>				//to store the headers
#include <fstream>			//for opening files
#include <string>			// for getline

#include <sys/types.h>		// needed for helper function to check if a file exists
#include <sys/stat.h>

#include <iostream>
#include <fstream>
#include <vector>


#include <unistd.h>


#include "colours.h"


// Map-key define
#define SERVNAME		"serverName"
#define HOST			"host"
#define PORT			"port"
#define CLIENTMAXBODY	"clientMaxBodySize"
#define ERROR404		"errorPage404"
#define METHODS			"methods"
#define ROOT			"root"
#define DIR				"dir"
#define UPLOADDIR		"uploadDir"






using std::cout;
using std::cerr;
using std::endl;
using std::strerror;
using std::string;
using std::stringstream;
using std::ifstream;

#define PORT_NBR			3332
#define MAX_LINE			4096
#define SA					struct sockaddr
#define SOCKET_BACKLOG		1024
#define HTTPVERSION			"HTTP/1.1"


bool file_exists (const std::string& name);

std::vector<std::string> split(std::string &str, const char sep, const int &flag=0);
#endif