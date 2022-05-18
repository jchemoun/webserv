set -xeo pipefail
# curl localhost:8080
# curl error
# Check body
curl localhost:8080/cgi/env_cgi | grep '<tr><td>GATEWAY_INTERFACE</td><td>CGI/1.1</td></tr>'
curl localhost:8080/cgi/env_cgi.sh | grep "GATEWAY_INTERFACE='CGI/1.1"

# Check header
curl -i localhost:8080/cgi/env_cgi.sh | grep 'Content-Type: text/plain' # should be there (webserv header format)
curl -i localhost:8080/cgi/env_cgi | grep 'Content-Type: text/html' # should be there (webserv header format)
! curl -i localhost:8080/cgi/env_cgi | grep 'Content-Type:text/html' # should not (cgi format)
