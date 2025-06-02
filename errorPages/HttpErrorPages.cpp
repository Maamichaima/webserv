#include "../_includes/client.hpp"

std::map<int, std::string> client::errorPages;
std::map<int, std::string> client::description;


void client::setErrorPages()
{
	errorPages[100] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>100 Continue</title><style>"
	"body{background-color:#e2f0f7;color:#0c5460;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#d1e4f0;padding:40px;border-radius:8px;display:inline-block;border:1px solid#c0d9e9;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>100 Continue</h1><p>The server has received the request headers.</p></div></body></html>\n";

	errorPages[101] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>101 Switching Protocols</title><style>"
	"body{background-color:#e2f0f7;color:#0c5460;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#d1e4f0;padding:40px;border-radius:8px;display:inline-block;border:1px solid#c0d9e9;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>101 Switching Protocols</h1><p>Server is switching protocols as requested.</p></div></body></html>\n";

	errorPages[102] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>102 Processing</title><style>"
	"body{background-color:#e2f0f7;color:#0c5460;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#d1e4f0;padding:40px;border-radius:8px;display:inline-block;border:1px solid#c0d9e9;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>102 Processing</h1><p>Server is processing your request.</p></div></body></html>\n";

	errorPages[103] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>103 Early Hints</title><style>"
	"body{background-color:#e2f0f7;color:#0c5460;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#d1e4f0;padding:40px;border-radius:8px;display:inline-block;border:1px solid#c0d9e9;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>103 Early Hints</h1><p>Preliminary headers for resource optimization.</p></div></body></html>\n";

	// Successful 2xx
	errorPages[200] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>200 OK</title><style>"
	"body{background-color:#d4edda;color:#155724;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#c3e6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid#b1dfbb;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>200 OK</h1><p>Request succeeded as expected.</p></div></body></html>\n";

	errorPages[201] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>201 Created</title><style>"
	"body{background-color:#d4edda;color:#155724;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#c3e6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid#b1dfbb;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>201 Created</h1><p>New resource successfully created.</p></div></body></html>\n";

	errorPages[202] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>202 Accepted</title><style>"
	"body{background-color:#d4edda;color:#155724;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#c3e6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid#b1dfbb;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>202 Accepted</h1><p>Request accepted for processing.</p></div></body></html>\n";

	errorPages[203] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>203 Non-Authoritative Information</title><style>"
	"body{background-color:#d4edda;color:#155724;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#c3e6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid#b1dfbb;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>203 Non-Authoritative Information</h1><p>Transformed response from a third-party.</p></div></body></html>\n";

	errorPages[204] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>204 No Content</title><style>"
	"body{background-color:#d4edda;color:#155724;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#c3e6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid#b1dfbb;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>204 No Content</h1><p>Request succeeded with no response body.</p></div></body></html>\n";

	errorPages[205] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>205 Reset Content</title><style>"
	"body{background-color:#d4edda;color:#155724;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#c3e6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid#b1dfbb;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>205 Reset Content</h1><p>Document view should be reset.</p></div></body></html>\n";

	errorPages[206] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>206 Partial Content</title><style>"
	"body{background-color:#d4edda;color:#155724;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#c3e6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid#b1dfbb;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>206 Partial Content</h1><p>Partial GET request successful.</p></div></body></html>\n";

	errorPages[207] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>207 Multi-Status</title><style>"
	"body{background-color:#d4edda;color:#155724;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#c3e6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid#b1dfbb;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>207 Multi-Status</h1><p>Multiple resources status information.</p></div></body></html>\n";

	errorPages[208] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>208 Already Reported</title><style>"
	"body{background-color:#d4edda;color:#155724;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#c3e6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid#b1dfbb;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>208 Already Reported</h1><p>Binding members already reported.</p></div></body></html>\n";

	errorPages[226] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>226 IM Used</title><style>"
	"body{background-color:#d4edda;color:#155724;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#c3e6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid#b1dfbb;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>226 IM Used</h1><p>Instance manipulation applied.</p></div></body></html>\n";
	errorPages[300] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>300 Multiple Choices</title><style>"
	"body{background-color:#e2e3e5;color:#383d41;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#d6d8db;padding:40px;border-radius:8px;display:inline-block;border:1px solid #c6c8ca;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>300 Multiple Choices</h1><p>The requested resource has multiple options.</p></div></body></html>\n";

	errorPages[301] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>301 Moved Permanently</title><style>"
	"body{background-color:#d1ecf1;color:#0c5460;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#bee5eb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #abdde5;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>301 Moved Permanently</h1><p>The resource has been moved permanently to a new URL.</p></div></body></html>\n";

	errorPages[302] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>302 Found</title><style>"
	"body{background-color:#e2e3e5;color:#383d41;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#d6d8db;padding:40px;border-radius:8px;display:inline-block;border:1px solid #c6c8ca;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>302 Found</h1><p>The requested resource has been temporarily moved to a different URL.</p></div></body></html>\n";

	errorPages[303] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>303 See Other</title><style>"
	"body{background-color:#d1ecf1;color:#0c5460;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#bee5eb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #abdde5;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>303 See Other</h1><p>The response to the request can be found under a different URI.</p></div></body></html>\n";

	errorPages[304] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>304 Not Modified</title><style>"
	"body{background-color:#e2e3e5;color:#383d41;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#d6d8db;padding:40px;border-radius:8px;display:inline-block;border:1px solid #c6c8ca;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>304 Not Modified</h1><p>The resource has not been modified since the last request.</p></div></body></html>\n";

	errorPages[305] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>305 Use Proxy</title><style>"
	"body{background-color:#fff3cd;color:#856404;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#ffeeba;padding:40px;border-radius:8px;display:inline-block;border:1px solid #ffeeba;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>305 Use Proxy</h1><p>The requested resource is only available through a proxy.</p></div></body></html>\n";

	errorPages[307] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>307 Temporary Redirect</title><style>"
	"body{background-color:#d1ecf1;color:#0c5460;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#bee5eb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #abdde5;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>307 Temporary Redirect</h1><p>The resource is temporarily located at a different URI.</p></div></body></html>\n";

	errorPages[308] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>308 Permanent Redirect</title><style>"
	"body{background-color:#d1ecf1;color:#0c5460;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#bee5eb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #abdde5;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>308 Permanent Redirect</h1><p>The resource has been permanently moved to a new URI.</p></div></body></html>\n";

	errorPages[400] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>400 Bad Request</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>400 Bad Request</h1><p>Sorry, your request could not be understood by the server.</p><p>Please check your URL or try again later.</p></div></body></html>\n";

	errorPages[401] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>401 Unauthorized</title><style>"
	"body{background-color:#fff3cd;color:#856404;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#ffeeba;padding:40px;border-radius:8px;display:inline-block;border:1px solid #ffeeba;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>401 Unauthorized</h1><p>You are not authorized to access the requested resource.</p></div></body></html>\n";

	errorPages[402] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>402 Payment Required</title><style>"
	"body{background-color:#fff3cd;color:#856404;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#ffeeba;padding:40px;border-radius:8px;display:inline-block;border:1px solid #ffeeba;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>402 Payment Required</h1><p>Payment is required to access the requested resource.</p></div></body></html>\n";

	errorPages[403] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>403 Forbidden</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>403 Forbidden</h1><p>You do not have permission to access the requested resource.</p></div></body></html>\n";

	errorPages[404] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>404 Not Found</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>404 Not Found</h1><p>The requested resource could not be found on this server.</p></div></body></html>\n";

	errorPages[405] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>405 Method Not Allowed</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>405 Method Not Allowed</h1><p>The method is not allowed for the requested URL.</p></div></body></html>\n";

	errorPages[406] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>406 Not Acceptable</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>406 Not Acceptable</h1><p>The requested resource cannot generate content matching your Accept headers.</p></div></body></html>\n";

	errorPages[407] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>407 Proxy Authentication Required</title><style>"
	"body{background-color:#fff3cd;color:#856404;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#ffeeba;padding:40px;border-radius:8px;display:inline-block;border:1px solid #ffeeba;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>407 Proxy Authentication Required</h1><p>Proxy authentication is needed to access this resource.</p></div></body></html>\n";

	errorPages[408] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>408 Request Timeout</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>408 Request Timeout</h1><p>The server timed out waiting for your request.</p></div></body></html>\n";

	errorPages[409] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>409 Conflict</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>409 Conflict</h1><p>Your request conflicts with the current state of the resource.</p></div></body></html>\n";

	errorPages[410] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>410 Gone</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>410 Gone</h1><p>The requested resource is permanently gone.</p></div></body></html>\n";

	errorPages[411] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>411 Length Required</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>411 Length Required</h1><p>Content-Length header is required for this request.</p></div></body></html>\n";

	errorPages[412] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>412 Precondition Failed</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>412 Precondition Failed</h1><p>One or more request preconditions failed.</p></div></body></html>\n";

	errorPages[413] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>413 Payload Too Large</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>413 Payload Too Large</h1><p>Your request payload is too large to process.</p></div></body></html>\n";

	errorPages[414] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>414 URI Too Long</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>414 URI Too Long</h1><p>The requested URL exceeds the maximum allowed length.</p></div></body></html>\n";

	errorPages[415] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>415 Unsupported Media Type</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>415 Unsupported Media Type</h1><p>The media format of your request is not supported.</p></div></body></html>\n";

	errorPages[416] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>416 Range Not Satisfiable</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>416 Range Not Satisfiable</h1><p>The requested range cannot be fulfilled.</p></div></body></html>\n";

	errorPages[417] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>417 Expectation Failed</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>417 Expectation Failed</h1><p>Server cannot meet the requirements of the Expect header.</p></div></body></html>\n";

	errorPages[418] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>418 I'm a Teapot</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>418 I'm a Teapot</h1><p>The server is a teapot and cannot brew coffee.</p></div></body></html>\n";

	errorPages[421] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>421 Misdirected Request</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>421 Misdirected Request</h1><p>Request was sent to a server unable to respond.</p></div></body></html>\n";

	errorPages[422] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>422 Unprocessable Entity</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>422 Unprocessable Entity</h1><p>Your request contains semantic errors.</p></div></body></html>\n";

	errorPages[423] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>423 Locked</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>423 Locked</h1><p>The requested resource is locked.</p></div></body></html>\n";

	errorPages[424] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>424 Failed Dependency</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>424 Failed Dependency</h1><p>The request failed due to a dependency failure.</p></div></body></html>\n";

	errorPages[425] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>425 Too Early</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>425 Too Early</h1><p>Your request is being processed and might be replayed.</p></div></body></html>\n";

	errorPages[426] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>426 Upgrade Required</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>426 Upgrade Required</h1><p>Protocol upgrade is required for this request.</p></div></body></html>\n";

	errorPages[428] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>428 Precondition Required</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>428 Precondition Required</h1><p>Your request requires a precondition header.</p></div></body></html>\n";

	errorPages[429] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>429 Too Many Requests</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>429 Too Many Requests</h1><p>You have sent too many requests in a given time.</p></div></body></html>\n";

	errorPages[431] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>431 Request Header Fields Too Large</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>431 Request Header Fields Too Large</h1><p>Your request headers are too large to process.</p></div></body></html>\n";

	errorPages[451] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>451 Unavailable For Legal Reasons</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>451 Unavailable For Legal Reasons</h1><p>This resource is unavailable due to legal reasons.</p></div></body></html>\n";

	// Server Errors (500-511)
	errorPages[500] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>500 Internal Server Error</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>500 Internal Server Error</h1><p>The server encountered an internal error.</p></div></body></html>\n";

	errorPages[501] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>501 Not Implemented</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>501 Not Implemented</h1><p>The server does not support this functionality.</p></div></body></html>\n";

	errorPages[502] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>502 Bad Gateway</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>502 Bad Gateway</h1><p>Server received an invalid response from upstream.</p></div></body></html>\n";

	errorPages[503] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>503 Service Unavailable</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>503 Service Unavailable</h1><p>The service is temporarily unavailable.</p></div></body></html>\n";

	errorPages[504] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>504 Gateway Timeout</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>504 Gateway Timeout</h1><p>Upstream server did not respond in time.</p></div></body></html>\n";

	errorPages[505] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>505 HTTP Version Not Supported</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>505 HTTP Version Not Supported</h1><p>The HTTP version is not supported.</p></div></body></html>\n";

	errorPages[506] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>506 Variant Also Negotiates</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>506 Variant Also Negotiates</h1><p>Content negotiation resulted in circular reference.</p></div></body></html>\n";

	errorPages[507] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>507 Insufficient Storage</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>507 Insufficient Storage</h1><p>Server cannot store the representation.</p></div></body></html>\n";

	errorPages[508] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>508 Loop Detected</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>508 Loop Detected</h1><p>Server detected an infinite loop processing your request.</p></div></body></html>\n";

	errorPages[510] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>510 Not Extended</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>510 Not Extended</h1><p>Further extensions to your request are required.</p></div></body></html>\n";

	errorPages[511] = "<!DOCTYPE html>\n"
	"<html><head><meta charset=\"UTF-8\"><title>511 Network Authentication Required</title><style>"
	"body{background-color:#f8d7da;color:#721c24;font-family:Arial,sans-serif;text-align:center;padding-top:100px;}"
	"div{background:#f5c6cb;padding:40px;border-radius:8px;display:inline-block;border:1px solid #f1b0b7;}"
	"h1{font-size:48px;}p{font-size:18px;}"
	"</style></head><body><div><h1>511 Network Authentication Required</h1><p>Network authentication is required for access.</p></div></body></html>\n";
	

	
	// client::description[400] = "Bad Request";
	// client::description[404] = "Not Found";
	// client::description[201] = "Created";
	// client::description[500] = "Internal Server Error";
	// client::description[411] = "Length Required";
	// client::description[501] = "Not Implemented";
	// client::description[505] = "Version Not Supported";
	// client::description[405] = "Version Not Supported";
}
void client::setDescription()
{
	description[200] = "OK";
	description[201] = "Created";
	description[202] = "Accepted";
	description[203] = "Non-Authoritative Information";
	description[204] = "No Content";
	description[205] = "Reset Content";
	description[206] = "Partial Content";
	description[207] = "Multi-Status";
	description[208] = "Already Reported";
	description[226] = "IM Used";
	
	description[300] = "Multiple Choices";
	description[301] = "Moved Permanently";
	description[302] = "Found";
	description[303] = "See Other";
	description[304] = "Not Modified";
	description[305] = "Use Proxy";
	description[306] = "Switch Proxy";
	description[307] = "Temporary Redirect";
	description[308] = "Permanent Redirect";
	
	description[400] = "Bad Request";
	description[401] = "Unauthorized";
	description[402] = "Payment Required";
	description[403] = "Forbidden";
	description[404] = "Not Found";
	description[405] = "Method Not Allowed";
	description[406] = "Not Acceptable";
	description[407] = "Proxy Authentication Required";
	description[408] = "Request Timeout";
	description[409] = "Conflict";
	description[410] = "Gone";
	description[411] = "Length Required";
	description[412] = "Precondition Failed";
	description[413] = "Payload Too Large";
	description[414] = "URI Too Long";
	description[415] = "Unsupported Media Type";
	description[416] = "Range Not Satisfiable";
	description[417] = "Expectation Failed";
	description[418] = "I'm a Teapot";
	description[421] = "Misdirected Request";
	description[422] = "Unprocessable Entity";
	description[423] = "Locked";
	description[424] = "Failed Dependency";
	description[425] = "Too Early";
	description[426] = "Upgrade Required";
	description[428] = "Precondition Required";
	description[429] = "Too Many Requests";
	description[431] = "Request Header Fields Too Large";
	description[451] = "Unavailable For Legal Reasons";
	
	description[500] = "Internal Server Error";
	description[501] = "Not Implemented";
	description[502] = "Bad Gateway";
	description[503] = "Service Unavailable";
	description[504] = "Gateway Timeout";
	description[505] = "HTTP Version Not Supported";
	description[506] = "Variant Also Negotiates";
	description[507] = "Insufficient Storage";
	description[508] = "Loop Detected";
	description[510] = "Not Extended";
	description[511] = "Network Authentication Required";
}
