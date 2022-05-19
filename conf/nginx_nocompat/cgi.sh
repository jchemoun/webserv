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

# Check errors
curl -i localhost:8080/cgi/fail_cgi.sh | grep 'HTTP/1.1 500' # cgi script exists and is executable but fails
curl -i localhost:8080/cgi/fail_cgi.sh | grep '<title>500' # idem but body check
curl -i localhost:8080/cgi/fdsagsafdgf | grep 'HTTP/1.1 404' # cgi script exists and is executable but fails
curl -i localhost:8080/cgi/not_executable_file | grep 'HTTP/1.1 403' # cgi script exists and is executable but fails
