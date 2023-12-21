#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <iostream>
#include "http_connection.h"
#include <Windows.h>
#include "../ConfigParser/config.h"


void httpServer(tcp::acceptor& acceptor, tcp::socket& socket)
{
	acceptor.async_accept(socket,
		[&](beast::error_code ec)
		{
			if (!ec)
				std::make_shared<HttpConnection>(std::move(socket))->start();
			httpServer(acceptor, socket);
		});
}


int main(int argc, char** argv)
{
	try
	{
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);
		
		Config config("../Config/config.ini");

		auto const address = net::ip::make_address("127.0.0.1");
		unsigned short port = static_cast<unsigned short>(std::stoi(config.getConfig("http_server_port")));

		net::io_context ioc{1};

		tcp::acceptor acceptor{ioc, { address, port }};
		tcp::socket socket{ioc};
		httpServer(acceptor, socket);

		std::cout << "Open browser and connect to http://127.0.0.1:" << port << " to see the web server operating" << std::endl;

		ioc.run();

	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		
		return EXIT_FAILURE;
	}
	catch (...)
	{
		std::cout << "General error" << std::endl;
	}

	return 0;
}