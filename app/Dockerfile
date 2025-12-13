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
    supervisor

# Копирование исходного кода
WORKDIR /app
COPY . /app

# Сборка приложения
RUN mkdir -p /app/build && \
    cd /app/build && \
    cmake -DCMAKE_MAKE_PROGRAM=/usr/bin/make .. && \
    /usr/bin/make

# Создание конфигурации supervisor
RUN mkdir -p /var/log/supervisor && \
    echo '[supervisord]\n\
nodaemon=true\n\
logfile=/var/log/supervisor/supervisord.log\n\
pidfile=/var/run/supervisord.pid\n\
\n\
[program:xvfb]\n\
command=/usr/bin/Xvfb :1 -screen 0 1024x768x24\n\
autorestart=true\n\
stdout_logfile=/var/log/supervisor/xvfb.log\n\
stderr_logfile=/var/log/supervisor/xvfb_err.log\n\
\n\
[program:fluxbox]\n\
command=/usr/bin/fluxbox -display :1\n\
autorestart=true\n\
environment=DISPLAY=":1"\n\
stdout_logfile=/var/log/supervisor/fluxbox.log\n\
stderr_logfile=/var/log/supervisor/fluxbox_err.log\n\
\n\
[program:x11vnc]\n\
command=/usr/bin/x11vnc -display :1 -forever -shared -rfbport 5900 -nopw\n\
autorestart=true\n\
stdout_logfile=/var/log/supervisor/x11vnc.log\n\
stderr_logfile=/var/log/supervisor/x11vnc_err.log\n\
\n\
[program:autosalon]\n\
command=/app/build/AutoSalon\n\
autorestart=true\n\
environment=DISPLAY=":1"\n\
stdout_logfile=/var/log/supervisor/autosalon.log\n\
stderr_logfile=/var/log/supervisor/autosalon_err.log\n\
' > /etc/supervisord.conf

ENV DISPLAY=:1

EXPOSE 5900

CMD ["/usr/bin/supervisord", "-c", "/etc/supervisord.conf"]
