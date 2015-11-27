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
		result get(const net_data& data);
		result post(const net_data& data);
		result download(const net_data& data) const;

	private:
		curl_x* normalCurl(const net_data& data, std::stringstream* out) const;
		result request(curl_x *mycurl, std::stringstream* rs);
		std::string parse_download_file_name(const std::string& url) const;
        void format_result(char* rs);
	};
}
