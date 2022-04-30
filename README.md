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

#### Available commands

| Command (host side)          | Effect                                                                        |
|------------------------------|-------------------------------------------------------------------------------|
| `make compile`               | run the docker image, compile webserv in the container, quit                  |
| `make cc`                    | like above but also generate `compile_commands.json`                          |
| `make run_image`             | run interactively the image (does not compile)                                |
| `make nginx CONF=<confname>` | load `conf/confname.conf` in nginx enabled sites, and run nginx (interactive) |


⚠️ **Beware**: The commands above mount your current working directory on `/home/dev/webserv` in **read-write** mode. So be careful.

### Run tests

| Command (host side)         | Effect                                                                                 |
|-----------------------------|----------------------------------------------------------------------------------------|
| `make test CONF=<confname>` | load `conf/confname.conf` in nginx/webserv, run `conf/confname.sh`, compare the output |
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

### More

- [Notion](https://webserv42.notion.site/webserv42/Webserv-cbb6ab4136ba4b4c8cb4f98109d5fc1f)
- [Ruslan's tutorial](https://ruslanspivak.com/lsbaws-part1/)
