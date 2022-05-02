FROM ubuntu:latest

# Dependencies to compile webserv and install nginx
RUN apt update
RUN apt install -y zsh clang make git
RUN DEBIAN_FRONTEND="noninteractive" TZ="Europe/Paris" apt install -y nginx
# RUN apt install -y python3-pip && pip install compiledb
RUN apt install -y curl telnet
RUN apt install -y netcat
RUN apt install -y sudo
RUN apt install -y vim

RUN useradd -m dev && echo "dev:dev" | chpasswd && adduser dev sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER dev

# Webserv location
ENV HOME=/home/dev
ENV MOUNTING_POINT=${HOME}/webserv
RUN mkdir -p ${MOUNTING_POINT}
WORKDIR ${MOUNTING_POINT}
COPY zshrc ${HOME}/.zshrc

ENV DOCKER=true
COPY entrypoint.sh ${HOME}
ENTRYPOINT ["/home/dev/entrypoint.sh"]
# CMD ["trivial_server"]

# build: `docker built -t webserv .`
# run: `docker run -it -p 8080:80 -v $(pwd):/root/webserv webserv`
