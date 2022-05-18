set -xeo pipefail
# curl localhost:8080
# curl error
curl localhost:8080/cgi/env_cgi | grep '<tr><td>GATEWAY_INTERFACE</td><td>CGI/1.1</td></tr>'
curl localhost:8080/cgi/env_cgi.sh | grep "GATEWAY_INTERFACE='CGI/1.1"
