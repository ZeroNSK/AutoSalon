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
    python3 \
    py3-pip \
    supervisor \
    curl \
    bash \
    git

# Установка websockify через pip
RUN pip3 install websockify

# Установка noVNC
RUN cd /opt && \
    curl -L https://github.com/novnc/noVNC/archive/v1.3.0.tar.gz | tar xz && \
    mv noVNC-1.3.0 noVNC && \
    ln -s /opt/noVNC/vnc.html /opt/noVNC/index.html

# Копирование исходного кода
WORKDIR /app
COPY . /app

# Сборка приложения
RUN make

# Создание конфигурации supervisor
RUN mkdir -p /var/log/supervisor
COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf

# Создание скрипта запуска приложения
RUN echo '#!/bin/bash' > /app/start_app.sh && \
    echo 'export DISPLAY=:1' >> /app/start_app.sh && \
    echo 'sleep 5' >> /app/start_app.sh && \
    echo 'cd /app' >> /app/start_app.sh && \
    echo './AutoSalon' >> /app/start_app.sh && \
    chmod +x /app/start_app.sh

# Экспорт портов
EXPOSE 5900 6080

# Запуск через supervisor
CMD ["/usr/bin/supervisord", "-c", "/etc/supervisor/conf.d/supervisord.conf"]
