FROM alpine:3.18

# Установка зависимостей для Qt и PostgreSQL
RUN apk add --no-cache \
    build-base \
    cmake \
    make \
    qt5-qtbase-dev \
    qt5-qtbase-postgresql \
    postgresql-client

# Копирование исходного кода
WORKDIR /app
COPY . /app

# Сборка приложения
RUN make

# Запуск приложения
CMD ["/app/AutoSalon"]
