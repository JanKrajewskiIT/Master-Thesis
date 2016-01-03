#!/bin/bash

echo "Otwieranie portów..."
systemctl restart httpd
systemctl restart vsftpd
systemctl restart telnet.socket
/root/unrealircd/unrealircd start
service cups restart & echo -n
systemctl restart postfix

TCP_PORTS="1 7 20 37 42 43 49 53 70 79 109 110 119 139 143 161 179 389 444 445 458 563 1080 8080 8090"

for tcp_port in $TCP_PORTS
do 
    ncat -l -k -p $tcp_port -m 2 & echo -n
done

#yum install httpd vsftpd telnet telnet-server -y
#wget https://www.unrealircd.org/downloads/unrealircd-4.0.0-rc3.tar.gz
#tar coś tam, /Configure make make install i poprawienie skazanych błędów konfiguracyjnych

#otwieranie portów dla postgresql
#gedit /var/lib/pgsql/data/postgresql.conf
#listen_addresses = '*'
#systemctl restart postgresql

#Konfigracja CUPS (IPP)
#gedit /etc/cups/cpsd.conf
##Listen localhost:631
#Port 631
#Listen /var/run/cups/cups.sock

#yum install postfix
#inet_interfaces = all
##inet_interfaces = localhost
#gedit /etc/postfix/main.cf
