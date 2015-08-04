#pragma once

#include <string>
#include <vector>

#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/curlbuild.h>

class HttpRs{
public:
	HttpRs(bool suc, const std::string& rs) :suc(suc), rs(rs)	{}

public:
	static HttpRs* CurlFailed();

public:
	bool suc;
	std::string rs;

private:
	static HttpRs* g_curlFailed;
};

class MyCurl{
public:
	MyCurl(CURL *curl, struct curl_slist *chunk) :curl(curl), chunk(chunk){}

public:
	CURL *curl;
	struct curl_slist *chunk;
};

class AutoCleanMyCurl{
public:
	AutoCleanMyCurl(MyCurl *mycurl) :mycurl(mycurl){}
	virtual ~AutoCleanMyCurl();
private:
	MyCurl *mycurl;
};

class Httper
{
public:
	static void globalInit();
	static void globalClean();

public:
	static HttpRs get(const std::string& url);
	static HttpRs post(const std::string& url, const std::string& postFields);
	static HttpRs download(const std::string& url, const std::vector<std::string>& headers, const std::string& outfilename);

	static bool writeFile(const std::string& outfilename, const std::string& strContent, bool bClear);
private:
	static MyCurl normalCurl(const std::string& url, const std::vector<std::string>& headers, std::stringstream* out);
	static HttpRs request(MyCurl *mycurl, std::stringstream* rs);

private:
	static const int g_timeout = 15;
};


