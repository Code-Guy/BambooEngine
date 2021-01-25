#include "utility.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>
#include <vector>
#include <chrono>
#include <thread>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

std::string Utility::exec(const char* cmd)
{
	std::array<char, 128> buffer;
	std::string result;

#ifdef __linux__
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
#elif _WIN32
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
#endif

	if (!pipe) 
	{
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}

bool Utility::replace(std::string& str, const std::string& from, const std::string& to)
{
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

void Utility::replaceAll(std::string& str, const std::string& from, const std::string& to)
{
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

std::string Utility::basename(const std::string& filename)
{
	std::string name = boost::filesystem::path(filename).filename().string();
	std::vector<std::string> strs;
	boost::split(strs, name, boost::is_any_of("."));
	return strs[0];
}

std::vector<std::string> Utility::traverseFiles(const std::string& directory)
{
	boost::filesystem::path p(directory);
	boost::filesystem::directory_iterator end_itr;
	std::vector<std::string> filenames;

	// cycle through the directory
	for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr)
	{
		// If it's not a directory, list it. If you want to list directories too, just remove this check.
		if (is_regular_file(itr->path()))
		{
			// assign current file name to current_file and echo it out to the console.
			filenames.push_back(itr->path().string());
		}
	}

	return filenames;
}

void Utility::sleep(float t)
{
	static constexpr std::chrono::duration<double> MinSleepDuration(0);
	auto start = std::chrono::high_resolution_clock::now();
	while (std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start).count() < t) {
		std::this_thread::sleep_for(MinSleepDuration);
	}
}