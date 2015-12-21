# Konfiguracja systemu

Do realizacji tematu wykorzystano dwie maszyny wirtualne posiadające różne systemy. Pierwsza zawierała system operacyjny Centos w wersji siódmej, na której zainstalowano takie narzędzia jak : Snort, Barnyard2, Postgresql itp. Na drugiej zainstalowano system Kali Linux posiadający zestaw narzędzi do przeprowadzania testów penetracyjnych serwera. 

## Instalacja pierwszej maszyny

Pierwsza maszyna wirtualna, stworzona w programie VirtualBox, zawiera system Centos 7 Linux ze środowiskiem gnome. Utworzono konto administratora z loginem "magisterka". Niestety pojawił się problem z logowaniem na wspomniane konto. Należało skonfigurować plik suoders.

```sh
$ su -
$ gedit /etc/suoders
```

Dodając poniższą zawartość :

```ah
## Allow root to run any commands anywhere
magisterka ALL=(ALL) ALL
```




