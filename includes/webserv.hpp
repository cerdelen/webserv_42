#ifndef WERBSERV_HPP
#define WERBSERV_HPP

#include <stdlib.h>
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
#include <stdlib.h>
#include <sys/types.h>		// needed for helper function to check if a file exists
#include <sys/stat.h>

#include <iostream>
#include <fstream>
#include <vector>

#include <sys/epoll.h> //epoll
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
#define CGIDIR			"CGIDIR"
#define UPLOADDIR		"uploadDir"
#define DIR_LISTING		"directoryListing"
#define HOSTNAMEHEAD	"Host"

#define DIRERRORPATH    "/database/default_index_.html"
#define DEFAULTPAGE     "/database/default_index.html"

// PATH defines
#define DEFAULT_PATH	"./database/default_index.html"
#define FAV_ICON_PATH	"./database/favicon.ico"
#define CGI_FOLDER_PATH	"/cgi-bin"
#define DEFAULT_CGI_FILE_PATH "/workspaces/webserv_42/database/intraPictures/"
#define DIR_LISTING_SCRIPT "/cgi-bin/directory_listing.sh"
// ERROR CODE PATH defines
#define ERROR_404_PATH	"./database/Error_404.png"
// #define PATH_TO_SCRIPTS	"/webserv_42"
#define PATH_TO_SCRIPTS	"/workspaces/webserv_42"

using std::cout;
using std::cerr;
using std::endl;
using std::strerror;
using std::string;
using std::stringstream;
using std::ifstream;
using std::make_pair;

#define PORT_NBR			3332
#define MAX_LINE			4096
#define SA					struct sockaddr
#define SOCKET_BACKLOG		1024
#define HTTPVERSION			"HTTP/1.1"
#define HTTP				"HTTPVERS"
#define MAX_EVENTS			10
#define KRISI_TESTING		1
#define DEBUG				0

///misc.cpp
bool						file_exists (const std::string& name);
std::vector<std::string>	split(std::string &str, const char sep, const int &flag=0);
int							ft_atoi(const char *nptr);
struct epoll_event			createEpollStruct(int fdSocket, uint32_t flags);
char						*ft_itoa(int n);



#endif //WERBSERV_HPP