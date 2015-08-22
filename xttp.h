#pragma once

#include <string>
#include <vector>

#include "xttp_struct.h"
#include "xttp_impl.h"


class xttp
{
public:
	static void globalInit();
	static void globalClean();


public:
	xttp_rs get() const;
	xttp_rs post() const;
	xttp_rs download() const;

public:
	xttp& timeout(int val);
	xttp& url(const std::string& val);
	xttp& post_fields(const std::string& val);
	xttp& outfile_name(const std::string& val);
	xttp& headers(const std::vector<std::string>& val);

private:
	xttp_impl m_impl;
	xttp_net_data m_net_data;
	static const int g_timeout = 15;
};


