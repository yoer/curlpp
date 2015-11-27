#pragma once

#include <string>
#include <vector>

#include "xttp_struct.h"
#include "xttp_impl.h"


namespace curlpp
{
	class hxxp
	{
	public:
		static void globalInit();
		static void globalClean();
        
	public:
		result get();
		result post();
		result download() const;

	public:
		hxxp& timeout(int val);
		hxxp& url(const std::string& val);

		hxxp& add_header(const std::string& val);
		hxxp& headers(const std::vector<std::string>& val);
		hxxp& add_post_params(const std::string& key, const std::string& value);

		hxxp& outfile_name(const std::string& val);
		hxxp& download_path(const std::string& val);
        hxxp& md5(const std::string& val);
	private:
		xttp_impl m_impl;
		net_data m_net_data;
	};
}

