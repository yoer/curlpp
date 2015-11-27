#include "xttp_struct.h"


#include <algorithm>

//////////////////////////////////////////////////////////////////////////
curlpp::result* curlpp::result::g_curlFailed = nullptr;

curlpp::result::result(bool state, const std::string& value) :m_state(state), m_value(value)
{
    
}

curlpp::result* curlpp::result::fail()
{
	if (nullptr == g_curlFailed) {
		g_curlFailed = new curlpp::result(false, "");
	}

	return g_curlFailed;
}

void curlpp::result::value(std::string val)
{
	m_value = val;
}

const std::string& curlpp::result::value() const
{
	return m_value;
}

void curlpp::result::state(bool val)
{
	m_state = val;
}

bool curlpp::result::state() const
{
	return m_state;
}

//////////////////////////////////////////////////////////////////////////

curlpp::curl_cleaner::~curl_cleaner()
{
    if (nullptr != mycurl) {
        delete mycurl;
        mycurl = nullptr;
    }
}


//////////////////////////////////////////////////////////////////////////
curlpp::net_data::net_data()
: m_timeout(curlpp::net_default_data::timeout())
, m_download_path(curlpp::net_default_data::download_path())
, m_needdegist(false)
{
}

void curlpp::net_data::headers(const std::vector<std::string>& headers)
{
	m_headers = headers;
}

const std::vector<std::string>& curlpp::net_data::headers() const
{
	return m_headers;
}

void curlpp::net_data::outfile_name(const std::string& outfile_name)
{
	m_outfile_name = outfile_name;
}

const std::string& curlpp::net_data::outfile_name() const
{
	return m_outfile_name;
}

void curlpp::net_data::url(const std::string& url)
{
	m_url = url;
}

const std::string& curlpp::net_data::url() const
{
	return m_url;
}

void curlpp::net_data::timeout(int val)
{
	m_timeout = val;
}

int curlpp::net_data::timeout() const
{
	return m_timeout;
}

void curlpp::net_data::download_path(const std::string& val)
{
	m_download_path = val;
}

const std::string& curlpp::net_data::download_path() const
{
	return m_download_path;
}

void curlpp::net_data::md5(const std::string& val) {
    m_needdegist = true;
    m_md5 = val;
}

const std::string& curlpp::net_data::md5() const {
    return m_md5;
}

bool curlpp::net_data::need_degist() const {
    return m_needdegist;
}

void curlpp::net_data::append_header(const std::string& header) 
{
	m_headers.push_back(header);
}

std::string curlpp::net_data::post_params() const
{
	return m_post_params.format();
}

curlpp::url_post_params& curlpp::net_data::post_params_write()
{
	return m_post_params;
}


//////////////////////////////////////////////////////////////////////////
int curlpp::net_default_data::g_timeout = 15;
std::string curlpp::net_default_data::m_download_path = ".";

void curlpp::net_default_data::timeout(int val)
{
	g_timeout = val;
}

int curlpp::net_default_data::timeout()
{
	return g_timeout;
}

void curlpp::net_default_data::download_path(std::string val)
{
	m_download_path = val;
}

std::string& curlpp::net_default_data::download_path()
{
	return m_download_path;
}

//////////////////////////////////////////////////////////////////////////
curlpp::curl_x::curl_x(CURL *curl, struct curl_slist *chunk) :m_curl(curl), m_chunk(chunk)
{

}

curlpp::curl_x::~curl_x()
{
    if (nullptr != m_curl) {
        curl_easy_cleanup(m_curl);
    }
    if (nullptr != m_chunk) {
        curl_slist_free_all(m_chunk);
    }
}

//////////////////////////////////////////////////////////////////////////
void curlpp::url_post_params::clear()
{
	m_values.clear();
}

std::string curlpp::url_post_params::format() const
{
	std::string param_formated;
	std::for_each(m_values.begin(), m_values.end(), [&](const post_prarm_type& param){
		param_formated += param.first + "=" + param.second + "&";
	});

	const auto trim_idx = param_formated.find_last_of("&");
	if (std::string::npos != trim_idx)
	{
		param_formated = param_formated.substr(0, trim_idx);
	}

	return param_formated;
}

void curlpp::url_post_params::add_params(const std::string& key, const std::string& value)
{
	m_values.push_back(std::make_pair(key,value));
}