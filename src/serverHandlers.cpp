#include "../includes/server.hpp"


void	Server::handleCGI(struct epoll_event	ev, std::vector<connecData*>::iterator it)
{
	cout << SKY << __func__ << ": isCGI!" << RESET_LINE;

	if (checkCGIPaths(it) == false)
	{
		cout << RED << "WRONG CGI PATH" << RESET_LINE;
		if ((*it)->response.status_code.compare("500") != 0)
			(*it)->response.status_code = "404";
		setErrorStatusCodeAndRespond(ev, it, (*it)->response.status_code);
		return ;
	}

	(*it)->isCGI = true;
	cout << YELLOW << "URI is: " << (*it)->request.URI << RESET_LINE;
	objectCGI.setEnvironment(it, servConfig);
	std::string sktStr =  ft_itoa((*it)->socket);
	(*it)->fileNameCGI = DEFAULT_CGI_FILE_PATH + sktStr + "_fileCGI.html";
	
	FILE * fileCGI= fopen((*it)->fileNameCGI.c_str(), "w");				//create file to write to
	if (!file_exists((*it)->fileNameCGI))
	{
		cout << SKY << __func__ << (*it)->fileNameCGI.c_str() \
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
		char *arr[2];
		arr[0] = strdup(executablePath.c_str());
		arr[1] = NULL;
		execve((executablePath).c_str(), arr, env);
		perror("\nExecve: ");
		free(env);
		exit(33);
	}
	wait(NULL);
	handleGet(it, ev);
}

void 	Server::handlePost( std::vector<connecData*>::iterator it, struct epoll_event ev)
{
	cout << SKY << __func__ << RESET_LINE;
	std::string extension = getExtensionFromRequestPost(it);
	(*it)->response.content_type = extension;
	createAndSendResponseHeaders(ev, it);
}

void	Server::handleDelete(std::vector<connecData*>::iterator it, struct epoll_event	ev)
{
	cout << SKY << __func__ << RESET_LINE;
	
	std::cout << "Chuj " << (*it)->request.URI << std::endl;
	if((*it)->request.URI.compare(0, 8,"/uploads") == 0)
	{
		std::string ret = ".";
		ret.append((*it)->request.URI);
		if(remove(ret.c_str()) != 0)
		{
			(*it)->response.status_code = "417";
			std::cout << "Cannot remove this file " << "." + (*it)->request.URI << std::endl;
		}else{
			(*it)->response.status_code = "204";
		}
	}else{
		std::cout << "Cannot delete from different directiory than uploads" << std::endl;
		(*it)->response.status_code = "417";
	}
	createAndSendResponseHeaders(ev, it);
}

void	Server::handleGet(std::vector<connecData*>::iterator it, struct epoll_event	ev)
{
	cout << SKY << __func__ << RESET_LINE;
	cout << YELLOW << "URI: "  << (*it)->request.URI << RESET_LINE;
	if ((*it)->isCGI)
	{
		cout << YELLOW << "IS CGI" << RESET_LINE;
		(*it)->request.file_one = fopen((*it)->fileNameCGI.c_str(), "rb");
		(*it)->request.file_two = fopen((*it)->fileNameCGI.c_str(), "rb");
	}
	else if ((*it)->request.URI.compare("/") == 0)
	{
		//if list directory is enabled return this page
		if (servConfig.getDirectoryListing().compare("yes") == 0 )
		{
			(*it)->request.URI = DIR_LISTING_SCRIPT;
			handleCGI(ev, it);
			return ;
		}
		//Root path for welcome page
		(*it)->request.file_one = fopen(DEFAULT_PATH , "rb");
		(*it)->request.file_two  = fopen(DEFAULT_PATH , "rb");
	}
	else if ((*it)->request.URI.compare("/favicon.ico") == 0)
	{
		//Favicon for now streamlined
		(*it)->request.file_one  = fopen(FAV_ICON_PATH, "rb");
		(*it)->request.file_two  = fopen(FAV_ICON_PATH, "rb");

	}
	else if ((*it)->request.URI.compare(0, 46, DEFAULT_CGI_FILE_PATH) == 0)
	{
		(*it)->request.file_one  = fopen((*it)->request.URI.c_str(), "rb");
		(*it)->request.file_two  = fopen((*it)->request.URI.c_str(), "rb");
	}
	else if((*it)->request.URI.compare((*it)->request.URI.length() - 1, 1, "/") != 0)
	{
		//If there is a different file user wants to open
		(*it)->request.file_one  = fopen(("." + (*it)->request.URI).c_str(), "rb");
		(*it)->request.file_two  = fopen(("." + (*it)->request.URI).c_str(), "rb");
	}
	if((*it)->request.file_one  == NULL)
	{
		//For errors
		(*it)->response.status_code = "404";
        createAndSendResponseHeaders(ev, it);
        endConnection(ev);
        return ;
	}

	fseek((*it)->request.file_one , 0, SEEK_END);
	(*it)->response.content_lenght = ftell((*it)->request.file_one);
	rewind((*it)->request.file_one );
	std::string binaryString;
	std::string extension = getExtensionFromRequestGet(it);
	std::stringstream conv;
	conv << (*it)->response.content_lenght;
	(*it)->response.content_type = extension;
	(*it)->response.content_lenght_str = conv.str();
	createAndSendResponseHeaders(ev, it);
	(*it)->response.body_fd = fileno((*it)->request.file_two);
	rewind((*it)->request.file_one);
	fclose((*it)->request.file_one);
}

std::string	Server::getExtensionFromRequestGet(std::vector<connecData*>::iterator it)
{
	cout << SKY << __func__ << RESET_LINE;
	
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
	cout << SKY << __func__ << RESET_LINE;
	
	std::string extension;
	int i = (*it)->request.URI.length() - 1;
 	if((*it)->request.URI.compare(0,8, "/uploads") == 0)
	{
		if((*it)->request.URI.length() < 9)
		{
			std::cout << "Path not specified correctly " << std::endl;
			return extension;
		}else{
			(*it)->request.file_two = fopen(("." + (*it)->request.URI).c_str(), "wb");
			if(!(*it)->request.file_two)
			{
				std::cout << "Cannot create a file " << std::endl;
				return extension;
			}
		}
	}else{
		return extension;
	}

	long unsigned int pos = (*it)->request.raw.find("content-length: ");
	if(pos == (*it)->request.raw.npos)
	{
		pos = (*it)->request.raw.find("Content-Length: ");
		if(pos == (*it)->request.raw.npos)
		{
			std::cout << "Content-length not specified correctly " << std::endl;
			return extension;
		}
	}
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
	unsigned body_pos = (*it)->request.raw.find("\r\n\r\n");
	if(body_pos == (*it)->request.raw.npos)
	{
		(*it)->response.status_code = "204";
		return extension;
	}
	(*it)->request.body = (*it)->request.raw.substr(body_pos + 4, (*it)->request.raw.length() - body_pos - 4);
	while((*it)->request.URI[i] && (*it)->request.URI[i] != '.')
	{
		if(i == 0)
			break; 
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
	(*it)->request.fd = fileno((*it)->request.file_two);
	(*it)->response.status_code = "201";

	return extension;
}
