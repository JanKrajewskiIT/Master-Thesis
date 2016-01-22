# Konfiguracja systemu

- Wstęp
- Instalacja pierwszej maszyny
- Instalacja drugiej maszyny
- Instalacja i konfiguracja Snort'a
- Konfiguracja bazy danych dla barnyard2
- Konfiguracja programu barnyard2
- Konfiguracja Snort'a
- Firewall
- Inne
- TODO

## Wstęp

Do realizacji tematu wykorzystano dwie maszyny wirtualne posiadające różne systemy. Pierwsza zawierała system operacyjny Centos w wersji siódmej, na której zainstalowano takie narzędzia jak : Snort, Barnyard2, Postgresql itp. Na drugiej zainstalowano system Kali Linux posiadający zestaw narzędzi do przeprowadzania testów penetracyjnych serwera. 

## Instalacja pierwszej maszyny

Pierwsza maszyna wirtualna, stworzona w programie VirtualBox, zawiera system Centos 7 Linux ze środowiskiem gnome. Utworzono konto administratora z loginem "magisterka". Niestety pojawił się problem z logowaniem na wspomniane konto. Należało skonfigurować plik suoders.
```sh
$ su -
$ gedit /etc/suoders
```

Dodając poniższą zawartość :
```sh
## Allow root to run any commands anywhere
magisterka ALL=(ALL) ALL
```

Aby uprzyjemnić sobię pracę z systemem dodany został plugin VB guest addition.
```sh
$ rpm -Uvh http://dl.fedoraproject.org/pub/epel/7/x86_64/e/epel-release-7-5.noarch.rpm;
$ yum -y install dkms kernel-devel-$(uname -r) kernel-headers-$(uname -r);
$ wget http://download.virtualbox.org/virtualbox/5.0.0/VBoxGuestAdditions_5.0.0.iso;
$ mount -o loop VBoxGuestAdditions_5.0.0.iso /media/;
$ /media/VBoxLinuxAdditions.run;
```

W konfiguracji maszyny wirtualnej w VirtualBox'ie w zakładce 'Network' ustawiamy dwa adaptery. Pierwszy 'Host-only Adapter', a drugi 'NAT'. Dzięki tej operacji dostajemy dwa interfejsy sieciowe, a mianowicie : enp0s3 (internetowy) i enp0s8.

## Instalacja drugiej maszyny

Drugą maszynę tworzymy instalując system Kali Linux. Dodajemy narzędzie Guest Addition, a także ustawiamy interfejsy sieciowe, analogicznie jak w przypadku maszyny pierwszej.

## Instalacja i konfiguracja Snort'a

Na wstępie instalujemy pakiety req i snort, które wcześniej należy pobrać z oficjalnej strony www.snort.org. Operację należy wykonać w odpowiedniej kolejności
```sh
$ rpm -ihv daq-2.0.5-1.centos7.x86_64.rpm snort-2.9.7.3-1.centos7.x86_64.rpm
```

Sprawdzamy pliki konfiguracyjne :
```sh
$ rpm ­-ql snort|grep etc
```

Zarządzamy usługą snort poprzez operacje :
```sh
$ service snortd start|stop|reload|status
```

Przy pierwszej próbie uruchomienia snorta, napotkamy problemy. Należy go dokonfigurować. Samą przyczynę wystąpienia błędu można sprawdzić poprzez :
```sh
$ service snortd status
$ grep snort.conf /var/log/messages
$ snort -c /etc/snort/snort.conf -T
```

Należy teraz zdefiniować zespół reguł. Można je również pobrać z oficjalnej strony snorta.
```sh
$ tar -xf snortrules-snapshot-2973.tar.gz
$ cp rules/* /etc/snort/rules

$ mkdir /etc/snort/rules/preproc_rules
$ cp preproc_rules/* /etc/snort/rules/preproc_rules

$ mkdir /etc/snort/rules/so_rules
$ cp so_rules/* /etc/snort/rules/so_rules - tylko pliki .rules

$ cp etc/sid-msg.map /etc/snort
```

Następnie dokonujemy zmian konfiguracyjnych w pliku snort.conf.
```sh
$ gedit /etc/snort/snort.conf
```
Podmieniająć :
```sh
# dynamicdetection directory /usr/local/lib/snort_dynamicrules

var RULE_PATH /etc/snort/rules
var SO_RULE_PATH /etc/snort/rules/so_rules
var PREPROC_RULE_PATH /etc/snort/rules/preproc_rules

include $PREPROC_RULE_PATH/preprocessor.rules
include $PREPROC_RULE_PATH/decoder.rules
include $PREPROC_RULE_PATH/sensitive­data.rules

# Reputation preprocessor. For more information see README.reputation
# preprocessor reputation: \
#   memcap 500, \
#   priority whitelist, \
#   nested_ip inner, \
#   whitelist $WHITE_LIST_PATH/white_list.rules, \
#   blacklist $BLACK_LIST_PATH/black_list.rules 
```

Snorta w trybie daemona uruchamiamy poprzez :
```sh
snort -d -D -i enp0s3 -u snort -g snort -c /etc/snort/snort.conf -l /var/log/snort
```

## Konfiguracja bazy danych dla barnyard2

Najpierw należy zainstalować postgresql
```sh
$ yum install postgresql postgresql-devel postgresql-server postgresql-contrib postgresql-libs postgresql-client
$ systemctl enable postgresql
$ postgresql-setup initdb
$ systemctl status postgresql.service
$ systemctl start postgresql
```

Aby móc uruchomić program barnyard2 należy wcześniej skonfigurować środowisko bazy danych
```sh
$ sudo su
$ su - postgres
$ psql

> create database snort_logs;
> create user snort_user PASSWORD 'magisterka';
> grant ALL ON DATABASE snort_logs to snort_user;
> systemctl reload postgresql
```

Kolejnym krokiem jest przygotowanie odpowiedniej konfiguracji w pliku pg_hba.conf
```sh
#host    all             all             127.0.0.1/32            ident
host    snort_logs      snort_user   	 127.0.0.1/32    	 password
```

Na koncu należy zalogować się do przygotowanej bazy danych i wrzucić schemat barnyard2.
```sh
$ systemctl reload postgresql
$ psql -U snort_user -h 127.0.0.1 snort_logs

> \i /home/magisterka/Pulpit/Magisterka/pluginy/barnyard2-stable/schemas/create_postgresql
```

## Konfiguracja programu barnyard2

Pobieramy źródła z repozytorium https://github.com/firnsy/barnyard2/tree/master. Rozpakowujemy barnyard2 i przechodzimy do katalogu barnyard2-master, gdzie wywołujemy :
```sh
$ yum install libtool libpcap-devel libdnet-devel
$ ./autogen.sh
$ ./configure --with-postgresql
$ make
$ make install
```

Program barnyard2 został zainstalowany w katalogu /usr/local/bin/

Do pliku konfiguracyjnego usr/local/etc/barnyard2.conf dodajemy :
```sh
$ gedit /usr/local/etc/barnyard2.conf

output database: alert, postgresql, host=127.0.0.1 port=5432 user=snort_user password=magisterka dbname=snort_logs sensor_name=snort1
#output database: log, postgresql, host=localhost port=5432 user=snort_user password=magisterka dbname=snort_logs

#config logdir: /tmp
config logdir: /var/log/snort
#config interface:  eth0
config interface: enp0s3
config waldo_file: /tmp/waldo
config process_new_records_only

output alert_fast: stdout
output firewalld_lock_plugin: stdout
output iptables_lock_plugin: stdout
output nftables_lock_plugin: stdout
```

Bez zmiany natomiast pozostawiamy konfigurację domyślną dotyczącą położenia plików konfiguracyjnych snorta.

```sh
config reference_file:      /etc/snort/reference.config
config classification_file: /etc/snort/classification.config
config gen_file:            /etc/snort/gen-msg.map
config sid_file:            /etc/snort/sid-msg.map

input unified2
```
Następnym krokiem będzie przygotowanie skryptu startowego.
```sh
$ cp rpm/barnyard2 /etc/init.d/barnyard2
$ cp rpm/barnyard2.config /etc/sysconfig/barnyard2
$ chmod +x /etc/init.d/barnyard2
$ chkconfig --add barnyard2
$ chkconfig barnyard2 on
```

Ważne jest w tym przypadku przygotowanie dobrej konfiguracji dla skryptu startowego
```sh
$ gedit /etc/sysconfig/barnyard2

LOG_FILE="merged.log"
SNORTDIR="/var/log/snort"
INTERFACES="enp0s3"
CONF=/etc/snort/barnyard2.conf
EXTRA_ARGS=""
```

Do tego wszystkiego należy stworzyć odpowiednie powiązania, a także utworzyć katalog archiwum.
```sh
$ ln -s /usr/local/etc/barnyard2.conf /etc/snort/barnyard2.conf
$ ln -s /usr/local/bin/barnyard2 /usr/bin
$ mkdir /var/log/snort/enp0s3/archive
```

DOPRACOWAĆ TE SKRYPTY STARTOWE I SERWISY DLA SNORT I BARNYARD2

Uruchamiamy program
```sh
#Tryb wsadowy
$ /usr/local/bin/barnyard2 -c /usr/local/etc/barnyard2.conf -o /var/log/snort/snort.log.1438440068

#Tryb daemona
$ /usr/local/bin/barnyard2 -v -c /usr/local/etc/barnyard2.conf -d /var/log/snort/ -f merged.log

```

Dokonfigurowanie barnyard2 cd...

## Konfiguracja Snort'a

Należy dokonać edycji plików konfiguracyjnych tak by Snort współgrał z systemem i programem barnyard2 :
```sh
$ gedit /etc/sysconfig/snort

INTERFACE=enp0s3
# ALERTMODE=fast
BINARY_LOG=0
```

```sh
$ gedit /etc/snort/snort.conf 

output unified2: filename merged.log, limit 128
```

Można uruchomić Snort'a jako sieciowego IDS'a można poprzez :
```sh
$ snort -c /etc/snort/snort.conf
$ snort -A fast -b -d -D -i enp0s3 -u snort -g snort -c /etc/snort/snort.conf -l /var/log/snort
$ ps aux|grep snort
```

Ostatecznie jednak uruchamiamy Snort'a bez opcji -b i -A, oraz sprawdzamy zawartość wygenerowanych log`ów :
```sh
$ snort -d -D -i enp0s3 -u snort -g snort -c /etc/snort/snort.conf -l /var/log/snort
$ file /var/log/snort/snort.log.1438440068
$ u2spewfoo /var/log/snort/snort.log.1438440068 |head
```

## Firewall
Należy doinstalować service do iptables
```sh
$ yum install iptables-services
```

Firewalld można zarządzać poprzez szynę D-BUS między innymi komendami :
```sh
$ firewall-cmd --get-active-zones
$ firewall-cmd --zone=public --add-port=2888/tcp --permanent
$ firewall-cmd --reload
$ firewall-cmd --zone=block --list-ports
```

## Inne 

Sprawdzić istniejące procesy można poprzez.
```sh
$ ps aux | less
$ ps aux | grep barnyard2
```

Lista wszystkich zdefiniowanych interfacow jest w pliku /proc/net/dev.
Katalog poleceń znajduje się w /usr/sbin/.

Przy wcześniejszym uruchomieniu usługi snort dobrze zrobić porządki.
Zarządzamy usługą snort poprzez operacje :
```sh
$ rm /var/log/snort/*
$ service snortd start|stop|reload|status
$ service postgresql start|stop|reload|status
```

Metody pomocnicze
```sh
$ nmap -sT -O localhost
$ sudo lsof -i
$ sudo netstat -lptu
$ sudo netstat -tulpn
```

## Konfiguracja Nftables
Bibliotekę libmnl w wersji 1.0.3 należy pobrać z oficjalnej strony http://www.netfilter.org
```sh
git clone git://git.netfilter.org/libmnl
cd libmnl
./autogen.sh
./configure
make
make install
```

```sh
export LIBMNL_LIBS=/usr/local/lib/libmnl.so
export LIBMNL_CFLAGS=/usr/include
```

```sh
git clone git://git.netfilter.org/libnftnl
cd libnftnl
./autogen.sh
./configure
make
make install
```

```sh
export LIBNFTNL_LIBS=/usr/local/lib/libnftnl.so
export LIBNFTNL_CFLAGS=/usr/include
```

```sh
git clone git://git.netfilter.org/nftables
./autogen.sh
./configure --with-mini-gmp --without-cli
make
make install
```

