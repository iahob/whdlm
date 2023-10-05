// ***********************************************************************
// Filename         : HttpRequest.h
// Author           : LIZHENG
// Created          : 2015-03-16
// Description      : 
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_HTTPREQUEST_H
#define ZL_HTTPREQUEST_H
//#include "../../Define.h"
#include "HttpProtocol.h"
#include <map>
#include "../../base/StringUtil.h"
namespace zl{
	namespace net{

		class HttpRequest
		{
		public:
			/// 请求解析出来的key-value对，key忽略大小写
			typedef std::map<std::string, std::string, zl::base::string_cmp_nocase> HeadersMap;

			static bool parseRequest(const char* requestLineandHeader, size_t len, HttpRequest* req);

		public:
			HttpRequest()
				: method_(HttpInvalid)
				, version_(HTTP_VERSION_0_0)
			{

			}

			~HttpRequest()
			{

			}

		public:
			void setMethod(HttpMethod method)        { method_ = method; }
			bool setMethod(const std::string& method)
			{
				if (method == "GET")
					method_ = HttpGet;
				else if (method == "POST")
					method_ = HttpPost;
				else if (method == "HEAD")
					method_ = HttpHead;
				else if (method == "PUT")
					method_ = HttpPut;
				else if (method == "DELETE")
					method_ = HttpDelete;
				else
					method_ = HttpInvalid;
				return method_ != HttpInvalid;
			}
			HttpMethod method() const                { return method_; }
			const std::string methodStr() const
			{
				switch (method_)
				{
				case HttpHead: return "HEAD";
				case HttpGet: return "GET";
				case HttpPost:return "POST";
				default:return "INVALID";
				}
			}

			void setVersion(HttpVersion httpver)     { version_ = httpver; }
			void setVersion(const std::string& httpver)
			{
				version_ = (httpver == "HTTP/1.1" ? HTTP_VERSION_1_1 : HTTP_VERSION_1_0);
			}
			HttpVersion version() const              { return version_; }
			const char* versionStr() const
			{
				return version_ == HTTP_VERSION_1_1 ? "HTTP/1.1" : "HTTP/1.0";
			}

			void setPath(const std::string& url)     { urlpath_ = url; }
			const std::string& path() const          { return urlpath_; }

			void setQuery(const std::string& url)    { query_ = url; }
			const std::string& query() const         { return query_; }

			void setBody(const std::string& body)    { body_ = body; }
			const std::string& body() const          { return body_; }

			//void setReceiveTime(Timestamp t)         { receiveTime_ = t; }
			//Timestamp receiveTime() const            { return receiveTime_; }

			void addHeader(const std::string& key, const std::string& value)
			{
				headers_[key] = value;
			}
			std::string getHeader(const std::string& field) const
			{
				std::string result;
				HeadersMap::const_iterator it = headers_.find(field);
				if (it != headers_.end())
				{
					result = it->second;
				}
				return result;
			}
			const HeadersMap& headers() const  { return headers_; }

			void swap(HttpRequest& that)
			{
				std::swap(method_, that.method_);
				std::swap(version_, that.version_);
				urlpath_.swap(that.urlpath_);
				query_.swap(that.query_);
				// receiveTime_.swap(that.receiveTime_);
				headers_.swap(that.headers_);
			}

			std::string dump() const
			{
				std::string result;
				result = std::string(methodStr()) + " " + path() + "?" + query() + " " + versionStr() + "\r\n";
				for (HeadersMap::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
				{
					result += it->first;
					result += ": ";
					result += it->second;
					result += "\r\n";
				}
				return result;
			}
		private:
			HttpMethod   method_;
			HttpVersion  version_;
			std::string  urlpath_;  // url
			std::string  query_;    // url后面的以?分割的参数(不包括?)
			std::string  body_;
			// Timestamp    receiveTime_;

			HeadersMap   headers_;
		};
	}
}
#endif /* ZL_HTTPREQUEST_H */
