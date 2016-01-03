#!/bin/bash

./ncat_open_ports.sh

TCP_PORTS="1 7 20 37 42 43 49 53 70 79 109 110 119 139 143 161 179 389 444 445 458 563 1080 8080 8090"

UDP_PORTS="1 7 20 37 42 43 49 53 70 79 109 110 119 139 143 161 179 389 444 445 458 563 1080 8080 8090"

echo "Wyłączanie wszystkich service'ów..."
systemctl stop postgresql
systemctl stop snortd
systemctl stop firewalld
systemctl stop iptables
#systemctl stop barnyard2 - trzeba dorzucić

echo "Przywracanie ustawień domyślnych firewalld..."
systemctl start firewalld

for zone in block public 
do 
   for port in $(firewall-cmd --zone=$zone --list-ports) 
   do 
      echo -n "Usuwanie [Zone : " $zone ", Port : " $port "] - "
      firewall-cmd --zone=$zone --remove-port=$port --permanent
   done
done

for tcp_port in $TCP_PORTS
do 
   echo -n "Dodawanie [Zone : " public ", Port : " $tcp_port/tcp "] - "
   firewall-cmd --zone=public --add-port=$tcp_port/tcp --permanent
done

for udp_port in $UDP_PORTS
do 
   echo -n "Dodawanie [Zone : " public ", Port : " $udp_port/udp "] - "
   firewall-cmd --zone=public --add-port=$udp_port/udp --permanent
done

echo -n "Zatwierdzanie ustawień firewalld - " && firewall-cmd --reload
systemctl stop firewalld

echo "Przywracanie ustawień domyślnych iptables..."
#systemctl start iptables
#systemctl stop iptables

echo "Przywracanie ustawień domyślnych nftables..."
#systemctl start nftables
#systemctl stop nftables

echo "Uruchomienie daemon'a snort'a..."
rm /var/log/snort/*
systemctl restart snortd
#systemctl start snortd
#systemctl reload snortd
#snort -d -D -i enp0s3 -u snort -g snort -c /etc/snort/snort.conf -l /var/log/snort

echo "Uruchomienie systemu postgresql..."
systemctl start postgresql
systemctl reload postgresql

echo -n "Uruchomienie daemon'a barnyard'a. "
#barnyard2 -v -c /usr/local/etc/barnyard2.conf -d /var/log/snort/ -f merged.log
