#!/bin/bash

#Port Number Description
#1 	TCP Port Service Multiplexer (TCPMUX)
#5 	Remote Job Entry (RJE)
#7 	ECHO
#18 	Message Send Protocol (MSP)
#20 	FTP -- Data
#21 	FTP -- Control
#22 	SSH Remote Login Protocol
#23 	Telnet
#25 	Simple Mail Transfer Protocol (SMTP)
#29 	MSG ICP
#37 	Time
#42 	Host Name Server (Nameserv)
#43 	WhoIs
#49 	Login Host Protocol (Login)
#53 	Domain Name System (DNS)
#69 	Trivial File Transfer Protocol (TFTP)
#70 	Gopher Services
#79 	Finger
#80 	HTTP
#103 	X.400 Standard
#108 	SNA Gateway Access Server
#109 	POP2
#110 	POP3
#115 	Simple File Transfer Protocol (SFTP)
#118 	SQL Services
#119 	Newsgroup (NNTP)
#137 	NetBIOS Name Service
#139 	NetBIOS Datagram Service
#143 	Interim Mail Access Protocol (IMAP)
#150 	NetBIOS Session Service
#156 	SQL Server
#161 	SNMP
#179 	Border Gateway Protocol (BGP)
#190 	Gateway Access Control Protocol (GACP)
#194 	Internet Relay Chat (IRC)
#197 	Directory Location Service (DLS)
#389 	Lightweight Directory Access Protocol (LDAP)
#396 	Novell Netware over IP
#443 	HTTPS
#444 	Simple Network Paging Protocol (SNPP)
#445 	Microsoft-DS
#458 	Apple QuickTime
#546 	DHCP Client
#547 	DHCP Server
#563 	SNEWS
#569 	MSN
#1080 	Socks

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
