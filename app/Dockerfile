FROM alpine:3.18

# Установка зависимостей для Qt, PostgreSQL и VNC
RUN apk add --no-cache \
    build-base \
    cmake \
    make \
    qt5-qtbase-dev \
    qt5-qtbase-postgresql \
    postgresql-client \
    xvfb \
    x11vnc \
    fluxbox \
    xterm \
    supervisor \
    python3 \
    py3-pip \
    py3-setuptools \
    py3-wheel \
    git \
    bash

# Простая установка websockify через pip
RUN pip3 install websockify

# Скачиваем noVNC
RUN git clone --depth 1 https://github.com/novnc/noVNC.git /opt/novnc || \
    (mkdir -p /opt/novnc && \
     cd /opt/novnc && \
     wget -O- https://github.com/novnc/noVNC/tarball/master | tar xz --strip-components=1)

# Копирование исходного кода
WORKDIR /app
COPY . /app

# Сборка приложения
RUN make

# Создание конфигурации supervisor
RUN mkdir -p /var/log/supervisor

# Копирование конфигурационного файла supervisord
COPY supervisord.conf /etc/supervisord.conf

ENV DISPLAY=:1

EXPOSE 5900 6080

CMD ["/usr/bin/supervisord", "-c", "/etc/supervisord.conf"]
