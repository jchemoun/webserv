# Webserv

This is a 42 project that implements a webserver.

## Workflow

To add a feature:
1. create a branch `gcb my_new_feature`
2. push the changes on the remote `gp -u origin my_new_feature`
3. when everything is fine and ready, create a merge request and assign partner as a reviewer
4. your partner will review and hopefully merge your PR shortly !

## Compile and run

### Standard compilation and run
```sh
make
./webserv <config_file>
```

### Compile and run in docker

This repo ships a `Dockerfile` to provide a build, run and testing environnement. The underlying image is `ubuntu:latest`.

#### Build the image
```sh
make build_image
```

#### Compile, run webserv or nginx in a container

| Command (host side)           | Effect                                                                                       |
|-------------------------------|----------------------------------------------------------------------------------------------|
| `make compile`                | run the docker image, compile webserv in the container, quit                                 |
| `make run CONF=<confpath>`    | compile and run `./webserv confpath` (interactive)                                           |
| `make run_fg CONF=<confpath>` | compile and run `./webserv confpath` in the foreground                                       |
| `make attach`                 | open a zsh session on the current container lauched with make run, make run_fg or make nginx |
| `make nginx CONF=<confpath>`  | load `confpath` in nginx enabled sites, and run nginx (interactive)                          |
| `make run_image`              | run interactively the image (does not compile or run anything)                               |


⚠️ **Beware**: The commands above mount your current working directory on `/home/dev/webserv` in **read-write** mode. So be careful.

### Run tests

| Command (host side)         | Effect                                                                                 |
|-----------------------------|----------------------------------------------------------------------------------------|
| `make test CONF=<confpath>` | load `confpath` in nginx/webserv, run bash script associated, compare the output |
| `make test`                 | run tests for all configuration files, unless an error occurs                          |

⚠️ **Beware**: The commands above mount your current working directory on `/home/dev/webserv` in **read-write** mode. So be careful.

## Resources

### Minimal webserver examples

- [how to build a simple webserver](https://www.youtube.com/watch?v=esXw4bdaZkc) (Jacob Sorber, youtube)
- [webserver from scratch](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa) (medium)

### Authorized system calls

- [select, poll, epoll comparison](http://byteliu.com/2019/05/08/LINUX-%E2%80%93-IO-MULTIPLEXING-%E2%80%93-SELECT-VS-POLL-VS-EPOLL/) (Byteliu)

### Nginx doc

- [index of all directives](http://nginx.org/en/docs/dirindex.html)
- [how is a request processed (listen, address, server_name, host...)](http://nginx.org/en/docs/http/request_processing.html)

### CGI

 - [htmlhelp](https://www.htmlhelp.com/faq/cgifaq.2.html)
 - [tutorialspoint](https://www.tutorialspoint.com/cplusplus/cpp_web_programming.htm)
 - [wikipedia-en](https://en.wikipedia.org/wiki/Common_Gateway_Interface)
 - [wikipedia-fr: cgi variables](https://fr.wikipedia.org/wiki/Variables_d'environnement_CGI)
 - [IBM](https://www.ibm.com/docs/pt-br/netcoolomnibus/8.1?topic=SSSHTQ_8.1.0/com.ibm.netcool_OMNIbus.doc_8.1.0/webtop/wip/reference/web_cust_envvariablesincgiscripts.html)
 - [condor.depaul.edu](https://condor.depaul.edu/elliott/shared/draft-cgi.html#4.14)


### More

- [Notion](https://webserv42.notion.site/webserv42/Webserv-cbb6ab4136ba4b4c8cb4f98109d5fc1f)
- [Ruslan's tutorial](https://ruslanspivak.com/lsbaws-part1/)
