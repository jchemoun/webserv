# Webserv

This is a 42 project that implements a webserver.

## Workflow

To add a feature:
1. create a branch `gcb my_new_feature`
2. push the changes on the remote `gp -u origin my_new_feature`
3. when everything is fine and ready, create a merge request and assign partner as a reviewer
4. your partner will review and hopefull merge your PR shortly !

## Compile and run

### Standard compilation and run
```sh
make
./webserv <config_file>
```

### Run tests

TODO!

## Resources

### Minimal webserver examples

- [how to build a simple webserver](https://www.youtube.com/watch?v=esXw4bdaZkc) (Jacob Sorber, youtube)
- [webserver from scratch](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa) (medium)

### Authorized system calls

- [select, poll, epoll comparison](http://byteliu.com/2019/05/08/LINUX-%E2%80%93-IO-MULTIPLEXING-%E2%80%93-SELECT-VS-POLL-VS-EPOLL/) (Byteliu)

### More

- [Notion](https://webserv42.notion.site/webserv42/Webserv-cbb6ab4136ba4b4c8cb4f98109d5fc1f)
- [Ruslan's tutorial](https://ruslanspivak.com/lsbaws-part1/)
