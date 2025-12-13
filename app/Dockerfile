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
    py3-numpy \
    wget \
    unzip

# Установка noVNC и websockify вручную
RUN mkdir -p /opt && \
    cd /opt && \
    wget https://github.com/novnc/noVNC/archive/refs/heads/master.zip -O novnc.zip && \
    unzip novnc.zip && \
    mv noVNC-master novnc && \
    rm novnc.zip && \
    wget https://github.com/novnc/websockify/archive/refs/heads/main.zip -O websockify.zip && \
    unzip websockify.zip && \
    mv websockify-main websockify && \
    rm websockify.zip && \
    pip3 install /opt/websockify

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
