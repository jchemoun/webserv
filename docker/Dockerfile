FROM ubuntu:latest

# Dependencies to compile webserv and install nginx
RUN apt update
RUN apt install -y zsh clang make git
RUN DEBIAN_FRONTEND="noninteractive" TZ="Europe/Paris" apt install -y nginx
RUN apt install -y python3-pip && pip install compiledb
RUN apt install -y curl telnet
RUN apt install -y netcat
RUN apt install -y sudo
RUN apt install -y vim

RUN useradd -m dev && echo "dev:dev" | chpasswd && adduser dev sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER dev

# Webserv location
RUN mkdir -p /home/dev/webserv
WORKDIR /home/dev/webserv
COPY zshrc /home/dev/.zshrc

ENV DOCKER=true
COPY entrypoint.sh /home/dev
ENTRYPOINT ["/home/dev/entrypoint.sh"]
# CMD ["trivial_server"]

# build: `docker built -t webserv .`
# run: `docker run -it -p 8080:80 -v $(pwd):/root/webserv webserv`
