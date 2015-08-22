#include "xttp_impl.h"


#include <sstream>
#include <algorithm>

#include "xttp_struct.h"


size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
	std::string data((const char*)ptr, (size_t)size * nmemb);
	*((std::stringstream*)stream) << data << std::endl;
	return size * nmemb;
}

static size_t write_file(void *buffer, size_t size, size_t nmemb, void *stream)
{
	download_file *out = (download_file *)stream;
	if (out && !out->stream) {
		out->stream = fopen(out->filename, "wb");
		if (!out->stream){
			return -1;
		}
	}
	return fwrite(buffer, size, nmemb, out->stream);
}

xttp_impl::xttp_impl()
{
}


xttp_impl::~xttp_impl()
{
}

xttp_rs xttp_impl::get(const xttp_net_data& data) const
{
	std::stringstream out;
	curl_ex curl = normalCurl(data, &out);
	if (nullptr == curl.curl) {
		return *xttp_rs::fail();
	}

	return request(&curl, &out);
}

xttp_rs xttp_impl::post(const xttp_net_data& data) const
{
	std::stringstream out;
	curl_ex mycurl = normalCurl(data, &out);
	if (nullptr == mycurl.curl) {
		return *xttp_rs::fail();
	}

	curl_easy_setopt(mycurl.curl, CURLOPT_POST, 1);
	curl_easy_setopt(mycurl.curl, CURLOPT_POSTFIELDS, data.post_fields().c_str());

	return request(&mycurl, &out);
}

xttp_rs xttp_impl::download(const xttp_net_data& data) const
{
	CURL *curl = curl_easy_init();
	if (nullptr == curl) {
		return *xttp_rs::fail();
	}

	download_file file = {
		data.outfile_name().c_str(), NULL
	};

	curl_easy_setopt(curl, CURLOPT_URL, data.url().c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); //Prevent "longjmp causes uninitialized stack frame" bug
	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");

	struct curl_slist *chunk = NULL;
	std::for_each(data.headers().begin(), data.headers().end(), [&](const std::string& header)
	{
		chunk = curl_slist_append(chunk, header.c_str());
	});

	curl_ex_cleaner clean(&curl_ex(curl, chunk));
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		return xttp_rs(false, curl_easy_strerror(res));
	}

	return xttp_rs(true, "");
}

curl_ex xttp_impl::normalCurl(const xttp_net_data& data, std::stringstream* out) const
{
	CURL *curl = curl_easy_init();
	if (nullptr == curl) {
		return curl_ex(nullptr, nullptr);
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

	return curl_ex(curl, chunk);
}

xttp_rs xttp_impl::request(curl_ex *mycurl, std::stringstream* rs) const
{
	curl_ex_cleaner clean(mycurl);
	CURLcode res = curl_easy_perform(mycurl->curl);
	if (res != CURLE_OK) {
		return xttp_rs(false, curl_easy_strerror(res));
	}

	return xttp_rs(true, rs->str());
}


