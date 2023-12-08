#include "spider.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ip = boost::asio::ip;
namespace ssl = boost::asio::ssl;

using tcp = boost::asio::ip::tcp;

Spider::Spider(std::mutex& _mtx, std::condition_variable& _cv, std::queue<std::function<void()>>& _tasks, Config& _config) : mtx(_mtx), cv(_cv), tasks(_tasks), config(_config){}

bool Spider::is_text(const boost::beast::multi_buffer::const_buffers_type& b)
{
	for (auto itr = b.begin(); itr != b.end(); itr++)
	{
		for (int i = 0; i < (*itr).size(); i++)
		{
			if (*((const char*)(*itr).data() + i) == 0)
			{
				return false;
			}
		}
	}

	return true;
}

std::string Spider::get_html_content(const Link& link)
{
	std::string result;

	try
	{
		std::string host = link.hostName;
		std::string query = link.query;

		net::io_context ioc; // The io_context class provides the core I/O functionality for users of the asynchronous I/O objects, including:
		// boost::asio::ip::tcp::socket
		// boost::asio::ip::tcp::acceptor
		// boost::asio::ip::udp::socket
		// deadline_timer
		// The io_context class also includes facilities intended for developers of custom asynchronous services.

		if (link.protocol == ProtocolType::HTTPS)
		{

			ssl::context ctx(ssl::context::tlsv13_client); // SSL Context is a collection of ciphers, protocol versions, trusted certificates, TLS options, TLS extensions etc.
			ctx.set_default_verify_paths(); // Configures the context to use the default directories for finding certification authority certificates.

			beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx); // Creates ssl_stream. ssl_stream provides stream-oriented functionality using OpenSSL.
			stream.set_verify_mode(ssl::verify_none); // Set the peer verification mode.

			stream.set_verify_callback([](bool preverified, ssl::verify_context& ctx) {
				return true; // Accept any certificate
				});


			if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
				beast::error_code ec{ static_cast<int>(::ERR_get_error()), net::error::get_ssl_category() };
				throw beast::system_error{ ec };
			}

			ip::tcp::resolver resolver(ioc); // The ip::basic_resolver class template provides the ability to resolve a query to a list of endpoints.
			get_lowest_layer(stream).connect(resolver.resolve({ host, "https" }));
			get_lowest_layer(stream).expires_after(std::chrono::seconds(30));


			http::request<http::empty_body> req{ http::verb::get, query, 11 }; // Set up an HTTP GET request message.
			req.set(http::field::host, host);
			req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

			stream.handshake(ssl::stream_base::client); //Send the request to the remote host.
			http::write(stream, req);

			beast::flat_buffer buffer; // Buffer is used for reading and must be persisted.
			http::response<http::dynamic_body> res; // Declare a container to hold the response.
			http::read(stream, buffer, res); // Receive the HTTP response.

			if (is_text(res.body().data()))
			{
				result = buffers_to_string(res.body().data());
			}
			else
			{
				std::cout << "This is not a text link, bailing out..." << std::endl;
			}

			beast::error_code ec;
			stream.shutdown(ec); // Shut down SSL on the stream.
			if (ec == net::error::eof) {
				ec = {};
			}

			if (ec) {
				throw beast::system_error{ ec };
			}
		}
		else
		{
			tcp::resolver resolver(ioc);
			beast::tcp_stream stream(ioc);

			auto const results = resolver.resolve(host, "http"); // Look up the domain name.

			stream.connect(results); // Make the connection on the IP address we get from a lookup.

			http::request<http::string_body> req{ http::verb::get, query, 11 }; // Set up an HTTP GET request message.
			req.set(http::field::host, host);
			req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);


			http::write(stream, req); // Send the HTTP request to the remote host.

			beast::flat_buffer buffer; // Buffer is used for reading and must be persisted.

			http::response<http::dynamic_body> res; // Receive the HTTP response.


			http::read(stream, buffer, res); // Receive the HTTP response.

			if (is_text(res.body().data()))
			{
				result = buffers_to_string(res.body().data());
			}
			else
			{
				std::cout << "This is not a text link, bailing out..." << std::endl;
			}

			beast::error_code ec;
			stream.socket().shutdown(tcp::socket::shutdown_both, ec); // Close the socket.

			if (ec && ec != beast::errc::not_connected)
				throw beast::system_error{ ec };

		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return result;
}

void Spider::parse_link(const Link& link, int depth)
{
	try {

		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		std::string html = Spider::get_html_content(link);

		if (html.size() == 0)
		{
			std::cout << "Failed to get HTML Content" << std::endl;
			return;
		}

		Indexer indexer;
		indexer.parse_words(html);
		indexer.parse_links(html, link);

		indexer.push_data_to_db(
			Spider::config.getConfig("db_host"), 
			Spider::config.getConfig("db_port"), 
			Spider::config.getConfig("db_name"), 
			Spider::config.getConfig("db_username"), 
			Spider::config.getConfig("db_user_password"), 
			link);

		std::cout << "THREAD ID = " << std::this_thread::get_id;

		auto links_vector = indexer.get_links();
		
		if (depth > 1) {
			std::lock_guard<std::mutex> lock(mtx);

			for (const auto& subLink : links_vector)
			{
				tasks.push([this, subLink, depth]() { parse_link(subLink, depth - 1); });
			}
			cv.notify_one();
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}