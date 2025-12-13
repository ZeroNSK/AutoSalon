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
    git clone https://github.com/novnc/websockify /opt/websockify && \
    ln -s /opt/websockify /opt/novnc/utils/websockify

# Копирование исходного кода
WORKDIR /app
COPY . /app

# Сборка приложения
RUN make

# Создание конфигурации supervisor
RUN mkdir -p /var/log/supervisor

# Создание конфигурационного файла supervisord
RUN cat > /etc/supervisord.conf << 'EOF'
[supervisord]
nodaemon=true
logfile=/var/log/supervisor/supervisord.log
pidfile=/var/run/supervisord.pid

[program:xvfb]
command=/usr/bin/Xvfb :1 -screen 0 1024x768x24
autorestart=true
stdout_logfile=/var/log/supervisor/xvfb.log
stderr_logfile=/var/log/supervisor/xvfb_err.log

[program:fluxbox]
command=/usr/bin/fluxbox -display :1
autorestart=true
environment=DISPLAY=":1"
stdout_logfile=/var/log/supervisor/fluxbox.log
stderr_logfile=/var/log/supervisor/fluxbox_err.log

[program:x11vnc]
command=/usr/bin/x11vnc -display :1 -forever -shared -rfbport 5900 -nopw
autorestart=true
stdout_logfile=/var/log/supervisor/x11vnc.log
stderr_logfile=/var/log/supervisor/x11vnc_err.log

[program:novnc]
command=python3 /opt/websockify/websockify.py --web /opt/novnc 6080 localhost:5900
autorestart=true
stdout_logfile=/var/log/supervisor/novnc.log
stderr_logfile=/var/log/supervisor/novnc_err.log

[program:autosalon]
command=/app/AutoSalon
autorestart=true
environment=DISPLAY=":1"
stdout_logfile=/var/log/supervisor/autosalon.log
stderr_logfile=/var/log/supervisor/autosalon_err.log
EOF

ENV DISPLAY=:1

EXPOSE 5900 6080

CMD ["/usr/bin/supervisord", "-c", "/etc/supervisord.conf"]
