#include "../includes/server.hpp"

void	Server::handleCGI(std::vector<connecData*>::iterator it)
{
	cout << RED << __func__ << ": isCGI!" << RESET_LINE;

	//check if path to script is legit maybe?
	(*it)->isCGI = true;
	objectCGI.setEnvironment(it, servConfig); 
	(*it)->fileNameCGI = DEFAULT_CGI_FILE_PATH + std::to_string((*it)->socket) + "_fileCGI.html";
	
	FILE * fileCGI= fopen((*it)->fileNameCGI.c_str(), "w");				//create file to write to
	if (!file_exists((*it)->fileNameCGI))
	{
		cout << RED << __func__ << (*it)->fileNameCGI.c_str() \
				<< ": could not create file" << endl;
		exit(77); //THIS SHOULD NOT BE EXIT
	}

	int fdFileCGI = fileno(fileCGI);									//get file descriptor for dup
	(*it)->request.URI = (*it)->fileNameCGI;							//give new file URI to the request struct
	pid_t pid = fork();

	if (pid == 0) //CHILD
	{
		dup2(fdFileCGI, 1);												// change output stream to the CGI file;
		char **env = objectCGI.envToDoubleCharacterArray();//!!!!!!!!!!!//THIS NEEDS TO BE FREED SOMEWHERE
		std::string executablePath = PATH_TO_SCRIPTS + objectCGI.env.at("SCRIPT_NAME");
		execve((executablePath).c_str(), NULL, env);
		perror("\nExecve: ");
		free(env);
		exit(33);
	}
	wait(NULL);
	handleGet(it);
}

void 	Server::handlePost( std::vector<connecData*>::iterator it, struct epoll_event ev)
{
	cout << RED << __func__ << RESET_LINE;
	std::string extension = getExtensionFromRequestPost(it);
	if(extension.empty())
		endResponse(ev);

	(*it)->response.content_type = extension;
	createAndSendResponseHeaders(it);
	// if((*it)->request.URI.
}

void	Server::handleDelete(std::vector<connecData*>::iterator it, struct epoll_event	ev)
{
	cout << RED << __func__ << RESET_LINE;
	
	FILE	*file_stream;
	std::cout << (*it)->request.URI << std::endl;

	if((*it)->request.URI.compare(0, 8,"/uploads") == 0)
	{
		//Root path for welcome page
		std::string ret = ".";
		ret.append((*it)->request.URI);
		if(remove(ret.c_str()) != 0)
		{
			(*it)->response.status_code = "417";
			std::cout << "Cannot remove this file " << "." + (*it)->request.URI << std::endl;
		}
		return ;
	}else{
		std::cout << "Cannot delete from diffrent directiory than uploads" << std::endl;
		(*it)->response.status_code = "417";
		return ;
	}	if(file_stream == nullptr)
	{
		//For errors
		(*it)->response.status_code = "404";
		std::cout << "File not found " << std::endl;
	}
	// (*it)->response.content_type = extension;
	// (*it)->response.content_lenght_str = conv.str();
	createAndSendResponseHeaders(it);
}

void	Server::handleGet(std::vector<connecData*>::iterator it)
{
	cout << RED << __func__ << RESET_LINE;
	
	FILE	*file_stream;
	FILE	*file_str_2;

	cout << YELLOW << __func__ << "URI: "  << (*it)->request.URI << RESET_LINE;

	if ((*it)->isCGI)
	{
		cout << YELLOW << __func__ << " IS CGI" << RESET_LINE;
		file_stream = fopen((*it)->fileNameCGI.c_str(), "rb");
		file_str_2 = fopen((*it)->fileNameCGI.c_str(), "rb");
	}
	else if ((*it)->request.URI.compare("/") == 0)
	{
		//Root path for welcome page
		file_stream = fopen(DEFAULT_PATH , "rb");
		file_str_2 = fopen(DEFAULT_PATH , "rb");
	}
	else if ((*it)->request.URI.compare("/favicon.ico") == 0)
	// else if ((*it)->request.URI.compare("./favicon.ico") == 0)
	{
		//Favicon for now streamlined
		file_stream = fopen(FAV_ICON_PATH, "rb");
		file_str_2 = fopen(FAV_ICON_PATH, "rb");

	}
	else if ((*it)->request.URI.compare(0, 46, DEFAULT_CGI_FILE_PATH) == 0)
	{
		file_stream = fopen((*it)->request.URI.c_str(), "rb");
		file_str_2 = fopen((*it)->request.URI.c_str(), "rb");
	}
	else
	{
		//If there is a different file user wants to open
		file_stream = fopen(("." + (*it)->request.URI).c_str(), "rb");
		file_str_2 = fopen(("." + (*it)->request.URI).c_str(), "rb");
	}
	if(file_stream == nullptr)
	{
		//For errors
		cout << GREEN << "No file could be opened. Opening 404 instead.\n\n\n" << RESET_LINE;
		(*it)->response.status_code = "404";
		file_stream = fopen(ERROR_404_PATH, "rb");
		file_str_2 = fopen(ERROR_404_PATH, "rb");
		(*it)->request.URI = "/database/Error_404.png";
	}

	fseek(file_stream, 0, SEEK_END);
	(*it)->response.content_lenght = ftell(file_stream);

	rewind(file_stream);
	std::string binaryString;
	// this line is creating problems with small files
	std::string extension = getExtensionFromRequestGet(it);
	std::stringstream conv;
	conv << (*it)->response.content_lenght;
	// std::cout << RED << "Extension: " << extension << std::endl;
	// std::cout << "Reponse Content-Lenght == " << (*it)->response.content_lenght << std::endl;
	//Those values are sent in the header as a response
	(*it)->response.content_type = extension;
	(*it)->response.content_lenght_str = conv.str();
	createAndSendResponseHeaders(it);
	(*it)->response.body_fd = fileno(file_str_2);

	// std::cout << (*it)->response.headers << std::endl;
	rewind(file_stream);
}

std::string	Server::getExtensionFromRequestGet(std::vector<connecData*>::iterator it)
{
	cout << RED << __func__ << RESET_LINE;
	
	std::string extension;
	int i = (*it)->request.URI.length() - 1;
	while((*it)->request.URI[i] && (*it)->request.URI[i] != '.')
	{
		i--;
	}
	if((*it)->request.URI.compare("/") == 0)
	{
		extension = ".html";
	}
	else if(!(*it)->request.URI[i])
	{
		//Default extension if there is none 
		extension = ".txt";
	}
	else{
		extension.append((*it)->request.URI.substr(i, (*it)->request.URI.length() - i));
	}
	if(getPossibleType(extension, false).empty())
	{
		if((*it)->response.status_code != "404")
			(*it)->response.status_code = "409";
		extension = "text/plain";
		std::cout << "Error "<< std::endl;
	}else{
		if((*it)->response.status_code != "404")
			(*it)->response.status_code = "200";
		extension = getPossibleType(extension, false);
	}
	return extension;
}

std::string	Server::getExtensionFromRequestPost(std::vector<connecData*>::iterator it)
{
	cout << RED << __func__ << RESET_LINE;
	
	std::string extension;
	FILE	*file_stream;
	int i = (*it)->request.URI.length() - 1;
 	if((*it)->request.URI.compare(0,8, "/uploads") == 0)
	{
		if((*it)->request.URI.length() < 9)
		{
			std::cout << "Path not specified correctly " << std::endl;
			return extension;
		}else{
			file_stream = fopen(("." + (*it)->request.URI).c_str(), "wb");
			if(!file_stream)
			{
				std::cout << "Cannot create a file " << std::endl;
				return extension;
			}
		}
	}else{
		return extension;
	}

	int pos = (*it)->request.raw.find("Content-Length: ");
	int j;
	for( j = pos + 15; (*it)->request.raw[j] != '\n'; j++)
	{
	}
	(*it)->response.content_lenght_str =  (*it)->request.raw.substr(pos + 15, j - 15 - pos);
	(*it)->request.content_size = ft_atoi(((*it)->request.raw.substr(pos + 15, j - 15 - pos)).c_str());
	if((*it)->request.content_size  == 0)
	{
		(*it)->response.status_code = "204";
		//Nothing to send 
		return extension;
	}
	int body_pos = (*it)->request.raw.find("\r\n\r\n");
	if(body_pos == (*it)->request.raw.npos)
	{
		(*it)->response.status_code = "204";
		return extension;
	}
	(*it)->request.body = (*it)->request.raw.substr(body_pos + 4, (*it)->request.raw.length() - body_pos - 4);
	while((*it)->request.URI[i] && (*it)->request.URI[i] != '.')
	{
		i--;
	}
	extension.append((*it)->request.URI.substr(i, (*it)->request.URI.length() - i));
	if(getPossibleType(extension, false).empty())
	{
		if((*it)->response.status_code != "404")
			(*it)->response.status_code = "409";
		extension = "text/plain";
		std::cout << "Error "<< std::endl;
	}else{
		if((*it)->response.status_code != "404")
			(*it)->response.status_code = "201";
		extension = getPossibleType(extension, false);
	}

	(*it)->request.body_in_char = (char *)(*it)->request.body.c_str();
	(*it)->request.fd = fileno(file_stream);
	(*it)->response.status_code = "201";

	return extension;
}