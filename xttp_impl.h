#pragma once

#include <iostream>

#include "xttp_struct.h"

class xttp_impl
{
public:
	xttp_impl();
	~xttp_impl();


public:
	xttp_rs get(const xttp_net_data& data) const;
	xttp_rs post(const xttp_net_data& data) const;
	xttp_rs download(const xttp_net_data& data) const;

private:
	curl_ex normalCurl(const xttp_net_data& data, std::stringstream* out) const;
	xttp_rs request(curl_ex *mycurl, std::stringstream* rs) const;
};

