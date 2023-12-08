#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>
#include "http_utils.h"
#include "../ConfigParser/config.h"
#include "indexer.h"
#include <pqxx/pqxx>

std::mutex mtx;
std::condition_variable cv;
std::queue<std::function<void()>> tasks;
bool exitThreadPool = false;

void threadPoolWorker() 
{
	std::unique_lock<std::mutex> lock(mtx);
	while (!exitThreadPool || !tasks.empty()) 
	{
		if (tasks.empty()) 
		{
			cv.wait(lock);
		}
		else 
		{
			auto task = tasks.front();
			tasks.pop();
			lock.unlock();
			task();
			lock.lock();
		}
	}
}

void parse_link(const Link& link, int depth, const Config& config)
{
	try 
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		std::string html = get_html_content(link);
		if (html.size() == 0)
		{
			std::cout << "Failed to get HTML Content" << std::endl;
			return;
		}
		else 
		{
			Indexer indexer;
			indexer.parse_words(html);
			indexer.parse_links(html, link);
			indexer.push_data_to_db(
				config.getConfig("db_host"),
				config.getConfig("db_port"),
				config.getConfig("db_name"),
				config.getConfig("db_username"),
				config.getConfig("db_user_password"),
				link);
			{
				std::lock_guard<std::mutex> lock(mtx);
				std::cout << "<<<<<<<< <<<<<<<< <<<<<<<< <<<<<<<< <<<<<<<< <<<<<<<< <<<<<<<< THREAD ID = " << std::this_thread::get_id << std::endl;			
				cv.notify_all();
			}
			if (depth > 1) 
			{
				std::vector<Link> links = indexer.get_links();
				std::lock_guard<std::mutex> lock(mtx);
				for (const auto& subLink : links)
				{
					tasks.push([subLink, depth, config]() { parse_link(subLink, depth - 1, config); }); 
				}
				cv.notify_one();
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}


int main(int argc, char** argv)
{
	try 
	{
		int numThreads = std::thread::hardware_concurrency();
		std::vector<std::thread> threadPool;
		for (int i = 0; i < numThreads; ++i)
		{
			threadPool.emplace_back(threadPoolWorker);
		}
		Config config("../Config/config.ini");
		int searching_depth = std::stoi(config.getConfig("searching_depth"));
		Link link{ ProtocolType::HTTPS, config.getConfig("domain"), config.getConfig("query") };
		{
			std::lock_guard<std::mutex> lock(mtx);
			tasks.push([link, searching_depth, config]() { parse_link(link, searching_depth, config); });
			cv.notify_one();
		}
		std::this_thread::sleep_for(std::chrono::seconds(2));
		{
			std::lock_guard<std::mutex> lock(mtx);
			exitThreadPool = true;
			cv.notify_all();
		}
		for (auto& t : threadPool) 
		{
			t.join();
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (pqxx::sql_error e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}
