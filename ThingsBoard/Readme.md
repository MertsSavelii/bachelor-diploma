# ThingsBoard
# Установка на Ubuntu Server

[Оригинал гайда](https://thingsboard.io/docs/user-guide/install/ubuntu/?ubuntuThingsboardDatabase=postgresql)

## Требования

В этом руководстве описывается, как установить ThingsBoard на Ubuntu 20.04 LTS / 22.04 LTS. Требования к оборудованию зависят от выбранной базы данных и количества устройств, подключенных к системе. Для запуска ThingsBoard и PostgreSQL на одной машине потребуется не менее 4 ГБ оперативной памяти.

## Шаг 1. Установка Java 11 (OpenJDK)

Сервис ThingsBoard работает на Java 11. Следуйте этим инструкциям для установки OpenJDK 11:
```sh
sudo apt update
sudo apt install openjdk-11-jdk
```
Пожалуйста, не забудьте настроить свою операционную систему на использование OpenJDK 11 по умолчанию. Вы можете настроить, какая версия будет использоваться по умолчанию, с помощью следующей команды:
```sh
sudo update-alternatives --config java
```
Проверить установку можно с помощью следующей команды:
```sh
java -version
```
Ожидаемый вывод команды:
```sh
openjdk version "11.0.xx"
OpenJDK Runtime Environment (...)
OpenJDK 64-Bit Server VM (build ...)
```

## Шаг 2. Установка сервиса ThingsBoard

Скачать установочный пакет.
```sh
wget https://github.com/thingsboard/thingsboard/releases/download/v3.6.4/thingsboard-3.6.4.deb
```
Установка ThingsBoard как сервиса
```sh
sudo dpkg -i thingsboard-3.6.4.deb
```
## Шаг 3. Настройка базы данных ThingsBoard
ThingsBoard может использовать SQL или гибридный подход к базам данных. Более подробную информацию см. [на соответствующей странице](https://thingsboard.io/docs/reference/#sql-vs-nosql-vs-hybrid-database-approach) архитектуры.
### Установка PostgreSQL
Инструкции, приведенные ниже, помогут вам установить PostgreSQL.
```sh
# install **wget** if not already installed:
sudo apt install -y wget

# import the repository signing key:
wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | sudo apt-key add -

# add repository contents to your system:
echo "deb https://apt.postgresql.org/pub/repos/apt/ $(lsb_release -cs)-pgdg main" | sudo tee  /etc/apt/sources.list.d/pgdg.list

# install and launch the postgresql service:
sudo apt update
sudo apt -y install postgresql-15
sudo service postgresql start
```
После установки PostgreSQL вы можете создать нового пользователя или установить пароль для основного пользователя. Приведенная ниже инструкция поможет установить пароль для основного пользователя postgresql
```sh
sudo su - postgres
psql
\password
\q
```
Затем нажмите "Ctrl+D", чтобы вернуться в главную консоль пользователя и подключиться к базе данных, чтобы создать БД thingsboard:
```sh
psql -U postgres -d postgres -h 127.0.0.1 -W
CREATE DATABASE thingsboard;
\q
```
### Конфигурация ThingsBoard
Редактирование конфигурационного файла ThingsBoard
```sh
sudo nano /etc/thingsboard/conf/thingsboard.conf
```
Добавьте следующие строки в конфигурационный файл. Не забудьте заменить "PUT_YOUR_POSTGRESQL_PASSWORD_HERE" на свой реальный пароль пользователя postgres:
```sh
# DB Configuration 
export DATABASE_TS_TYPE=sql
export SPRING_DATASOURCE_URL=jdbc:postgresql://localhost:5432/thingsboard
export SPRING_DATASOURCE_USERNAME=postgres
export SPRING_DATASOURCE_PASSWORD=PUT_YOUR_POSTGRESQL_PASSWORD_HERE
# Specify partitioning size for timestamp key-value storage. Allowed values: DAYS, MONTHS, YEARS, INDEFINITE.
export SQL_POSTGRES_TS_KV_PARTITIONING=MONTHS
```
## Шаг 4. Выберите сервис очередей ThingsBoard
По умолчанию используется встроенный однако поддерживаются и другие
## Шаг 5. [Дополнительный] Обновление памяти для медленных компьютеров (4 ГБ ОЗУ)
Редактирование конфигурационного файла ThingsBoard
```sh
sudo nano /etc/thingsboard/conf/thingsboard.conf
```
Добавьте следующие строки в конфигурационный файл.
```sh
# Update ThingsBoard memory usage and restrict it to 2G in /etc/thingsboard/conf/thingsboard.conf
export JAVA_OPTS="$JAVA_OPTS -Xms2G -Xmx2G"
```
## Шаг 6. Запустите сценарий установки
После установки сервиса ThingsBoard и обновления конфигурации БД можно выполнить следующий сценарий:
```sh
# --loadDemo option will load demo data: users, devices, assets, rules, widgets.
sudo /usr/share/thingsboard/bin/install/install.sh --loadDemo
```
## Шаг 7. Запустить сервис ThingsBoard
Выполните следующую команду, чтобы запустить ThingsBoard:
```sh
sudo service thingsboard start
```
После запуска вы сможете открыть веб-интерфейс по следующей ссылке:
```sh
http://localhost:8080/
```
Следующие учетные данные по умолчанию доступны, если вы указали –loadDemo во время выполнения сценария установки:
* *Системный администратор*: sysadmin@thingsboard.org / sysadmin
* *Администратор арендатора*: tenant@thingsboard.org / tenant
* *Customer User*: customer@thingsboard.org / customer

Вы всегда можете изменить пароли для каждой учетной записи на странице профиля учетной записи.
### Устранение неполадок
Журналы ThingsBoard хранятся в следующем каталоге:
```sh
/var/log/thingsboard
```
Вы можете выполнить следующую команду, чтобы проверить, есть ли какие-либо ошибки на стороне бэкенда:
```sh
cat /var/log/thingsboard/thingsboard.log | grep ERROR
```
