#pragma once

#include <iostream>
#include "../ConfigParser/config.h"
#include "indexer.h"
#include "link.h"
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>
#include <vector>
#include <string>

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <openssl/ssl.h>

class Spider
{
public:
	Spider() = default;
	Spider(std::mutex&, std::condition_variable&, std::queue<std::function<void()>>&, Config&);

	virtual ~Spider() = default;

private:
	bool is_text(const boost::beast::multi_buffer::const_buffers_type&);
	std::string get_html_content(const Link&);

	std::mutex& mtx;
	std::condition_variable& cv;
	std::queue<std::function<void()>>& tasks;
	const Config& config;

public:
	void parse_link(const Link&, int);

};