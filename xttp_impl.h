#pragma once

#include <iostream>

#include "xttp_struct.h"

namespace curlpp
{
	class xttp_impl
	{
	public:
		xttp_impl();
		~xttp_impl();


	public:
		result get(const net_data& data) const;
		result post(const net_data& data) const;
		result download(const net_data& data) const;

	private:
		curl_x normalCurl(const net_data& data, std::stringstream* out) const;
		result request(curl_x *mycurl, std::stringstream* rs) const;
		std::string parse_download_file_name(const std::string& url) const;
	};
}
