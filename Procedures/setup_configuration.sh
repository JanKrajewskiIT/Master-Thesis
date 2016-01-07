#!/bin/bash

cp ../Configuration/snort.conf /etc/snort/snort.conf
cp ../Configuration/pg_hba.conf /var/lib/pgsql/data/pg_hba.conf
cp ../Configuration/barnyard2.conf /usr/local/etc/barnyard2.conf
cp ../Configuration/snort /etc/sysconfig/snort

cp "../Barnyard2 Plus/rpm/barnyard2" /etc/init.d/barnyard2
chmod +x /etc/init.d/barnyard2

cp "../Barnyard2 Plus/rpm/barnyard2.config" /etc/sysconfig/barnyard2
chkconfig --add barnyard2
chkconfig barnyard2 on
#chkconfig barnyard2 reset


