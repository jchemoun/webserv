FROM ubuntu:latest

# Dependencies to compile webserv and install nginx
RUN apt update
RUN apt install -y zsh clang make git
RUN DEBIAN_FRONTEND="noninteractive" TZ="Europe/Paris" apt install -y nginx
RUN apt install -y python3-pip && pip install compiledb
RUN apt install -y curl telnet
RUN apt install -y netcat

# Webserv location
RUN mkdir /root/webserv
WORKDIR /root/webserv

ENV DOCKER=true
COPY entrypoint.sh /root/
ENTRYPOINT ["/root/entrypoint.sh"]
# CMD ["trivial_server"]

# build: `docker built -t webserv .`
# run: `docker run -it -p 8080:80 -v $(pwd):/root/webserv webserv`
