set -xeo pipefail
# curl localhost:8080
# curl error
# Check body
curl localhost:8080/cgi-bin/env_cpp.cgi | grep '<tr><td>GATEWAY_INTERFACE</td><td>CGI/1.1</td></tr>'
curl localhost:8080/cgi-bin/env.sh.cgi | grep "GATEWAY_INTERFACE='CGI/1.1"
curl localhost:8080/cgi-bin/env.sh.cgi | grep "HTTP_USER_AGENT='curl" # check that client request's variable are correctly passed on

# Check header
curl -i localhost:8080/cgi-bin/env.sh.cgi | grep 'Content-Type: text/plain' # should be there (webserv header format)
curl -i localhost:8080/cgi-bin/env_cpp.cgi | grep 'Content-Type: text/html' # should be there (webserv header format)
! curl -i localhost:8080/cgi-bin/env_cpp.cgi | grep 'Content-Type:text/html' # should not (cgi format)

# Check errors
curl -i localhost:8080/cgi-bin/fail.sh.cgi | grep 'HTTP/1.1 500' # cgi script exists and is executable but fails
curl -i localhost:8080/cgi-bin/fail.sh.cgi | grep '<title>500' # idem but body check
curl -i localhost:8080/cgi-bin/fdsagsafdgf.cgi | grep 'HTTP/1.1 404' # cgi script exists and is executable but fails
curl -i localhost:8080/cgi-bin/not_executable_file.cgi | grep 'HTTP/1.1 403' # cgi script exists and is executable but fails
