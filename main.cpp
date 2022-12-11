#include "./includes/webserv.hpp"

#include "./includes/server.hpp"
#include "./includes/config.hpp"
#include "./includes/CGI.hpp"

int main( int argc, char **argv )
{		
	// if(argc == 1)
	// {
	// 	Server	servy;

	// 	if(servy.getConfigOutcome() == false)
	// 		return(-1);
	// 	// servy.getConfig().printMap();
	// 	servy.requestLoop();

	// }
	// else if (argc == 2)
	// {
		Server			servy(argv[1]);

		config lel	=	servy.getConfig("hostname");
		cout << RED << lel.getDir() << RESET_LINE;

		// if(servy.getConfigOutcome() == false)
		// 	return(-1);
		// servy.getConfig().printMap();
		// while(true)
		// {
		// 	servy.requestLoop();
		// }
	// }
	// else if (argc > 2)
	// {
	// 	cerr << RED << "Too many arguments!" << RESET_LINE;
	// 	return (-1);
	// }
}

