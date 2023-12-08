#pragma once

#include <iostream>
//#include <regex> // Regular Expressions. The standard C++ library provides support for regular expressions in the <regex> header through a series of operations.
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

#include "link.h"

std::string get_html_content(const Link&);
