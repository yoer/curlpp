#include "Xttp.h"


//////////////////////////////////////////////////////////////////////////
void xttp::globalInit()
{
	curl_global_init(CURL_GLOBAL_ALL);
}

void xttp::globalClean()
{
	curl_global_cleanup();
}

xttp_rs xttp::get() const
{
	return m_impl.get(m_net_data);
}

xttp_rs xttp::post() const
{
	return m_impl.post(m_net_data);
}

xttp_rs xttp::download() const
{
	return m_impl.download(m_net_data);
}

xttp& xttp::headers(const std::vector<std::string>& val)
{
	m_net_data.headers(val); 
	return *this;
}

xttp& xttp::outfile_name(const std::string& val)
{
	m_net_data.outfile_name(val); 
	return *this;
}

xttp& xttp::post_fields(const std::string& val)
{
	m_net_data.post_fields(val); 
	return *this;
}

xttp& xttp::url(const std::string& val)
{
	m_net_data.url(val);
	return *this;
}

xttp& xttp::timeout(int val)
{
	m_net_data.timeout(val); 
	return *this;
}
