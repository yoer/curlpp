#include "xttp_impl.h"


#include <sstream>
#include <algorithm>
#include <regex>

#include "xttp_struct.h"


size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
	std::string data((const char*)ptr, (size_t)size * nmemb);
	*((std::stringstream*)stream) << data << std::endl;
	return size * nmemb;
}

static size_t write_file(void *buffer, size_t size, size_t nmemb, void *stream)
{
	curlpp::download_file *out = (curlpp::download_file *)stream;
	if (out && !out->stream) {
		out->stream = fopen(out->filename, "wb");
		if (!out->stream){
			return -1;
		}
	}
	return fwrite(buffer, size, nmemb, out->stream);
}

curlpp::xttp_impl::xttp_impl()
{
}

curlpp::xttp_impl::~xttp_impl()
{
}

curlpp::result curlpp::xttp_impl::get(const net_data& data) const
{
	std::stringstream out;
	curlpp::curl_x curl = normalCurl(data, &out);
	if (nullptr == curl.curl()) {
		return *curlpp::result::fail();
	}

	return request(&curl, &out);
}

curlpp::result curlpp::xttp_impl::post(const net_data& data) const
{
	std::stringstream out;
	curlpp::curl_x mycurl = normalCurl(data, &out);
	if (nullptr == mycurl.curl()) {
		return *result::fail();
	}

	curl_easy_setopt(mycurl.curl(), CURLOPT_POST, 1);
	curl_easy_setopt(mycurl.curl(), CURLOPT_POSTFIELDS, data.post_params().c_str());

	return request(&mycurl, &out);
}

std::string  curlpp::xttp_impl::parse_download_file_name(const std::string& url) const{
	std::regex download_filename_regex("^.*/(.+\..+)$", std::regex_constants::ECMAScript | std::regex_constants::icase);

	std::smatch name_match;
	std::regex_match(url, name_match, download_filename_regex);

	if (2 == name_match.size())
	{
		return name_match[1].str();
	}

	time_t t(0);
	struct tm * time_now = localtime(&t);

	return std::to_string(time_now->tm_year + 1900) + "-" +
		std::to_string(time_now->tm_mon + 1) + "-" +
		std::to_string(time_now->tm_mday) + "-" +
		std::to_string(time_now->tm_hour) + "-" +
		std::to_string(time_now->tm_min) + "-" +
		std::to_string(time_now->tm_sec);
}

curlpp::result curlpp::xttp_impl::download(const net_data& data) const
{
	CURL *curl = curl_easy_init();
	if (nullptr == curl) {
		return *curlpp::result::fail();
	}

	curlpp::download_file file;
	std::string download_file_name = data.download_path() + "/";
	if (data.outfile_name().empty()){
		download_file_name += parse_download_file_name(data.url()).c_str();
	}
	else{
		download_file_name += data.outfile_name();
	}
	file.filename = download_file_name.c_str();
	file.stream = NULL;

	curl_easy_setopt(curl, CURLOPT_URL, data.url().c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); //Prevent "longjmp causes uninitialized stack frame" bug
	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");

	struct curl_slist *chunk = NULL;
	std::for_each(data.headers().begin(), data.headers().end(), [&](const std::string& header){
		chunk = curl_slist_append(chunk, header.c_str());
	});

	curlpp::curl_cleaner clean(&curlpp::curl_x(curl, chunk));
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		return curlpp::result(false, curl_easy_strerror(res));
	}

	return curlpp::result(true, "");
}

curlpp::curl_x curlpp::xttp_impl::normalCurl(const curlpp::net_data& data, std::stringstream* out) const
{
	CURL *curl = curl_easy_init();
	if (nullptr == curl) {
		return curlpp::curl_x(nullptr, nullptr);
	}

	curl_easy_setopt(curl, CURLOPT_URL, data.url().c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, data.timeout());

	struct curl_slist *chunk = NULL;
	std::for_each(data.headers().begin(), data.headers().end(), [&](const std::string& header)
	{
		chunk = curl_slist_append(chunk, header.c_str());
	});
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	return curlpp::curl_x(curl, chunk);
}

curlpp::result curlpp::xttp_impl::request(curlpp::curl_x *mycurl, std::stringstream* rs) const
{
	curl_cleaner clean(mycurl);
	CURLcode res = curl_easy_perform(mycurl->curl());
	if (res != CURLE_OK) {
		return curlpp::result(false, curl_easy_strerror(res));
	}

	return curlpp::result(true, rs->str());
}


