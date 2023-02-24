Popescu Ioan Emanuel Theodor - 333CB

	Pentru implementarea proiectului am folosit in mare urmatorul approach: generez M + R
thread-uri (M-nr. mappere, R-nr. reducere), pe care le diferentiez doar dupa id:
	0->M-1: thread mapper; M->M+R-1: thread reducer;
	Functia care va fi rulata de fiecare thread verifica dupa id tipul thread-ului
si executa portiunea de cod corespunzatoare. Intre cele 2 blocuri de instructiuni 
corespunzatoare fiecarui tip de thread (primul pentru mapper, al doilea pentru reducer),
am pus o bariera, deoarece vreau ca reducerele sa inceapa sa execute doar dupa ce au
terminat toate mapperele.
	Distributia fisierelor pe mappere a fost facuta static, sortand crescator
fisierele dupa numarul de elemente din ele, apoi atribuind fiecarui mapper cate unul in
ordine pana la epuizare.
	M-am folosit de mai multe structuri de date, majoritatea fiind destul de usor de
inteles din comentariile din cod, dar am sa intru in detalii pentru structura argument,
care a servit la incapsularea mai multor structuri intr-o singura variabila. Existenta ei
nu a fost absolut necesara, dar simt ca ajuta la organizarea structurilor. Aceasta contine
o lista cu id-urile thread-urilor, o lista cu liste de fisiere de prelucrat pentru fiecare
mapper, un "vector de frecventa" de forma mapList[m][e][b]=1 doar daca mapperul m a 
descoperit ca un numar poate fi scris ca b^e, un vector de frecventa de tipul redList[e][b]
unde vor fi agregate listele partiale de catre mappere, un vector care contine numere prime
calculate prin ciur pana la sqrt(2^31).
	Ca si dimensiuni pentru aceste variabile globale am folosit:
->MAXMAPPERS si MAXREDUCERS: 32;
->MAXNUM: sqrt(2^31) - reprezinta baza maxima a unui numar care poate fi scris ca putere
perfecta pe int, pentru exponentul minim(2);
->MAXPRIMENUM: numarul maxim de numere prime care exista pana la MAXNUM, calculat dupa
formula: MAXNUM/log(MAXNUM); 
->MAXFACTORS: numarul maxim de factori care se pot afla in descompunerea unui numar pana
la MAXNUM;
->MAXFILES: numarul maxim de fisiere ce pot fi prelucrate, dat suficient de mare, cam din
burta :);
	Ca si algoritm pentru descompunerea in factori primi: pentru fiecare numar n, caut in
vectorul de numere prime divizori pana la radical din n, iar pentru fiecare divizor calculez
exponentul. Daca exista pentru n cel putin un factor cu exponentul 1, inseamna ca acesta nu
poate fi o putere perfecta(deci si daca numarul e prim). Pentru descompunerea lui n in factori
primi am sa calculez divizorii comuni ai exponentilor diferiti de 1, acestia reprezentand 
exponentii puterii perfecte reprezentate de n. 
