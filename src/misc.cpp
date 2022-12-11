#include "../includes/webserv.hpp"

bool file_exists (const std::string& name)
{
  cout << ON_PINK << __func__ << RESET_LINE;
  struct stat buffer;   
  return (stat(name.c_str(), &buffer) == 0); 
}


std::vector<std::string> split(std::string &str, const char sep, const int &flag)
{
	// cout << ON_PINK << __func__ << RESET_LINE;
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
			// cout << RED << "Value: " << val << RESET_LINE;
		i++;
	}
			// cout << RED << "Value: " << val << RESET_LINE;
			// cout << RED << "Value: " << outputArray[1] << RESET_LINE;
	return outputArray;
}

int	ft_atoi(const char *nptr)
{
	// cout << ON_PINK << __func__ << RESET_LINE;
	int	i;
	int	sign;
	int	out;

	i = 0;
	sign = 1;
	out = 0;
	while (nptr[i] == 32 || (nptr[i] > 8 && nptr[i] < 14))
		i++;
	if (nptr[i] == '+' || nptr[i] == '-')
		sign = (nptr[i++] - 44) * -1;
	while (nptr[i] >= '0' && nptr[i] <= '9')
		out = (out * 10) + (nptr[i++] - '0');
	return (out * sign);
}


struct epoll_event	createEpollStruct(int fdSocket, uint32_t flags)
{
	cout << SKY << __func__ << RESET_LINE;
	
	struct	epoll_event	ev;

	ev.events = flags;
	ev.data.fd = fdSocket;

	return (ev);
}



static int	digits(int n)
{
	int	dig;
	int	x;

	dig = 1;
	x = n;
	if (n >= 0)
	{
		while (x >= 10)
		{
			dig += 1;
			x /= 10;
		}
	}
	if (n < 0)
	{
		while (x <= -10)
		{
			dig += 1;
			x /= 10;
		}
	}
	return (dig);
}

static char	*stringcreate(int dig, int n, char *ptr)
{
	if (n < 0)
	{
		ptr[0] = '-';
		dig += 1;
		ptr[dig] = 0;
		while (dig > 1)
		{
			ptr[dig - 1] = ((n % 10) * (-1)) + 48;
			n /= 10;
			dig--;
		}
	}
	else
	{
		ptr[dig] = 0;
		while (dig > 0)
		{
			ptr[dig - 1] = (n % 10) + 48;
			n /= 10;
			dig--;
		}
	}
	return (ptr);
}

char	*ft_itoa(int n)
{
	int		dig;
	char	*ptr;

	dig = digits(n);
	if (n < 0)
		ptr = (char *)malloc(dig + 2 * sizeof(char));
	else
		ptr = (char *)malloc(dig + 1 * sizeof(char));
	if (ptr == NULL)
		return (NULL);
	ptr = stringcreate(dig, n, ptr);
	return (ptr);
}