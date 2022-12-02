#include "../includes/webserv.hpp"

bool file_exists (const std::string& name)
{
  struct stat buffer;   
  return (stat(name.c_str(), &buffer) == 0); 
}


std::vector<std::string> split(std::string &str, const char sep, const int &flag)
{
	std::vector<std::string>	outputArray;
	std::stringstream			streamString(str);
	std::string					val;
	std::string					temp;
	int 						i;
	
	i = 0;
	while(std::getline(streamString, val, sep))
	{
		if (flag == 1 && i == 1) //because of the +2 this case only works for headers
		{
			int start = outputArray[0].length() + 2;
			outputArray.push_back(str.substr(start, str.length() - start));
			break ;
		}
		if(val.length() > 0)
			outputArray.push_back(val);

		if (sep == '?')
			cout << RED << "Value: " << val << RESET_LINE;
		i++;
	}
			cout << RED << "Value: " << val << RESET_LINE;
			cout << RED << "Value: " << outputArray[1] << RESET_LINE;
	return outputArray;
}
