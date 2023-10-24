#include <iostream>
#include "../ConfigParser/config.h"


int main(int argc, char** argv)
{
    try
    {
    Config config("../Config/config.ini");
    
    std::cout << config.getConfig("db_host") << std::endl;
    std::cout << config.getConfig("db_port") << std::endl;
    std::cout << config.getConfig("db_name") << std::endl;
    std::cout << config.getConfig("db_username") << std::endl;
    std::cout << config.getConfig("db_user_password") << std::endl;
    std::cout << config.getConfig("url") << std::endl;
    std::cout << config.getConfig("searching_depth") << std::endl;
    std::cout << config.getConfig("http_server_port") << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}