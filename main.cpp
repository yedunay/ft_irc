#include "ft_irc.hpp"
#include <cstdlib>

int main(int ac, char **argv)
{
    if(ac != 3)
        return (std::cerr << "Correct usage : ./program <port> <password>" << std::endl), 1;
    try
    {
        Server server(std::atoi(argv[1]), argv[2]);
        server.Launch();
    }
    catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
}