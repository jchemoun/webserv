FROM ubuntu:latest

# Dependencies to compile webserv and install nginx
RUN apt update
RUN apt install -y --no-install-recommends zsh clang make git
RUN DEBIAN_FRONTEND="noninteractive" TZ="Europe/Paris" apt install -y --no-install-recommends nginx
# RUN apt install -y python3-pip && pip install compiledb
#NOTE:	we might need to install curl with suggested dependencies (like ca-certificates)
# 			but maybe not (since we don't have to use those with webserv)
RUN apt install -y --no-install-recommends curl telnet
RUN apt install -y --no-install-recommends netcat
RUN apt install -y --no-install-recommends sudo
RUN apt install -y --no-install-recommends vim
RUN apt install -y --no-install-recommends valgrind
RUN apt install -y --no-install-recommends siege

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

# ENV HOST_USER_ID=1000
# ENV HOST_USER_GROUP=${HOST_USER_ID}

ENTRYPOINT ["/home/dev/entrypoint.sh"]
# CMD ["trivial_server"]

# build: `docker built -t webserv .`
# run: `docker run -it -p 8080:80 -v $(pwd):/root/webserv webserv`
