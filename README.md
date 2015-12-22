# Konfiguracja systemu

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

## Instalacja pierwszej maszyny

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


