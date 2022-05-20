set -xeo pipefail
# curl localhost:8080
# curl error
# Check body
curl localhost:8080/cgi-bin/env_cpp.cgi | grep '<tr><td>GATEWAY_INTERFACE</td><td>CGI/1.1</td></tr>'
curl localhost:8080/cgi-bin/env_cpp.cgi/hello?world | grep '<tr><td>QUERY_STRING</td><td>world</td></tr>'
curl localhost:8080/cgi-bin/env.sh.cgi | grep "GATEWAY_INTERFACE='CGI/1.1"
curl localhost:8080/cgi-bin/env.sh.cgi | grep "HTTP_USER_AGENT='curl" # check that client request's variable are correctly passed on

# Check header
curl -i localhost:8080/cgi-bin/env.sh.cgi | grep 'Content-Type: text/plain' # should be there (webserv header format)
curl -i localhost:8080/cgi-bin/env_cpp.cgi | grep 'Content-Type: text/html' # should be there (webserv header format)
! curl -i localhost:8080/cgi-bin/env_cpp.cgi | grep 'Content-Type:text/html' # should not (cgi format)
curl -i -X GET http://localhost:8080/cgi-bin/weird_status.sh.cgi -d "hello" | grep 'HTTP/1.1 444 Some weird status' # check the code and message are retrieved from cgi
! curl -i -X GET http://localhost:8080/cgi-bin/weird_status.sh.cgi -d "hello" | grep 'Status' # check the Status Header is removed

# Check errors
curl -i localhost:8080/cgi-bin/fail.sh.cgi | grep 'HTTP/1.1 500' # cgi script exists and is executable but fails
curl -i localhost:8080/cgi-bin/fail.sh.cgi | grep '<title>500' # idem but body check
curl -i localhost:8080/cgi-bin/fdsagsafdgf.cgi | grep 'HTTP/1.1 404' # cgi script exists and is executable but fails
curl -i localhost:8080/cgi-bin/not_executable_file.cgi | grep 'HTTP/1.1 403' # cgi script exists and is executable but fails

# 42 tester
# if [ ! -f html/cgi-bin/ubuntu_cgi_tester.cgi ]; then
#   curl -o html/cgi-bin/ubuntu_cgi_tester.cgi https://projects.intra.42.fr/uploads/document/document/8373/ubuntu_cgi_tester # does not work...
#   sudo chmod +x html/cgi-bin/ubuntu_cgi_tester.cgi
# fi
curl -i -X GET localhost:8080/cgi-bin/ubuntu_cgi_tester.cgi | grep 200 # empty but good
curl -i -X GET localhost:8080/cgi-bin/ubuntu_cgi_tester.cgi | grep charset # charset=utf-8 in header
curl -X GET localhost:8080/cgi-bin/ubuntu_cgi_tester.cgi -d "hello" | grep 'HELLO'
