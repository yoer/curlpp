#include "Httper.h"

#include <sstream>
#include <iostream>
#include <algorithm>
#include <io.h>    
#include <iostream>
#include <sstream>
#include <fstream>

#include "FunAssistant.h"
#include "FileAssistant.h"


using namespace std;

HttpRs* HttpRs::g_curlFailed = nullptr;

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
	string data((const char*)ptr, (size_t)size * nmemb);
	*((stringstream*)stream) << data << endl;
	return size * nmemb;
}

size_t write_file(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}
//////////////////////////////////////////////////////////////////////////


HttpRs* HttpRs::CurlFailed()
{
	if (nullptr == g_curlFailed) {
		g_curlFailed = new HttpRs(false, "");
	}

	return g_curlFailed;
}


//////////////////////////////////////////////////////////////////////////
void Httper::globalInit()
{
	curl_global_init(CURL_GLOBAL_ALL);
}

void Httper::globalClean()
{
	curl_global_cleanup();
}

HttpRs Httper::get(const std::string& url)
{
	std::stringstream out;
	MyCurl curl = normalCurl(url, std::vector <std::string>(), &out);
	if (nullptr == curl.curl) {
		return *HttpRs::CurlFailed();
	}
	
	return request(&curl, &out);
}

HttpRs Httper::post(const std::string& url, const std::string& postFields)
{
	std::stringstream out;
	MyCurl mycurl = normalCurl(url, std::vector <std::string>(), &out);
	if (nullptr == mycurl.curl) {
		return *HttpRs::CurlFailed();
	}

	curl_easy_setopt(mycurl.curl, CURLOPT_POST, 1);
	curl_easy_setopt(mycurl.curl, CURLOPT_POSTFIELDS, postFields.c_str());

	return request(&mycurl, &out);
}

HttpRs Httper::request(MyCurl *mycurl, std::stringstream* rs)
{
	AutoCleanMyCurl clean(mycurl);
	CURLcode res = curl_easy_perform(mycurl->curl);
	if (res != CURLE_OK) {
		return HttpRs(false, curl_easy_strerror(res));
	}

	return HttpRs(true, rs->str());
}

HttpRs Httper::download(const std::string& url, const std::vector<std::string>& headers, const std::string& outfilename)
{
	CURL *curl = curl_easy_init();
	if (nullptr == curl) {
		return *HttpRs::CurlFailed();
	}

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file);
	FILE *fp = fopen(outfilename.c_str(), "wb");
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); //Prevent "longjmp causes uninitialized stack frame" bug
	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");

	struct curl_slist *chunk = NULL;
	std::for_each(headers.begin(), headers.end(), [&](const std::string& header)
	{
		chunk = curl_slist_append(chunk, header.c_str());
	});

	AutoCleanMyCurl clean(&MyCurl(curl, chunk));
	CURLcode res = curl_easy_perform(curl);
	fclose(fp);
	if (res != CURLE_OK) {
		return HttpRs(false, curl_easy_strerror(res));
	}

	return HttpRs(true, "");
}

MyCurl Httper::normalCurl(const std::string& url, const std::vector<std::string>& headers, std::stringstream* out)
{
	CURL *curl = curl_easy_init();
	if (nullptr == curl) {
		return MyCurl(nullptr,nullptr);
	}

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, g_timeout);

	struct curl_slist *chunk = NULL;
	std::for_each(headers.begin(), headers.end(), [&](const std::string& header)
	{
		chunk = curl_slist_append(chunk, header.c_str());
	});
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	return MyCurl(curl, chunk);
}

AutoCleanMyCurl::~AutoCleanMyCurl()
{
	if (nullptr != mycurl->curl) {
		curl_easy_cleanup(mycurl->curl);
	}
	if (nullptr != mycurl->chunk) {
		curl_slist_free_all(mycurl->chunk);
	}
}
