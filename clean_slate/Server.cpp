// The address of a socket in the Internet domain consists of the Internet address of the host machine (every computer on the Internet has a unique 32 bit address, often referred to as its IP address).
// In addition, each socket needs a port number on that host.
// Port numbers are 16 bit unsigned integers.
// The lower numbers are reserved in Unix for standard services. For example, the port number for the FTP server is 21. It is important that standard services be at the same port on all computers so that clients will know their addresses.
// However, port numbers above 2000 are generally available.

#include "Server.hpp"

Server::Server(std::string name)
: host_name(name)
{

}

Server::Server()
: host_name("test_server")
{

}
void Server::identify_request(std::string request, int pid) 
{
    Request req(request, pid);
}
void Server::process_request(int socket_num)
{
    char *file_stuff = strdup("");
    std::ifstream infile("rzut_kostka.html");
    int content_count;

    // for( std::string line; getline( infile, line ); )
    // {
       
    //     file_stuff = ft_strjoin(file_stuff, line.c_str());
    //     file_stuff = ft_strjoin(file_stuff, "\n");
    //     content_count += line.length();
    // }
    // std::cout << "Here " << file_stuff << content_count << std::endl;

    // std::cout << "reply\n" << reply << std::endl;
    // std::cout << "file_stuff \n" << file_stuff<< std::endl;
    // reply = ft_strjoin(reply, file_stuff);
    int n;
    char buff[1000];
    bzero(buff, 1000);
    n = read(socket_num, buff, 1000);
    std::cout << "Request to process \n" << buff << std::endl;
    identify_request(buff, newsock_fd);
    if(n < 0)
    {
        std::cerr << "Error reading from the socket";
    }
    // send(newsock_fd, reply, strlen(reply), 0);
    // if(n < 0)
    // {
    //     std::cerr << "Error writing to the buffer" << std::endl;
    // }
}

void Server::run(int port)
{
    this->port = port;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0 )
    {
        std::cerr << "Error opening  a socket";
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if(bind(sock_fd, (struct  sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Binding error";
        exit(1);
    }
    listen(sock_fd, 5);
    clilen = sizeof(cli_address);
    while(1)
    {

        newsock_fd = accept(sock_fd, (struct sockaddr*) &cli_address, &clilen);
        if(newsock_fd < 0)
        {
            std::cerr << "Error on accept";
            exit(1);
        }
        pid = fork();
        if(pid < 0)
        {
            std::cerr << "Error on fork" << std::endl;
        }else if(pid == 0)
        {
            close(sock_fd);
            process_request(newsock_fd);
            exit(0);
        }else{
            close(newsock_fd);
        }

    }
    close(sock_fd);
}
