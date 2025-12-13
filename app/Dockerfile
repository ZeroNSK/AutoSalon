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
    git \
    python3 \
    py3-pip \
    nodejs \
    npm

# Установка noVNC для веб-доступа
RUN git clone https://github.com/novnc/noVNC.git /opt/novnc && \
    git clone https://github.com/novnc/websockify /opt/websockify

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
