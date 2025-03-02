# Graph-structure-and-algorithms

---

## Descrierea Proiectului

Acest proiect oferă o implementare detaliată a unor algoritmi fundamentali pentru grafuri, având ca scop demonstrerea unei înțelegeri profunde a structurilor de date, algoritmilor și tehnicilor moderne de programare în C++. Este un proiect realizat în cadrul cursului de Algoritmi Fundamentali, care reflectă cunoștințele și abilitățile dobândite în programare și rezolvarea problemelor complexe.

### Funcționalități

Proiectul include o implementare completă a claselor și a funcțiilor necesare pentru manipularea și analiza grafurilor complexe, abordând o gamă largă de algoritmi și probleme clasice:

- **Citirea și afișarea grafurilor**: Permite citirea grafurilor din fișiere și reprezentarea lor folosind lista de adiacență, matrice de adiacență sau lista de muchii.
- **Parcurgerea grafurilor**: Implementări pentru traversarea grafurilor utilizând BFS și DFS atât într-un mod iterativ, cât și recursiv.
- **Algoritmi pentru găsirea drumurilor minime**:
  - **Dijkstra** (pentru grafuri ponderate)
  - **Bellman-Ford** (pentru grafuri cu ponderi pozitive, negative sau nule)
  - **Floyd-Warshall** (pentru determinarea drumurilor minime pentru toate perechile de noduri)
- **Algoritmi pentru arborii de cost minim**:
  - **Kruskal**
  - **Prim**
- **Algoritmi pentru componente conexe și tare conexe**:
  - **Flood Fill**
  - **Tarjan**
  - **Kosaraju**
- **Sortare topologică** (pentru grafuri aciclice orientate)
- **Algoritmi pentru grafuri bipartite și cuplaje maxime**:
  - Verificarea unui graf bipartit
  - **Algoritmul Hopcroft-Karp**
- **Flux maxim și flux de cost minim**:
  - **Algoritmul Edmonds-Karp**
  - Flux maxim cu cost minim
- **Determinarea drumului critic într-un graf ponderat orientat** (Critical Path Method - CPM)
- **Determinarea punctelor și muchiilor critice într-un graf neorientat**
- **Probleme speciale**:
  - **Havel-Hakimi** pentru construirea unui graf neorientat dintr-o secvență de grade
  - Verificarea și construcția grafurilor **hamiltoniene** și **euleriene**
  - Algoritm pentru colorarea grafurilor cu 6 culori
- **Alți algoritmi**:
  - **Clustering** bazat pe algoritmul lui Kruskal
  - Funcții pentru calcularea distanței Levenshtein între șiruri de caractere

### Tehnologii și Concepte Utilizate

- **Programare Orientată pe Obiecte (OOP)**: Clasa principală `Graph` joacă rolul central în implementarea acestui proiect, folosind principiile OOP pentru a organiza și structura codul.
- **Funcții recursive și iterative**: Algoritmii de parcurgere sunt implementați atât recursiv, cât și iterativ, demonstrând flexibilitate în abordarea problemelor.
- **Algoritmi Greedy și Programare Dinamică**: Algoritmi precum Dijkstra, Bellman-Ford, Prim și Floyd-Warshall sunt implementați folosind tehnici de tip greedy și programare dinamică.
- **Structuri de date avansate**: Proiectul utilizează structuri precum heapuri, stive, cozi de priorități și multe altele din STL pentru a optimiza algoritmii.
- **Design modular**: Codul este organizat în mod modular, permițând extinderea și reutilizarea ușoară a componentelor pentru proiecte viitoare.

### Scopul Proiectului

Acest cod reprezintă o evaluare academică, dar și o oportunitate de a arăta competențele mele în implementarea și utilizarea algoritmilor pentru grafuri complexe. Proiectul poate fi folosit ca bază pentru dezvoltarea unor aplicații mai avansate sau ca material educațional pentru învățarea algoritmilor de bază și avansați.
