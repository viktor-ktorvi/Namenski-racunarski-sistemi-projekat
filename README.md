# Namenski-racunarski-sistemi-projekat

## Projekat br. 9 iz NSS

Napisati program kojim se vrši akvizicija signala sa jednog naponskog kanala. Na komandu
pritiska tastera startuje se akvizicija sa ucestanošcu od 10 sample/s, i sekvenca
dužina 100 odbiraka se smešta u ram memoriju kontrolera. U toku akvizicije ukljucena je
LE dioda. Po završetku akvizicije na komandu pritiska drugog tastera se na LCD displeju
istovremeno ispisuje minimalna, srednja i maksimalna vrednost uzorkovanog signala.

<br><br>

Projekat radi na simulatoru, debaunsiranje dugmadi je implementirano ali LCD trazi neko strogo postovanje vremenskih intervala 
tako da skoro sigurno ne bi radio uzivo, ali to se lako popravi sa par delay_cycles().
