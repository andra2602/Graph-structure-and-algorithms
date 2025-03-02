#include <iostream>
#include <fstream>
#include <vector>
#include <tuple> // pentru std::tuple
#include <stack>
#include <queue>
#include <algorithm>
#include <functional>
#include <map>
#include <numeric>
#include <set>

using namespace std;


/*****-------LEGENDA------------

    NEORIENTAT = 0 ; ORIENTAT = 1
    NEPONDERAT = 0 ; PONDERAT = 1
 *****/


class Graf {
private:
    int nrNoduri, nrMuchii;
    bool orientat;  // Indică dacă graful este orientat        NEORIENTAT = 0 ; ORIENTAT = 1
    bool ponderat;  // Indică dacă graful este ponderat        NEPONDERAT = 0 ; PONDERAT = 1
    vector<vector<pair<int, int>>> listaAdiacenta; // Lista de adiacență (vecin, greutate)
    vector<vector<int>> matriceAdiacenta;          // Matricea de adiacență (greutăți)
    vector<tuple<int, int, int>> listaMuchii;      // Lista de muchii (nod1, nod2, greutate)

    void initStructuri() {
        listaAdiacenta = vector<vector<pair<int, int>>>(nrNoduri + 1);
        matriceAdiacenta = vector<vector<int>>(nrNoduri + 1, vector<int>(nrNoduri + 1, 0));
        listaMuchii.clear();
    }

    // Funcție auxiliară pentru Flood Fill
    void floodFill(int nod, vector<bool>& vizitat) {
        vizitat[nod] = true;

        for (auto [vecin, _] : listaAdiacenta[nod]) {
            if (!vizitat[vecin]) {
                floodFill(vecin, vizitat);
            }
        }
    }

    void tarjanDFS(int nod) {
        index[nod] = lowLink[nod] = time++;
        stiva.push(nod);
        inStack[nod] = true;

        for (auto [vecin, _] : listaAdiacenta[nod]) {
            if (index[vecin] == -1) {
                tarjanDFS(vecin);
                lowLink[nod] = min(lowLink[nod], lowLink[vecin]);
            } else if (inStack[vecin]) {
                lowLink[nod] = min(lowLink[nod], index[vecin]);
            }
        }

        if (lowLink[nod] == index[nod]) {
            vector<int> componenta;
            int v;
            do {
                v = stiva.top();
                stiva.pop();
                inStack[v] = false;
                componenta.push_back(v);
            } while (v != nod);
            componenteTareConexe.push_back(componenta);
        }
    }


    vector<int> disc, low;
    vector<bool> vizitat;
    vector<pair<int, int>> muchiiCritice;
    vector<int> puncteCritice;
    int parent;

    // Funcție auxiliară pentru DFS (determinarea punctelor și muchiilor critice)
    void criticiDFS(int nod, int parinte) {
        static int timp = 0; // Timpul DFS global
        disc[nod] = low[nod] = ++timp; // Inițializăm disc și low pentru nodul curent
        int copii = 0; // Numărăm câți copii are nodul în arborele DFS

        for (auto [vecin, _] : listaAdiacenta[nod]) {
            if (vecin == parinte) continue; // Sărim peste muchia către părinte

            if (disc[vecin] == -1) { // Dacă vecinul nu a fost vizitat
                copii++;
                criticiDFS(vecin, nod);

                // Actualizăm valoarea `low` a nodului curent pe baza copilului
                low[nod] = min(low[nod], low[vecin]);

                // Dacă nodul este un punct critic
                if (parinte == -1 && copii > 1) { // Cazul special pentru rădăcină
                    puncteCritice.push_back(nod);
                }
                if (parinte != -1 && low[vecin] >= disc[nod]) {
                    puncteCritice.push_back(nod);
                }

                // Dacă muchia (nod, vecin) este critică
                if (low[vecin] > disc[nod]) {
                    muchiiCritice.emplace_back(nod, vecin);
                }
            } else { // Dacă vecinul a fost deja vizitat, actualizăm `low`
                low[nod] = min(low[nod], disc[vecin]);
            }
        }
    }

    // functie auxiliara pt Cuplaj maxim in graf bipartit
    // Funcție pentru a găsi un drum de creștere folosind DFS
    // Funcție pentru a găsi un drum de creștere folosind DFS
    bool dfsCuplaj(int u, vector<bool>& vizitat, vector<int>& pereche, vector<int>& parent) {
        for (auto [v, _] : listaAdiacenta[u]) {
            if (!vizitat[v]) {
                vizitat[v] = true;
                // Dacă v nu este cuplat sau putem găsi un drum de creștere
                if (pereche[v] == -1 || dfsCuplaj(pereche[v], vizitat, pereche, parent)) {
                    pereche[v] = u;
                    parent[u] = v;
                    return true;
                }
            }
        }
        return false;
    }

public:
    Graf() : nrNoduri(0), nrMuchii(0), orientat(false), ponderat(false) {}
    int getNumarNoduri() const {
        return nrNoduri;
    }
    // ===================================================================================
    // 1. Citire si afisare graf indiferent de tipul lui
    // ===================================================================================
    void citireDinFisier(const string& numeFisier) {
        ifstream inputFile(numeFisier);
        if (!inputFile.is_open()) {
            cerr << "Eroare la deschiderea fisierului: " << numeFisier << endl;
            return;
        }

        inputFile >> nrNoduri >> nrMuchii >> orientat >> ponderat;
        if (inputFile.fail()) {
            cerr << "Eroare: Nu s-au putut citi numarul de noduri, muchii, orientarea sau tipul ponderat din fisier." << endl;
            return;
        }

        initStructuri();

        for (int i = 0; i < nrMuchii; ++i) {
            int u, v, greutate = 1; // Greutatea implicită este 1 pentru grafuri neponderate
            inputFile >> u >> v;

            if (ponderat && !(inputFile >> greutate)) {
                cerr << "Eroare: Datele muchiilor sunt incomplete sau incorecte." << endl;
                return;
            }

            // Adaugă muchia în structurile de date
            listaAdiacenta[u].emplace_back(v, greutate);
            matriceAdiacenta[u][v] = greutate;
            listaMuchii.emplace_back(u, v, greutate);

            // Dacă graful nu este orientat, adaugă și muchia inversă
            if (!orientat) {
                listaAdiacenta[v].emplace_back(u, greutate);
                matriceAdiacenta[v][u] = greutate;
            }
        }

        inputFile.close();
        cout << "Datele au fost citite cu succes din fisierul: " << numeFisier << endl;
    }

    void citireGraf() {

        cin >> nrNoduri >> nrMuchii ;
        orientat = 1;
        ponderat = 1;
        initStructuri();

        for (int i = 0; i < nrMuchii; ++i) {
            int u, v, greutate; // Greutatea implicită este 1 pentru grafuri neponderate
            cin >> u >> v >> greutate;

            // Adaugă muchia în structurile de date
            listaAdiacenta[u].emplace_back(v, greutate);
            matriceAdiacenta[u][v] = greutate;
            listaMuchii.emplace_back(u, v, greutate);
        }

    }

    void afisareMatriceAdiacenta() {
        cout << "Matricea de adiacenta:" << endl;
        for (int i = 1; i <= nrNoduri; ++i) {
            for (int j = 1; j <= nrNoduri; ++j) {
                cout << matriceAdiacenta[i][j] << " ";
            }
            cout << endl;
        }
    }

    void afisareListaAdiacenta() {
        cout << "Lista de adiacenta:" << endl;
        for (int i = 1; i <= nrNoduri; ++i) {
            cout << "Nod " << i << ":";
            for (auto [vecin, greutate] : listaAdiacenta[i]) {
                cout << " (" << vecin << ", " << greutate << ")";
            }
            cout << endl;
        }
    }

    void afisareListaMuchii() {
        cout << "Lista de muchii:" << endl;
        for (auto [u, v, greutate] : listaMuchii) {
            cout << "(" << u << ", " << v << ", " << greutate << ")\n";
        }
    }

    void afisareInformatiiGraf() {
        cout << "Numarul de noduri: " << nrNoduri << endl;
        cout << "Numarul de muchii: " << nrMuchii << endl;
        cout << "Graful este " << (orientat ? "orientat" : "neorientat") << "." << endl;
        cout << "Graful este " << (ponderat ? "ponderat" : "neponderat") << "." << endl;
    }


    // ===================================================================================
    // 2. BFS SI DFS (toate tipurile de grafuri)
    // ===================================================================================

    // Funcție pentru BFS (Parcurgere în lățime)
    vector<int> bfs(int start) {
        vector<bool> vizitat(nrNoduri + 1, false);
        queue<int> coada;
        vector<int> ordine;

        vizitat[start] = true;
        coada.push(start);

        while (!coada.empty()) {
            int nod = coada.front();
            coada.pop();
            ordine.push_back(nod);

            for (auto [vecin, _] : listaAdiacenta[nod]) {
                if (!vizitat[vecin]) {
                    vizitat[vecin] = true;
                    coada.push(vecin);
                }
            }
        }

        return ordine;
    }

    // Funcție pentru DFS (Parcurgere în adâncime) - iterativ
    vector<int> dfs(int start) {
        vector<bool> vizitat(nrNoduri + 1, false);
        stack<int> stiva;
        vector<int> ordine;

        stiva.push(start);

        while (!stiva.empty()) {
            int nod = stiva.top();
            stiva.pop();

            if (!vizitat[nod]) {
                vizitat[nod] = true;
                ordine.push_back(nod);

                for (auto [vecin, _] : listaAdiacenta[nod]) {
                    if (!vizitat[vecin]) {
                        stiva.push(vecin);
                    }
                }
            }
        }

        return ordine;
    }


    // ===================================================================================
    // 3. Verificare graf bipartit (graf neorientat)
    // ===================================================================================

    // Funcție pentru verificarea unui graf bipartit
    bool esteBipartit() {
        vector<int> culori(nrNoduri + 1, -1); // Inițial toate nodurile sunt necolorate (-1)
        queue<int> q;

        for (int i = 1; i <= nrNoduri; ++i) { // Verificăm toate componentele conexe
            if (culori[i] == -1) { // Dacă nodul nu este vizitat
                culori[i] = 0; // Colorăm nodul curent cu 0
                q.push(i);

                while (!q.empty()) {
                    int nod = q.front();
                    q.pop();

                    for (auto [vecin, _] : listaAdiacenta[nod]) {
                        if (culori[vecin] == -1) {
                            culori[vecin] = 1 - culori[nod]; // Colorăm vecinul cu culoarea opusă
                            q.push(vecin);
                        } else if (culori[vecin] == culori[nod]) {
                            return false; // Dacă doi vecini au aceeași culoare, graful nu e bipartit
                        }
                    }
                }
            }
        }
        return true; // Dacă toate componentele sunt bipartite
    }


    // ===================================================================================
    // 4. Sortarea topologica (graf orientat aciclic)
    // ===================================================================================

    // Funcție pentru sortarea topologică (valabilă doar pentru grafuri orientate aciclice)
    vector<int> sortareTopologica() {
        if (!orientat) {
            throw runtime_error("Sortarea topologica este valabila doar pentru grafuri orientate.");
        }

        vector<int> inDegree(nrNoduri + 1, 0); // Gradul intern pentru fiecare nod

        // Calculăm gradul intern pentru fiecare nod
        for (int i = 1; i <= nrNoduri; ++i) {
            for (auto [vecin, _] : listaAdiacenta[i]) {
                inDegree[vecin]++;
            }
        }

        // Adăugăm în coadă nodurile cu grad intern 0
        queue<int> q;
        for (int i = 1; i <= nrNoduri; ++i) {
            if (inDegree[i] == 0) {
                q.push(i);
            }
        }

        vector<int> ordine;
        while (!q.empty()) {
            int nod = q.front();
            q.pop();
            ordine.push_back(nod);

            // Reducem gradul intern al vecinilor și îi adăugăm în coadă dacă devin 0
            for (auto [vecin, _] : listaAdiacenta[nod]) {
                inDegree[vecin]--;
                if (inDegree[vecin] == 0) {
                    q.push(vecin);
                }
            }
        }

        // Dacă nu am parcurs toate nodurile, înseamnă că graful are cicluri
        if (ordine.size() != nrNoduri) {
            throw runtime_error("Graful are cicluri, sortarea topologica nu este posibila.");
        }

        return ordine;
    }


    // ===================================================================================
    // 4. Componente conexe (graf neorientat)
    // ===================================================================================

    // Funcție pentru a găsi componentele conexe folosind Flood Fill
    void componenteConexe() {
        if (orientat) {
            throw runtime_error("Componentele conexe sunt valabile doar pentru grafuri neorientate.");
        }

        vector<bool> vizitat(nrNoduri + 1, false);
        int numarComponente = 0;

        for (int i = 1; i <= nrNoduri; ++i) {
            if (!vizitat[i]) {
                numarComponente++;
                floodFill(i, vizitat);
            }
        }

        cout << "Numarul de componente conexe: " << numarComponente << "\n";
    }


    // ===================================================================================
    // 5. Componente tare conexe (graf orientat)
    // ===================================================================================

    // Variabile pentru Tarjan
    vector<int> index, lowLink;
    vector<bool> inStack;
    stack<int> stiva;
    int time;
    vector<vector<int>> componenteTareConexe;

    // Funcție pentru a găsi componentele tare conexe folosind Tarjan
    void componenteTareConexeTarjan() {
        if (!orientat) {
            throw runtime_error("Componentele tare conexe sunt valabile doar pentru grafuri orientate.");
        }

        index.assign(nrNoduri + 1, -1);
        lowLink.assign(nrNoduri + 1, -1);
        inStack.assign(nrNoduri + 1, false);
        time = 0;
        componenteTareConexe.clear();

        for (int i = 1; i <= nrNoduri; ++i) {
            if (index[i] == -1) {
                tarjanDFS(i);
            }
        }

        cout << "Componente tare conexe:\n";
        for (const auto& componenta : componenteTareConexe) {
            for (int nod : componenta) {
                cout << nod << " ";
            }
            cout << "\n";
        }
    }




    // ===================================================================================
    // 6. Puncte si muchii critice (graf neorientat)
    // ===================================================================================

    // Funcție pentru determinarea punctelor și muchiilor critice
    void puncteSiMuchiiCritice() {
        disc.assign(nrNoduri + 1, -1);
        low.assign(nrNoduri + 1, -1);
        vizitat.assign(nrNoduri + 1, false);
        muchiiCritice.clear();
        puncteCritice.clear();

        for (int i = 1; i <= nrNoduri; ++i) {
            if (disc[i] == -1) { // Începem DFS din nodurile neexplorate
                criticiDFS(i, -1);
            }
        }

        // Eliminăm duplicatele din punctele critice
        sort(puncteCritice.begin(), puncteCritice.end());
        puncteCritice.erase(unique(puncteCritice.begin(), puncteCritice.end()), puncteCritice.end());

        cout << "Puncte critice: ";
        for (int p : puncteCritice) {
            cout << p << " ";
        }
        cout << "\n";

        cout << "Muchii critice: ";
        for (const auto& m : muchiiCritice) {
            cout << "(" << m.first << ", " << m.second << ") ";
        }
        cout << "\n";
    }


    // ===================================================================================
    // 7. Dijkstra (graf neorientat/orientat cu muchii ponderate)
    // ===================================================================================

    // Funcție pentru algoritmul Dijkstra (determinarea distanțelor minime de la un nod sursă)
    vector<int> dijkstra(int start) {
        if (!ponderat) {
            throw runtime_error("Algoritmul Dijkstra este valabil doar pentru grafuri ponderate.");
        }

        vector<int> dist(nrNoduri + 1, INT_MAX); // Inițializăm distanțele la infinit
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> minHeap; // Min-Heap pentru noduri

        dist[start] = 0;
        minHeap.push({0, start}); // Adăugăm nodul de start cu distanța 0

        while (!minHeap.empty()) {
            int distCurenta = minHeap.top().first;
            int nodCurent = minHeap.top().second;
            minHeap.pop();

            if (distCurenta > dist[nodCurent]) continue;

            // Relaxăm toate muchiile care pleacă din nodul curent
            for (auto [vecin, greutate] : listaAdiacenta[nodCurent]) {
                if (dist[nodCurent] + greutate < dist[vecin]) {
                    dist[vecin] = dist[nodCurent] + greutate;
                    minHeap.push({dist[vecin], vecin});
                }
            }
        }

        return dist; // Returnăm vectorul cu distanțele minime
    }


    // ===================================================================================
    // 8. Padure (graf neorientat, fara cicluri, fiecare comp conexa este un arbore)
    // ===================================================================================

    // Funcție pentru verificarea dacă graful este o pădure
    bool estePadure() {
        if (orientat) {
            throw runtime_error("Un graf orientat nu poate fi o pădure.");
        }

        vector<bool> vizitat(nrNoduri + 1, false);
        int componente = 0;

        // Funcție DFS pentru parcurgerea unei componente conexe
        function<void(int, int&, int&)> dfs = [&](int nod, int& noduriInComponenta, int& muchiiInComponenta) {
            vizitat[nod] = true;
            noduriInComponenta++; // Contorizăm nodul curent

            for (auto [vecin, _] : listaAdiacenta[nod]) {
                muchiiInComponenta++; // Contorizăm muchia (contorizată de 2 ori în graf neorientat)
                if (!vizitat[vecin]) {
                    dfs(vecin, noduriInComponenta, muchiiInComponenta);
                }
            }
        };

        // Iterăm prin toate nodurile pentru a verifica componentele conexe
        for (int i = 1; i <= nrNoduri; ++i) {
            if (!vizitat[i]) { // Dacă găsim un nod nevizitat, începem o nouă componentă conexă
                componente++;
                int noduriInComponenta = 0;
                int muchiiInComponenta = 0;

                // Parcurgem componenta conexă
                dfs(i, noduriInComponenta, muchiiInComponenta);

                // Ajustăm muchiile (în graf neorientat sunt contorizate de 2 ori)
                muchiiInComponenta /= 2;

                // Verificăm dacă această componentă este un arbore
                if (muchiiInComponenta != noduriInComponenta - 1) {
                    cout << "Componenta " << componente << " NU este arbore.\n";
                    return false; // Dacă o componentă nu este arbore, graful nu este pădure
                }
            }
        }

        return true; // Dacă toate componentele sunt arbori, graful este o pădure
    }


    // ===================================================================================
    // 9. Kruskal (graf neorientat cu ponderi pe muchii)
    // ===================================================================================

    // Funcție pentru determinarea Arborelui Parțial de Cost Minim (APM) folosind algoritmul Kruskal
    vector<tuple<int, int, int>> kruskalAPM() {
        vector<tuple<int, int, int>> apm; // Arborele parțial minim rezultat
        vector<int> parinte(nrNoduri + 1);

        // Inițializare: fiecare nod este propriul său părinte
        for (int i = 1; i <= nrNoduri; ++i) {
            parinte[i] = i;
        }

        // Funcție pentru găsirea rădăcinii unui nod (cu compresie)
        std::function<int(int)> gasesteRadacina = [&](int nod) -> int {
            if (parinte[nod] != nod) {
                parinte[nod] = gasesteRadacina(parinte[nod]);
            }
            return parinte[nod];
        };

        // Sortăm lista de muchii după cost
        sort(listaMuchii.begin(), listaMuchii.end(), [](const auto& a, const auto& b) {
            return get<2>(a) < get<2>(b); // Comparăm greutățile muchiilor
        });

        // Parcurgem muchiile în ordine crescătoare a greutății
        for (const auto& [u, v, greutate] : listaMuchii) {
            int radU = gasesteRadacina(u);
            int radV = gasesteRadacina(v);

            // Dacă nodurile u și v sunt în componente diferite, le unim
            if (radU != radV) {
                apm.emplace_back(u, v, greutate); // Adăugăm muchia în APM
                parinte[radU] = radV;            // Unim cele două componente
            }
        }

        return apm;
    }


    // ===================================================================================
    // 10. Prim (graf neorientat cu ponderi pe muchii)
    // ===================================================================================

    // Funcție pentru determinarea Arborelui Parțial Minim (APM) folosind algoritmul Prim
    vector<pair<int, int>> primAPM() {
        vector<bool> inAPM(nrNoduri + 1, false);           // Marcăm nodurile incluse în APM
        vector<int> cost(nrNoduri + 1, INT_MAX);           // Costul pentru a adăuga fiecare nod
        vector<int> parinte(nrNoduri + 1, -1);             // Părintele fiecărui nod în APM
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> minHeap; // Min-Heap pentru selectarea nodului cu cost minim
        vector<pair<int, int>> apm;                        // Lista muchiilor din APM

        cost[1] = 0;                                       // Pornim de la nodul 1
        minHeap.push({0, 1});

        while (!minHeap.empty()) {
            int nod = minHeap.top().second;
            minHeap.pop();

            if (inAPM[nod]) continue;                      // Dacă nodul este deja în APM, trecem mai departe
            inAPM[nod] = true;

            if (parinte[nod] != -1) {
                apm.emplace_back(parinte[nod], nod);       // Adăugăm muchia la APM
            }

            for (auto [vecin, greutate] : listaAdiacenta[nod]) {
                if (!inAPM[vecin] && greutate < cost[vecin]) {
                    cost[vecin] = greutate;               // Actualizăm costul
                    parinte[vecin] = nod;                 // Setăm părintele
                    minHeap.push({cost[vecin], vecin});   // Adăugăm vecinul în heap
                }
            }
        }

        return apm;
    }


    // ===================================================================================
    // 11. Clustering (graf neorientat cu ponderi, se bazeaza pe Kruskal)
    // ===================================================================================

    // Funcție pentru determinarea clusterelor prin eliminarea celor mai grele k-1 muchii din APM
    vector<vector<int>> clustering(int k) {
        vector<tuple<int, int, int>> muchiiAPM = kruskalAPM(); // Obținem APM folosind Kruskal

        // Sortăm muchiile descrescător după greutate
        sort(muchiiAPM.begin(), muchiiAPM.end(), [](const auto& a, const auto& b) {
            return get<2>(a) > get<2>(b);
        });

        // Eliminăm cele mai grele k-1 muchii
        muchiiAPM.erase(muchiiAPM.begin(), muchiiAPM.begin() + (k - 1));

        // Inițializăm Union-Find
        vector<int> parinte(nrNoduri + 1);
        for (int i = 1; i <= nrNoduri; ++i) {
            parinte[i] = i;
        }

        // Funcție pentru găsirea rădăcinii unui nod (cu compresie)
        std::function<int(int)> gasesteRadacina = [&](int nod) -> int {
            if (parinte[nod] != nod) {
                parinte[nod] = gasesteRadacina(parinte[nod]);
            }
            return parinte[nod];
        };

        // Construim componentele conexe rămase
        for (const auto& [u, v, _] : muchiiAPM) {
            int radU = gasesteRadacina(u);
            int radV = gasesteRadacina(v);
            if (radU != radV) {
                parinte[radU] = radV;
            }
        }

        // Grupăm nodurile în funcție de rădăcina lor
        map<int, vector<int>> clusterMap;
        for (int i = 1; i <= nrNoduri; ++i) {
            clusterMap[gasesteRadacina(i)].push_back(i);
        }

        vector<vector<int>> clusteri;
        for (const auto& [_, cluster] : clusterMap) {
            clusteri.push_back(cluster);
        }

        return clusteri;
    }


    // ===================================================================================
    // 12. Drum critic/CPM (graf orientat si ponderat)
    // ===================================================================================

    vector<int> criticalPath() {
        // Obținem sortarea topologică a grafului
        vector<int> topSort = sortareTopologica();
        vector<int> earliest(nrNoduri + 1, 0);        // Timpii cei mai devreme
        vector<int> latest(nrNoduri + 1, INT_MAX);    // Timpii cei mai târziu

        // Calculăm timpii de început (earliest start times)
        for (int nod : topSort) {
            for (auto [vecin, greutate] : listaAdiacenta[nod]) {
                earliest[vecin] = max(earliest[vecin], earliest[nod] + greutate);
            }
        }

        // Durata maximă a proiectului este timpul cel mai târziu al ultimului nod
        int durataMaxima = *max_element(earliest.begin(), earliest.end());
        fill(latest.begin(), latest.end(), durataMaxima);

        // Calculăm timpii de sfârșit (latest finish times) în ordine inversă
        reverse(topSort.begin(), topSort.end());
        for (int nod : topSort) {
            for (auto [vecin, greutate] : listaAdiacenta[nod]) {
                latest[nod] = min(latest[nod], latest[vecin] - greutate);
            }
        }

        // Identificăm muchiile care aparțin drumului critic
        vector<int> drumCritic;
        int nodCurent = 1; // Presupunem că nodul de start este 1 (se poate schimba în funcție de graful dat)

        drumCritic.push_back(nodCurent);
        while (nodCurent <= nrNoduri) {
            bool muchieCriticaGasita = false;
            for (auto [vecin, greutate] : listaAdiacenta[nodCurent]) {
                if (earliest[nodCurent] + greutate == earliest[vecin] &&
                    latest[nodCurent] + greutate == latest[vecin]) {
                    drumCritic.push_back(vecin);
                    nodCurent = vecin;
                    muchieCriticaGasita = true;
                    break;
                }
            }
            if (!muchieCriticaGasita) break; // Nu mai sunt muchii critice
        }

        return drumCritic;
    }


    // ===================================================================================
    // 13. Bellman Ford (graf orientat/neorientat cu ponderi pozitive/negative/nule)
    // ===================================================================================

    vector<int> bellmanFord(int start) {
        vector<int> dist(nrNoduri + 1, INT_MAX); // Inițializăm toate distanțele cu infinit
        dist[start] = 0; // Distanța de la sursă la sine este 0

        // Relaxăm toate muchiile de (nrNoduri - 1) ori
        for (int i = 1; i <= nrNoduri - 1; ++i) {
            for (const auto &[u, v, greutate] : listaMuchii) {
                if (dist[u] != INT_MAX && dist[u] + greutate < dist[v]) {
                    dist[v] = dist[u] + greutate;
                }
            }
        }

        // Verificăm dacă există cicluri de cost negativ
        for (const auto &[u, v, greutate] : listaMuchii) {
            if (dist[u] != INT_MAX && dist[u] + greutate < dist[v]) {
                throw runtime_error("Graful contine un ciclu de cost negativ.");
            }
        }

        return dist;
    }


    // ===================================================================================
    // 14. Floyd-Warshall (graf orientat/neorientat cu ponderi)
    // ===================================================================================

    void floydWarshall() {
        // Inițializare matrice distanțe
        vector<vector<int>> dist(nrNoduri + 1, vector<int>(nrNoduri + 1, INT_MAX));

        // Setăm distanța inițială pentru fiecare nod
        for (int i = 1; i <= nrNoduri; ++i) {
            dist[i][i] = 0; // Distanța la sine este 0
        }

        // Inițializăm distanțele folosind listaMuchii
        for (const auto& [u, v, greutate] : listaMuchii) {
            dist[u][v] = greutate;
            if (!orientat) {
                dist[v][u] = greutate; // Pentru grafuri neorientate
            }
        }

        // Aplicăm algoritmul Floyd-Warshall
        for (int k = 1; k <= nrNoduri; ++k) {
            for (int i = 1; i <= nrNoduri; ++i) {
                for (int j = 1; j <= nrNoduri; ++j) {
                    if (dist[i][k] != INT_MAX && dist[k][j] != INT_MAX) {
                        dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
                    }
                }
            }
        }

        // Afișăm matricea distanțelor minime
        cout << "Matricea distantelor minime folosind Floyd-Warshall:\n";
        for (int i = 1; i <= nrNoduri; ++i) {
            for (int j = 1; j <= nrNoduri; ++j) {
                if (dist[i][j] == INT_MAX)
                    cout << "INF ";
                else
                    cout << dist[i][j] << " ";
            }
            cout << endl;
        }
    }


    // ===================================================================================
    // 15. Flux maxim de cost minim(graf orientat cu ponderi)
    // ===================================================================================

    pair<int, int> fluxMaxCostMin(int start, int end) {
        vector<vector<int>> capacitate(nrNoduri + 1, vector<int>(nrNoduri + 1, 0));
        vector<vector<int>> cost(nrNoduri + 1, vector<int>(nrNoduri + 1, 0));
        vector<vector<int>> adj(nrNoduri + 1);

        // Inițializăm capacitățile și costurile folosind listaMuchii
        for (const auto &[u, v, greutate] : listaMuchii) {
            capacitate[u][v] = 1; // Capacitatea poate fi ajustată aici
            cost[u][v] = greutate;
            cost[v][u] = -greutate; // Costul pentru muchia inversă
            adj[u].push_back(v);
            adj[v].push_back(u);
        }

        int fluxTotal = 0, costTotal = 0;

        while (true) {
            vector<int> dist(nrNoduri + 1, INT_MAX), parinte(nrNoduri + 1, -1);
            vector<bool> inQueue(nrNoduri + 1, false);
            dist[start] = 0;
            queue<int> q;
            q.push(start);
            inQueue[start] = true;

            // Algoritm Bellman-Ford pentru determinarea drumului de cost minim
            while (!q.empty()) {
                int u = q.front();
                q.pop();
                inQueue[u] = false;

                for (int v : adj[u]) {
                    if (capacitate[u][v] > 0 && dist[u] + cost[u][v] < dist[v]) {
                        dist[v] = dist[u] + cost[u][v];
                        parinte[v] = u;
                        if (!inQueue[v]) {
                            q.push(v);
                            inQueue[v] = true;
                        }
                    }
                }
            }

            // Dacă nu există drumuri către destinație, terminăm
            if (dist[end] == INT_MAX) break;

            // Calculăm fluxul minim de-a lungul drumului găsit
            int flux = INT_MAX;
            for (int v = end; v != start; v = parinte[v]) {
                int u = parinte[v];
                flux = min(flux, capacitate[u][v]);
            }

            // Afișează detalii despre drum și cost
            cout << "\nDrum gasit:\n";
            for (int v = end; v != start; v = parinte[v]) {
                int u = parinte[v];
                cout << u << " -> " << v << " (capacitate ramasa: " << capacitate[u][v]
                     << ", cost: " << cost[u][v] << ")\n";
            }
            cout << "Flux adaugat: " << flux << "\n";

            // Actualizăm capacitățile și costurile
            for (int v = end; v != start; v = parinte[v]) {
                int u = parinte[v];
                capacitate[u][v] -= flux;
                capacitate[v][u] += flux;
                costTotal += flux * cost[u][v];
            }

            fluxTotal += flux;
            cout << "Cost adaugat: " << flux * cost[parinte[end]][end] << "\n";
        }

        cout << "Flux total: " << fluxTotal << "\n";
        cout << "Cost total: " << costTotal << "\n";
        return {fluxTotal, costTotal};
    }


    // ===================================================================================
    // 16. Cuplaj maxim in graf bipartit/ Hopcroft-Karp (graf bipartit)
    // ===================================================================================

    // Funcție pentru determinarea cuplajului maxim
    int cuplajMaxim(int m) { // m = numărul de noduri din mulțimea A
        vector<int> pereche(nrNoduri + 1, -1); // Nodurile din B care sunt cuplate
        vector<int> parent(nrNoduri + 1, -1);  // Nodurile din A cuplate
        int cuplaj = 0;

        for (int u = 1; u <= m; ++u) { // Parcurgem nodurile din A (1 până la m)
            vector<bool> vizitat(nrNoduri + 1, false);
            if (dfsCuplaj(u, vizitat, pereche, parent)) {
                ++cuplaj; // Creștem cuplajul dacă găsim un drum de creștere
            }
        }

        // Afișăm perechile din cuplajul maxim
        cout << "Cuplaj maxim:\n";
        for (int v = 1; v <= nrNoduri; ++v) {
            if (pereche[v] != -1) {
                cout << pereche[v] << " - " << v << "\n";
            }
        }

        return cuplaj;
    }

    // ===================================================================================
    // 17. Constructia unui graf orientat cu secvente de grade de intrare si iesire date
    // ===================================================================================

    void afisareListaAdiacentaPersonalizata() {
        cout << "Lista de adiacenta:\n";
        for (int i = 1; i <= nrNoduri; ++i) {
            cout << "Nod " << i << ":";
            for (const auto& [vecin, greutate] : listaAdiacenta[i]) {
                cout << " (" << vecin << ", " << greutate << ")";
            }
            cout << "\n";
        }
    }

    void afisareListaMuchiiPersonalizata() {
        cout << "Lista de muchii:\n";
        for (const auto& [u, v, greutate] : listaMuchii) {
            cout << "(" << u << ", " << v << ", " << greutate << ")\n";
        }
    }

    void citireDinFisierGrade(const string& numeFisier, vector<int>& inDegree, vector<int>& outDegree) {
        ifstream inputFile(numeFisier);

        if (!inputFile.is_open()) {
            cerr << "Eroare: Nu s-a putut deschide fisierul " << numeFisier << ".\n";
            return;
        }

        int n;
        inputFile >> n; // Citim numărul de noduri
        if (inputFile.fail() || n <= 0) {
            cerr << "Eroare: Nu s-a putut citi numarul de noduri sau este invalid.\n";
            return;
        }

        inDegree.assign(n + 1, 0);
        outDegree.assign(n + 1, 0);

        // Citim gradele de intrare
        for (int i = 1; i <= n; ++i) {
            if (!(inputFile >> inDegree[i])) {
                cerr << "Eroare: Nu s-au putut citi toate gradele de intrare.\n";
                return;
            }
        }

        // Citim gradele de ieșire
        for (int i = 1; i <= n; ++i) {
            if (!(inputFile >> outDegree[i])) {
                cerr << "Eroare: Nu s-au putut citi toate gradele de iesire.\n";
                return;
            }
        }

        if (inputFile.fail()) {
            cerr << "Eroare: Datele din fisier nu sunt corecte.\n";
            return;
        }

        nrNoduri = n; // Setăm numărul de noduri
        inputFile.close();
        cout << "Datele au fost citite cu succes din fisierul " << numeFisier << ".\n";
    }


    bool construiesteGrafOrientat(vector<int>& inDegree, vector<int>& outDegree) {
        if (inDegree.size() != nrNoduri + 1 || outDegree.size() != nrNoduri + 1) {
            cerr << "Eroare: Dimensiunea vectorilor de grade nu corespunde cu numarul de noduri.\n";
            return false;
        }

        // Verificăm dacă suma gradelor de intrare este egală cu suma gradelor de ieșire
        int sumIn = accumulate(inDegree.begin(), inDegree.end(), 0);
        int sumOut = accumulate(outDegree.begin(), outDegree.end(), 0);
        if (sumIn != sumOut) {
            cerr << "Eroare: Suma gradelor de intrare nu este egala cu suma gradelor de iesire.\n";
            return false;
        }

        initStructuri(); // Resetăm structurile grafului

        queue<pair<int, int>> outQueue, inQueue;
        set<pair<int, int>> muchiiExistente; // Reținem perechile de muchii existente

        // Adăugăm nodurile cu grade inițiale
        for (int i = 1; i <= nrNoduri; ++i) {
            if (outDegree[i] > 0) outQueue.push({i, outDegree[i]});
            if (inDegree[i] > 0) inQueue.push({i, inDegree[i]});
        }

        // Conectăm nodurile
        while (!outQueue.empty() && !inQueue.empty()) {
            auto [u, outDeg] = outQueue.front();
            auto [v, inDeg] = inQueue.front();

            outQueue.pop();
            inQueue.pop();

            // Evităm buclele și muchiile duplicate
            if (u != v && !muchiiExistente.count({u, v})) {
                listaAdiacenta[u].emplace_back(v, 1);  // Greutate implicită 1
                matriceAdiacenta[u][v] = 1;
                listaMuchii.emplace_back(u, v, 1);
                muchiiExistente.insert({u, v}); // Marcăm că muchia există

                // Afișare conexiune realizată
                cout << "Conectam nodul " << u << " cu nodul " << v << endl;

                // Reducem gradele
                --outDeg;
                --inDeg;

                // Afișare grade rămase
                cout << "Grade ramase pentru noduri: " << outDeg << " (iesire), " << inDeg << " (intrare)" << endl;
            }

            // Dacă mai rămân grade pozitive, reintroducem nodurile în cozi
            if (outDeg > 0) outQueue.push({u, outDeg});
            if (inDeg > 0) inQueue.push({v, inDeg});
        }

        // Verificăm dacă toate gradele au fost folosite
        if (!outQueue.empty() || !inQueue.empty()) {
            cerr << "Eroare: Nu s-a putut construi graful cu datele date.\n";
            return false;
        }

        cout << "Graful a fost construit cu succes!" << endl;
        return true;
    }

    // ===================================================================================
    // 18. Graf eulerian (graf neorientat - grad par, graf conex)
    //                   (graf orientat - gradul de intrare = gradul de iesire)
    // ===================================================================================

    bool esteEulerian() {
        if (orientat) {
            // Gradurile de intrare și de ieșire pentru fiecare nod
            vector<int> inDegree(nrNoduri + 1, 0), outDegree(nrNoduri + 1, 0);
            for (int i = 1; i <= nrNoduri; ++i) {
                for (auto [vecin, _] : listaAdiacenta[i]) {
                    outDegree[i]++;
                    inDegree[vecin]++;
                }
            }

            // Verificăm dacă gradul de intrare este egal cu gradul de ieșire pentru fiecare nod
            for (int i = 1; i <= nrNoduri; ++i) {
                if (inDegree[i] != outDegree[i]) {
                    return false;
                }
            }
        } else {
            // Pentru grafuri neorientate, verificăm dacă fiecare nod are grad par
            for (int i = 1; i <= nrNoduri; ++i) {
                if (listaAdiacenta[i].size() % 2 != 0) {
                    return false;
                }
            }
        }

        return true; // Graful este Eulerian
    }


    // ===================================================================================
    // 19. Graf hamiltonian (graf neorientat - exista un ciclu hamiltonian = ciclu care trece o data prin fiecare nod)
    //                      (graf orientat - exista un drum hamiltonian )
    // ===================================================================================

    bool esteHamiltonian() {
        vector<bool> vizitat(nrNoduri + 1, false); // Vector pentru marcarea nodurilor vizitate
        vector<int> drum; // Vector pentru a stoca drumul curent

        // Funcție recursivă pentru parcurgerea DFS
        function<bool(int, int)> hamiltonianDFS = [&](int nod, int lungime) {
            vizitat[nod] = true;
            drum.push_back(nod);

            if (lungime == nrNoduri) {
                // Dacă există o muchie de la ultimul nod în drum la primul nod, este ciclu Hamiltonian
                for (auto [vecin, _] : listaAdiacenta[nod]) {
                    if (vecin == drum[0]) {
                        return true;
                    }
                }
            }

            for (auto [vecin, _] : listaAdiacenta[nod]) {
                if (!vizitat[vecin]) {
                    if (hamiltonianDFS(vecin, lungime + 1)) {
                        return true;
                    }
                }
            }

            vizitat[nod] = false; // Backtracking
            drum.pop_back();
            return false;
        };

        // Verificăm pentru fiecare nod dacă poate fi începutul unui ciclu Hamiltonian
        for (int i = 1; i <= nrNoduri; ++i) {
            if (hamiltonianDFS(i, 1)) {
                return true;
            }
        }

        return false;
    }


    // ===================================================================================
    // 20. Edmonds Karp (graf orientat cu fiecare muchie avand capacitate pozitiva)
    // ===================================================================================

    int edmondsKarp(int start, int end) {
        vector<vector<int>> capacitate(nrNoduri + 1, vector<int>(nrNoduri + 1, 0));

        // Inițializăm matricea de capacitate folosind lista de muchii
        for (const auto &[u, v, greutate] : listaMuchii) {
            capacitate[u][v] += greutate; // Adăugăm capacitatea pe muchie
        }

        vector<vector<int>> flux(nrNoduri + 1, vector<int>(nrNoduri + 1, 0));
        vector<int> parinte(nrNoduri + 1); // Părintele fiecărui nod în calea găsită de BFS
        int fluxMaxim = 0;

        while (true) {
            fill(parinte.begin(), parinte.end(), -1); // Inițializăm părinții
            queue<int> q;
            q.push(start);
            parinte[start] = start;

            // BFS pentru găsirea unei căi de augmentare
            while (!q.empty() && parinte[end] == -1) {
                int nod = q.front();
                q.pop();
                for (int vecin = 1; vecin <= nrNoduri; ++vecin) {
                    if (parinte[vecin] == -1 && capacitate[nod][vecin] - flux[nod][vecin] > 0) {
                        parinte[vecin] = nod;
                        q.push(vecin);
                        if (vecin == end) break; // Am ajuns la destinație
                    }
                }
            }

            // Dacă nu există o cale de augmentare, ieșim
            if (parinte[end] == -1) break;

            // Determinăm fluxul maxim posibil pe calea găsită
            int fluxAdaugat = INT_MAX;
            for (int nod = end; nod != start; nod = parinte[nod]) {
                int prev = parinte[nod];
                fluxAdaugat = min(fluxAdaugat, capacitate[prev][nod] - flux[prev][nod]);
            }

            // Actualizăm fluxul pe calea găsită
            for (int nod = end; nod != start; nod = parinte[nod]) {
                int prev = parinte[nod];
                flux[prev][nod] += fluxAdaugat;
                flux[nod][prev] -= fluxAdaugat;
            }

            fluxMaxim += fluxAdaugat; // Adăugăm fluxul calea găsită
        }

        return fluxMaxim;
    }


    // ===================================================================================
    // 21. Kosaraju (graf orientat - se determina comp tare conexe)
    // ===================================================================================

    void kosaraju() {
        vector<bool> vizitat(nrNoduri + 1, false);
        stack<int> ordine;

        // Primul DFS pentru a obține ordinea nodurilor
        function<void(int)> dfs1 = [&](int nod) {
            vizitat[nod] = true;
            for (auto [vecin, _] : listaAdiacenta[nod]) {
                if (!vizitat[vecin]) {
                    dfs1(vecin);
                }
            }
            ordine.push(nod);
        };

        // Aplicăm primul DFS pe toate nodurile
        for (int i = 1; i <= nrNoduri; ++i) {
            if (!vizitat[i]) {
                dfs1(i);
            }
        }

        // Construim graful transpus
        vector<vector<pair<int, int>>> transpus(nrNoduri + 1);
        for (int i = 1; i <= nrNoduri; ++i) {
            for (auto [vecin, greutate] : listaAdiacenta[i]) {
                transpus[vecin].emplace_back(i, greutate);
            }
        }

        // Resetăm vectorul de vizitare
        fill(vizitat.begin(), vizitat.end(), false);

        // Vector pentru a stoca componentele
        vector<vector<int>> componente;

        // Al doilea DFS pe graful transpus
        function<void(int, vector<int>&)> dfs2 = [&](int nod, vector<int>& componenta) {
            vizitat[nod] = true;
            componenta.push_back(nod);
            for (auto [vecin, _] : transpus[nod]) {
                if (!vizitat[vecin]) {
                    dfs2(vecin, componenta);
                }
            }
        };

        // Aplicăm al doilea DFS conform ordinii din stivă
        while (!ordine.empty()) {
            int nod = ordine.top();
            ordine.pop();
            if (!vizitat[nod]) {
                vector<int> componenta;
                dfs2(nod, componenta);
                componente.push_back(componenta);
            }
        }

        // Afișăm componentele tare conexe
        cout << "Componente tare conexe (Kosaraju):\n";
        for (const auto& componenta : componente) {
            for (int nod : componenta) {
                cout << nod << " ";
            }
            cout << "\n";
        }
    }

    // ===================================================================================
    // 22. 6 Colorarea unui graf  (6 culori disponibile ai nodurile adiacente au culori diferite )
    /*
    Pasi:
            1.Atribuim culorile nodurilor unul câte unul.
            2.Pentru fiecare nod, alegem cea mai mică culoare care nu este utilizată de vecinii săi.
            3.Continuăm până când toate nodurile sunt colorate.
    */
    // ===================================================================================

    void colorare6() {
        const int MAX_CULORI = 6;
        vector<int> culori(nrNoduri + 1, -1); // -1 indică noduri necolorate
        vector<vector<int>> grupuriCulori(MAX_CULORI); // Vector de vectori pentru nodurile pe culori

        for (int nod = 1; nod <= nrNoduri; ++nod) {
            // Marcăm culorile folosite de vecini
            vector<bool> culoriVecini(MAX_CULORI, false);
            for (auto [vecin, _] : listaAdiacenta[nod]) {
                if (culori[vecin] != -1) { // Dacă vecinul are o culoare, o marcăm ca folosită
                    culoriVecini[culori[vecin]] = true;
                }
            }

            // Găsim prima culoare disponibilă
            int culoareDisponibila = -1;
            for (int c = 0; c < MAX_CULORI; ++c) {
                if (!culoriVecini[c]) {
                    culoareDisponibila = c;
                    break;
                }
            }

            // Atribuim culoarea nodului curent
            if (culoareDisponibila == -1) {
                throw runtime_error("Nu se poate colora graful cu 6 culori (nu ar trebui să se întâmple pentru un graf planar).");
            }

            culori[nod] = culoareDisponibila;
            grupuriCulori[culoareDisponibila].push_back(nod);
        }

        // Afișăm rezultatul colorării
        // afisare de tipul nod i-> culoare a
        cout << "\nColorarea grafului (6 culori):\n";
        for (int nod = 1; nod <= nrNoduri; ++nod) {
            cout << "Nod " << nod << " -> Culoare " << culori[nod] << "\n";
        }
        cout << endl;

        // afisare de tipul culoare a : nod i ,nod j, nod k
        cout << "\nColorarea grafului (6 culori):\n";
        for (int c = 0; c < MAX_CULORI; ++c) {
            if (!grupuriCulori[c].empty()) {
                cout << "Culoarea " << c << " : ";
                for (int nod : grupuriCulori[c]) {
                    cout << nod << " ";
                }
                cout << "\n";
            }
        }
    }


    // ===================================================================================
    // 23. Havel hakimi si constructia unui graf neorientat
    // ===================================================================================

    bool havelHakimi(vector<int> secventa) {
        // Pas 1: Sortăm secvența în ordine descrescătoare
        sort(secventa.rbegin(), secventa.rend());

        while (!secventa.empty()) {
            // Pas 2: Eliminăm primul element al secvenței (gradul cel mai mare)
            int grad = secventa.front();
            secventa.erase(secventa.begin());

            // Verificăm dacă gradul este negativ sau depășește lungimea secvenței rămase
            if (grad < 0 || grad > secventa.size()) {
                return false; // Secvența nu este validă
            }

            // Pas 3: Reducem gradul următoarelor noduri
            for (int i = 0; i < grad; ++i) {
                secventa[i]--;

                // Dacă un grad devine negativ, secvența nu este validă
                if (secventa[i] < 0) {
                    return false;
                }
            }

            // Re-sortăm secvența după reducere
            sort(secventa.rbegin(), secventa.rend());
        }

        // Dacă am eliminat toate elementele fără erori, secvența este validă
        return true;
    }

    void construiesteGrafDinSecventa(const string& numeFisier) {
        ifstream inputFile(numeFisier);
        if (!inputFile.is_open()) {
            cerr << "Eroare la deschiderea fisierului: " << numeFisier << endl;
            return;
        }

        vector<int> secventa;
        int grad;
        while (inputFile >> grad) {
            secventa.push_back(grad);
        }
        inputFile.close();

        if (!havelHakimi(secventa)) {
            cout << "Secventa data nu poate genera un graf valid." << endl;
            return;
        }

        // Inițializăm structurile de date pentru graf
        nrNoduri = secventa.size();
        nrMuchii = 0;
        initStructuri();

        // Construim graful pe baza secvenței
        vector<int> noduri;
        for (int i = 0; i < nrNoduri; ++i) {
            noduri.push_back(i + 1); // Nodurile numerotate de la 1 la nrNoduri
        }

        while (!secventa.empty()) {
            // Sortăm secvența și nodurile în ordine descrescătoare a gradului
            vector<pair<int, int>> noduriGrade;
            for (int i = 0; i < secventa.size(); ++i) {
                noduriGrade.emplace_back(secventa[i], noduri[i]);
            }
            sort(noduriGrade.rbegin(), noduriGrade.rend());

            // Extragem nodul cu cel mai mare grad
            int grad = noduriGrade[0].first;
            int nodCurent = noduriGrade[0].second;

            // Verificăm dacă gradul poate fi satisfăcut
            if (grad > noduriGrade.size() - 1) {
                cerr << "Eroare: Gradul nu poate fi satisfăcut." << endl;
                return;
            }

            // Reducem gradul primilor `grad` noduri disponibile
            for (int i = 1; i <= grad; ++i) {
                int vecin = noduriGrade[i].second;

                // Adăugăm muchia între nodCurent și vecin
                listaAdiacenta[nodCurent].emplace_back(vecin, 1);
                listaAdiacenta[vecin].emplace_back(nodCurent, 1);
                matriceAdiacenta[nodCurent][vecin] = 1;
                matriceAdiacenta[vecin][nodCurent] = 1;
                listaMuchii.emplace_back(nodCurent, vecin, 1);

                // Reducem gradul vecinului
                noduriGrade[i].first--;
            }

            // Eliminăm nodurile procesate
            noduriGrade.erase(noduriGrade.begin());

            // Eliminăm nodurile cu grad zero
            noduriGrade.erase(remove_if(noduriGrade.begin(), noduriGrade.end(),
                                        [](pair<int, int> p) { return p.first == 0; }),
                              noduriGrade.end());

            // Actualizăm secvența și nodurile
            secventa.clear();
            noduri.clear();
            for (auto& [g, n] : noduriGrade) {
                secventa.push_back(g);
                noduri.push_back(n);
            }
        }

        cout << "Graful a fost construit cu succes din secventa data." << endl;
        afisareListaAdiacenta();
        afisareMatriceAdiacenta();
    }



    // ===================================================================================
    // 24. elimina muchie (sterge muchia din graf)
    // ===================================================================================

    void eliminaMuchie(int u, int v) {
        // Eliminăm muchia (u, v) din lista de adiacență
        auto it = remove_if(listaAdiacenta[u].begin(), listaAdiacenta[u].end(),
                            [v](const pair<int, int>& p) { return p.first == v; });
        listaAdiacenta[u].erase(it, listaAdiacenta[u].end());

        // Deoarece graful este neorientat, eliminăm și muchia (v, u)
        it = remove_if(listaAdiacenta[v].begin(), listaAdiacenta[v].end(),
                       [u](const pair<int, int>& p) { return p.first == u; });
        listaAdiacenta[v].erase(it, listaAdiacenta[v].end());

        // Eliminăm muchia din lista de muchii
        listaMuchii.erase(remove_if(listaMuchii.begin(), listaMuchii.end(),
                                    [u, v](const tuple<int, int, int>& edge) {
                                        return (get<0>(edge) == u && get<1>(edge) == v) ||
                                               (get<0>(edge) == v && get<1>(edge) == u);
                                    }),
                          listaMuchii.end());
    }

    // ===================================================================================
    // 25. Aici verific daca un graf are cel putin 2apm-uri distincte
    // ===================================================================================


    void gasesteDoiArboriDistincti(Graf& graf) {
        // Obținem primul arbore folosind Prim
        vector<pair<int, int>> apm1 = graf.primAPM();

        if (apm1.size() != graf.getNumarNoduri() - 1) {
            cout << "Nu" << endl;
            return;  // Dacă nu există un arbore de acoperire, returnăm direct
        }

        vector<pair<int, int>> apm2;
        bool existaDoiArbori = false;

        // Încercăm să construim un al doilea arbore eliminând câte o muchie diferită
        for (auto muchie : apm1) {
            Graf copieGraf = graf;

            // Eliminăm muchia curentă
            copieGraf.eliminaMuchie(muchie.first, muchie.second);

            vector<pair<int, int>> potentialApm = copieGraf.primAPM();

            // Verificăm dacă am găsit un arbore diferit de primul
            if (potentialApm.size() == graf.getNumarNoduri() - 1 && potentialApm != apm1) {
                apm2 = potentialApm;
                existaDoiArbori = true;
                break;
            }
        }

        if (!existaDoiArbori) {
            cout << "Nu" << endl;
        } else {
            cout << "Da" << endl;

            // Funcție pentru afișarea unui arbore sub formă de listă de adiacență
            auto afisareListaAdiacenta = [](const vector<pair<int, int>>& arbore, int nrNoduri) {
                vector<vector<int>> lista(nrNoduri + 1);
                for (auto [u, v] : arbore) {
                    lista[u].push_back(v);
                    lista[v].push_back(u);
                }
                for (int i = 1; i <= nrNoduri; ++i) {
                    for (int vecin : lista[i]) {
                        cout << vecin << " ";
                    }
                    cout << endl;
                }
            };

            afisareListaAdiacenta(apm1, graf.getNumarNoduri());
            cout << endl;
            afisareListaAdiacenta(apm2, graf.getNumarNoduri());
        }
    }

};

// ===================================================================================
// 26. Distanta de editare intre 2 cuvinte
// ===================================================================================

int levenshteinFullMatrix(const string& str1,
                        const string& str2)
{
    int m = str1.length();
    int n = str2.length();

    vector<vector<int> > dp(m + 1, vector<int>(n + 1, 0));

    for (int i = 0; i <= m; i++) {
        dp[i][0] = i;
    }

    for (int j = 0; j <= n; j++) {
        dp[0][j] = j;
    }

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            }
            else {
                dp[i][j] = 1
                        + min(

                            // Insert
                            dp[i][j - 1],
                            min(

                                // Remove
                                dp[i - 1][j],

                                // Replace
                                dp[i - 1][j - 1]));
            }
        }
    }

    return dp[m][n];
}

int main() {
    string numeFisier="graf.in";
//    cout << "Introdu numele fisierului pentru citire: ";
//    cin >> numeFisier;

    // ===================================================================================
    // 1. Citire si afisare graf indiferent de tipul lui
    // ===================================================================================
    Graf graf;


   graf.citireDinFisier(numeFisier);

   cout << endl;
   graf.afisareInformatiiGraf();
   cout << endl;
   graf.afisareMatriceAdiacenta();
   cout << endl;
   graf.afisareListaAdiacenta();
   cout << endl;
   graf.afisareListaMuchii();

    // ===================================================================================
    // 2. BFS SI DFS (toate tipurile de grafuri)
    // ===================================================================================

    /*
    int start;
    cout << "Introdu nodul de start pentru BFS si DFS: ";
    cin >> start;

    cout << "\nParcurgere BFS de la nodul " << start << ": ";
    vector<int> bfsRezultat = graf.bfs(start);
    for (int nod : bfsRezultat) {
        cout << nod << " ";
    }
    cout << endl;

    cout << "Parcurgere DFS de la nodul " << start << ": ";
    vector<int> dfsRezultat = graf.dfs(start);
    for (int nod : dfsRezultat) {
        cout << nod << " ";
    }
    cout << endl;
    */

    // ===================================================================================
    // 3. Verificare graf bipartit (graf neorientat)
    // ===================================================================================

    /*
    cout << "\nVerificare graf bipartit: ";
    if (graf.esteBipartit()) {
        cout << "Graful este bipartit.\n";
    } else {
        cout << "Graful NU este bipartit.\n";
    }
     */

    // ===================================================================================
    // 4. Sortarea topologica (graf orientat aciclic)
    // ===================================================================================

    /*
    try {
        cout << "\nSortarea topologica: ";
        vector<int> topSort = graf.sortareTopologica();
        for (int nod : topSort) {
            cout << nod << " ";
        }
        cout << endl;
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
     */

    // ===================================================================================
    // 4. Componente conexe (graf neorientat)
    // ===================================================================================

    /*
    try {
        graf.componenteConexe();
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
    */

    // ===================================================================================
    // 5. Componente tare conexe (graf orientat)
    // ===================================================================================

    /*
    try {
        graf.componenteTareConexeTarjan();
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
     */

    // ===================================================================================
    // 6. Puncte si muchii critice (graf neorientat)
    // ===================================================================================

    /*
    cout << "\nDeterminarea punctelor si muchiilor critice:\n";
    graf.puncteSiMuchiiCritice();
    */

    // ===================================================================================
    // 7. Dijkstra (graf neorientat/orientat cu muchii ponderate)
    // ===================================================================================

    /*
    int start;
    cout << "Introdu nodul de start pentru algoritmul Dijkstra: ";
    cin >> start;

    try {
        vector<int> dist = graf.dijkstra(start);
        cout << "\nDistantele minime de la nodul " << start << ":\n";
        for (int i = 1; i <= graf.getNumarNoduri(); ++i) {
            if (dist[i] == INT_MAX) {
                cout << "Nodul " << i << ": INF\n";
            } else {
                cout << "Nodul " << i << ": " << dist[i] << "\n";
            }
        }
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
     */

    // ===================================================================================
    // 8. Padure (graf neorientat, fara cicluri, fiecare comp conexa este un arbore)
    // ===================================================================================

    /*
    try {
        cout << "\nVerificare daca graful este o padure: ";
        if (graf.estePadure()) {
            cout << "Graful este o padure.\n";
        } else {
            cout << "Graful NU este o padure.\n";
        }
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
     */


    // ===================================================================================
    // 9. Kruskal (graf neorientat cu ponderi pe muchii)
    // ===================================================================================

    /*
    try {
        vector<tuple<int, int, int>> apm = graf.kruskalAPM();
        cout << "\nArborele Partial Minim (APM) rezultat folosind Kruskal:\n";
        for (const auto& [u, v, greutate] : apm) {
            cout << u << " - " << v << " (cost: " << greutate << ")\n";
        }
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
     */

    // ===================================================================================
    // 10. Prim (graf neorientat cu ponderi pe muchii)
    // ===================================================================================

    /*
    try {
        vector<pair<int, int>> apm = graf.primAPM();
        cout << "\nArborele Partial Minim (APM) rezultat folosind Prim:\n";
        for (const auto& [u, v] : apm) {
            cout << u << " - " << v << "\n";
        }
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
     */


    // ===================================================================================
    // 11. Clustering (graf neorientat cu ponderi, se bazeaza pe Kruskal)
    // ===================================================================================

    /*
    int k;
    cout << "Introdu numarul de clustere dorit (k): ";
    cin >> k;

    try {
        vector<vector<int>> clusteri = graf.clustering(k);
        cout << "\nClusterii determinati:\n";
        for (size_t i = 0; i < clusteri.size(); ++i) {
            cout << "Cluster " << i + 1 << ": ";
            for (int nod : clusteri[i]) {
                cout << nod << " ";
            }
            cout << "\n";
        }
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
     */

    // ===================================================================================
    // 12. Drum critic/CPM (graf orientat si ponderat)
    // ===================================================================================

    /*
    try {
        vector<int> drumCritic = graf.criticalPath();
        cout << "\nDrumul critic (nodurile incluse): ";
        for (int nod : drumCritic) {
            cout << nod << " ";
        }
        cout << "\n";
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
     */

    // ===================================================================================
    // 13. Bellman Ford (graf orientat/neorientat cu ponderi pozitive/negative/nule)
    // ===================================================================================

    /*
    int start;
    cout << "Introdu nodul de start pentru Bellman-Ford: ";
    cin >> start;

    try {
        vector<int> dist = graf.bellmanFord(start);
        cout << "\nDistantele minime de la nodul " << start << ":\n";
        for (int i = 1; i <= graf.getNumarNoduri(); ++i) {
            if (dist[i] == INT_MAX) {
                cout << "Nodul " << i << ": INF\n";
            } else {
                cout << "Nodul " << i << ": " << dist[i] << "\n";
            }
        }
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
     */

    // ===================================================================================
    // 14. Floyd-Warshall (graf orientat/neorientat cu ponderi)
    // ===================================================================================

    /*
    graf.floydWarshall();
    */

    // ===================================================================================
    // 15. Flux maxim de cost minim(graf orientat cu ponderi)
    // ===================================================================================

    /*
    int start, end;
    cout << "Introdu nodul sursa si nodul destinatie pentru Flux Maxim de Cost Minim: ";
    cin >> start >> end;

    try {
        auto [fluxTotal, costTotal] = graf.fluxMaxCostMin(start, end);
        cout << "Flux maxim: " << fluxTotal << "\n";
        cout << "Cost minim asociat fluxului maxim: " << costTotal << "\n";
    } catch (const exception &e) {
        cerr << "Eroare: " << e.what() << endl;
    }
     */


    // ===================================================================================
    // 16. Cuplaj maxim in graf bipartit/ Hopcroft-Karp (graf bipartit)
    // ===================================================================================

    /*

    int m = 5; // Numărul de noduri din mulțimea A (presupunem că sunt de la 1 la m)

    cout << "\nCuplaj maxim in graf bipartit:\n";
    int cuplaj = graf.cuplajMaxim(m);
    cout << "Cuplajul maxim este: " << cuplaj << endl;

    */

    // ===================================================================================
    // 17. Constructia unui graf orientat cu secvente de grade de intrare si iesire date
    // ===================================================================================

    /*

    vector<int> inDegree, outDegree;

    Graf graf;
    graf.citireDinFisierGrade("grafpersonalizat.in", inDegree, outDegree);

    if (graf.construiesteGrafOrientat(inDegree, outDegree)) {
        cout << "Graful a fost construit cu succes!\n";
        graf.afisareListaAdiacentaPersonalizata();
        graf.afisareListaMuchiiPersonalizata();
    } else {
        cout << "Nu s-a putut construi graful cu datele din fisier.\n";
    }
    */

    // ===================================================================================
    // 18. Graf eulerian (graf neorientat - grad par, graf conex)
    //                   (graf orientat - gradul de intrare = gradul de iesire)
    // ===================================================================================

    /*
    try {
        if (graf.esteEulerian()) {
            cout << "\nGraful este Eulerian.\n";
        } else {
            cout << "\nGraful NU este Eulerian.\n";
        }
    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
     */

    // ===================================================================================
    // 19. Graf hamiltonian (graf neorientat - exista un ciclu hamiltonian = ciclu care trece o data prin fiecare nod)
    //                      (graf orientat - exista un drum hamiltonian )
    // ===================================================================================

    /*
    try {
        if (graf.esteHamiltonian()) {
            cout << "\nGraful are ciclu Hamiltonian.\n";
        } else {
            cout << "\nGraful NU are ciclu Hamiltonian.\n";
        }
    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
     */

    // ===================================================================================
    // 20. Edmonds Karp (graf orientat cu fiecare muchie avand capacitate pozitiva)
    // ===================================================================================

    /*
    int start, end;
    cout << "Introdu nodul sursa ai nodul destinatie pentru flux maxim: ";
    cin >> start >> end;

    try {
        int fluxMaxim = graf.edmondsKarp(start, end);
        cout << "\nFluxul maxim intre nodul " << start << " si nodul " << end << " este: " << fluxMaxim << "\n";
    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
     */

    // ===================================================================================
    // 21. Kosaraju (graf orientat - se determina comp tare conexe)
    // ===================================================================================

    /*
    try {
        graf.kosaraju();
    } catch (const exception &e) {
        cerr << e.what() << endl;
    }
     */


    // ===================================================================================
    // 22. 6 Colorarea unui graf  (6 culori disponibile ai nodurile adiacente au culori diferite )
    /*
    Pasi:
            1.Atribuim culorile nodurilor unul câte unul.
            2.Pentru fiecare nod, alegem cea mai mică culoare care nu este utilizată de vecinii săi.
            3.Continuăm până când toate nodurile sunt colorate.
    */
    // ===================================================================================

    /*
    try {
        graf.colorare6();
    } catch (const exception& e) {
        cerr << "Eroare: " << e.what() << "\n";
    }
    */

    // ===================================================================================
    // 23. Havel hakimi si constructia unui graf neorientat
    // ===================================================================================

    //graf.construiesteGrafDinSecventa("grafpersonalizat.in");


    return 0;
}

/***
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple> // pentru std::tuple
#include <stack>
#include <queue>
#include <algorithm>
#include <functional>
#include <map>
#include <numeric>
#include <set>

using namespace std;


//-------LEGENDA------------

//    NEORIENTAT = 0 ; ORIENTAT = 1
//    NEPONDERAT = 0 ; PONDERAT = 1



class Graf {
private:
    int nrNoduri, nrMuchii;
    vector<int> destinatii; // Vector pentru a stoca destinațiile
    bool orientat ;  // Indică dacă graful este orientat        NEORIENTAT = 0 ; ORIENTAT = 1
    bool ponderat;  // Indică dacă graful este ponderat        NEPONDERAT = 0 ; PONDERAT = 1
    vector<vector<pair<int, int>>> listaAdiacenta; // Lista de adiacență (vecin, greutate)
    vector<vector<int>> matriceAdiacenta;          // Matricea de adiacență (greutăți)
    vector<tuple<int, int, int>> listaMuchii;      // Lista de muchii (nod1, nod2, greutate)

    void initStructuri() {
        listaAdiacenta = vector<vector<pair<int, int>>>(nrNoduri + 1);
        matriceAdiacenta = vector<vector<int>>(nrNoduri + 1, vector<int>(nrNoduri + 1, 0));
        listaMuchii.clear();
    }
    void eliminaMuchie(int u, int v) {
        // Eliminăm muchia (u, v) din lista de adiacență
        auto it = remove_if(listaAdiacenta[u].begin(), listaAdiacenta[u].end(),
                            [v](const pair<int, int>& p) { return p.first == v; });
        listaAdiacenta[u].erase(it, listaAdiacenta[u].end());

        // Deoarece graful este neorientat, eliminăm și muchia (v, u)
        it = remove_if(listaAdiacenta[v].begin(), listaAdiacenta[v].end(),
                       [u](const pair<int, int>& p) { return p.first == u; });
        listaAdiacenta[v].erase(it, listaAdiacenta[v].end());

        // Eliminăm muchia din lista de muchii
        listaMuchii.erase(remove_if(listaMuchii.begin(), listaMuchii.end(),
                                    [u, v](const tuple<int, int, int>& edge) {
                                        return (get<0>(edge) == u && get<1>(edge) == v) ||
                                               (get<0>(edge) == v && get<1>(edge) == u);
                                    }),
                          listaMuchii.end());
    }
public:
    Graf() : nrNoduri(0), nrMuchii(0), orientat(false), ponderat(false) {}
    Graf(int n) : nrNoduri(n) {}

    int getNumarNoduri() const {
        return nrNoduri;
    }

    // ===================================================================================
    // 1. Citire si afisare graf indiferent de tipul lui
    // ===================================================================================
    void citireDinFisier(const string &numeFisier) {
        ifstream inputFile(numeFisier);
        int k, s;
        if (!inputFile.is_open()) {
            cerr << "Eroare la deschiderea fisierului: " << numeFisier << endl;
            return;
        }

        inputFile >> nrNoduri >> nrMuchii ;
        orientat = true;
        ponderat = false;
        //>> orientat >> ponderat;

        inputFile >> k;
        destinatii.resize(k);

        for (int i = 0; i < k; i++) {
            inputFile >> destinatii[i];
        }

        initStructuri();


        for (int i = 0; i < nrMuchii; ++i) {
            int u, v, greutate = 1; // Greutatea implicită este 1 pentru grafuri neponderate
            inputFile >> u >> v;

            if (ponderat && !(inputFile >> greutate)) {
                cerr << "Eroare: Datele muchiilor sunt incomplete sau incorecte." << endl;
                return;
            }

            // Adaugă muchia în structurile de date
            listaAdiacenta[u].emplace_back(v, greutate);
            matriceAdiacenta[u][v] = greutate;
            listaMuchii.emplace_back(u, v, greutate);

            // Dacă graful nu este orientat, adaugă și muchia inversă
            if (!orientat) {
                listaAdiacenta[v].emplace_back(u, greutate);
                matriceAdiacenta[v][u] = greutate;
            }
        }

        inputFile.close();
        cout << "Datele au fost citite cu succes din fisierul: " << numeFisier << endl;
    }

    void afisareMatriceAdiacenta() {
        cout << "Matricea de adiacenta:" << endl;
        for (int i = 1; i <= nrNoduri; ++i) {
            for (int j = 1; j <= nrNoduri; ++j) {
                cout << matriceAdiacenta[i][j] << " ";
            }
            cout << endl;
        }
    }

    void afisareListaAdiacenta() {
        cout << "Lista de adiacenta:" << endl;
        for (int i = 1; i <= nrNoduri; ++i) {
            cout << "Nod " << i << ":";
            for (auto [vecin, greutate]: listaAdiacenta[i]) {
                cout << " (" << vecin << ", " << greutate << ")";
            }
            cout << endl;
        }
    }

    void afisareListaMuchii() {
        cout << "Lista de muchii:" << endl;
        for (auto [u, v, greutate]: listaMuchii) {
            cout << "(" << u << ", " << v << ", " << greutate << ")\n";
        }
    }

    void afisareInformatiiGraf() {
        cout << "Numarul de noduri: " << nrNoduri << endl;
        cout << "Numarul de muchii: " << nrMuchii << endl;
        //cout << "Graful este " << (orientat ? "orientat" : "neorientat") << "." << endl;
        //cout << "Graful este " << (ponderat ? "ponderat" : "neponderat") << "." << endl;
    }

    // EX 1
    // Funcție pentru determinarea Arborelui Parțial Minim (APM) folosind algoritmul Prim
    vector<pair<int, int>> primAPM() {
        vector<bool> inAPM(nrNoduri + 1, false);           // Marcăm nodurile incluse în APM
        vector<int> cost(nrNoduri + 1, INT_MAX);           // Costul pentru a adăuga fiecare nod
        vector<int> parinte(nrNoduri + 1, -1);             // Părintele fiecărui nod în APM
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> minHeap; // Min-Heap pentru selectarea nodului cu cost minim
        vector<pair<int, int>> apm;                        // Lista muchiilor din APM

        cost[1] = 0;                                       // Pornim de la nodul 1
        minHeap.push({0, 1});

        while (!minHeap.empty()) {
            int nod = minHeap.top().second;
            minHeap.pop();

            if (inAPM[nod]) continue;                      // Dacă nodul este deja în APM, trecem mai departe
            inAPM[nod] = true;

            if (parinte[nod] != -1) {
                apm.emplace_back(parinte[nod], nod);       // Adăugăm muchia la APM
            }

            for (auto [vecin, greutate] : listaAdiacenta[nod]) {
                if (!inAPM[vecin] && greutate < cost[vecin]) {
                    cost[vecin] = greutate;               // Actualizăm costul
                    parinte[vecin] = nod;                 // Setăm părintele
                    minHeap.push({cost[vecin], vecin});   // Adăugăm vecinul în heap
                }
            }
        }

        return apm;
    }

    void gasesteDoiArboriDistincti(Graf& graf) {
        // Obținem primul arbore folosind Prim
        vector<pair<int, int>> apm1 = graf.primAPM();

        if (apm1.size() != graf.getNumarNoduri() - 1) {
            cout << "Nu" << endl;
            return;  // Dacă nu există un arbore de acoperire, returnăm direct
        }

        vector<pair<int, int>> apm2;
        bool existaDoiArbori = false;

        // Încercăm să construim un al doilea arbore eliminând câte o muchie diferită
        for (auto muchie : apm1) {
            Graf copieGraf = graf;

            // Eliminăm muchia curentă
            copieGraf.eliminaMuchie(muchie.first, muchie.second);

            vector<pair<int, int>> potentialApm = copieGraf.primAPM();

            // Verificăm dacă am găsit un arbore diferit de primul
            if (potentialApm.size() == graf.getNumarNoduri() - 1 && potentialApm != apm1) {
                apm2 = potentialApm;
                existaDoiArbori = true;
                break;
            }
        }

        if (!existaDoiArbori) {
            cout << "Nu" << endl;
        } else {
            cout << "Da" << endl;

            // Funcție pentru afișarea unui arbore sub formă de listă de adiacență
            auto afisareListaAdiacenta = [](const vector<pair<int, int>>& arbore, int nrNoduri) {
                vector<vector<int>> lista(nrNoduri + 1);
                for (auto [u, v] : arbore) {
                    lista[u].push_back(v);
                    lista[v].push_back(u);
                }
                for (int i = 1; i <= nrNoduri; ++i) {
                    for (int vecin : lista[i]) {
                        cout << vecin << " ";
                    }
                    cout << endl;
                }
            };

            afisareListaAdiacenta(apm1, graf.getNumarNoduri());
            cout << endl;
            afisareListaAdiacenta(apm2, graf.getNumarNoduri());
        }
    }

    // EXERCITIU 2
    // Funcție pentru sortarea topologică (valabilă doar pentru grafuri orientate aciclice)
    vector<int> sortareTopologica() {

        if (!orientat) {
            throw runtime_error("Sortarea topologica este valabila doar pentru grafuri orientate.");
        }

        vector<int> inDegree(nrNoduri + 1, 0); // Gradul intern pentru fiecare nod

        // Calculăm gradul intern pentru fiecare nod
        for (int i = 1; i <= nrNoduri; ++i) {
            for (auto [vecin, _] : listaAdiacenta[i]) {
                inDegree[vecin]++;
            }
        }

        // Adăugăm în coadă nodurile cu grad intern 0
        queue<int> q;
        for (int i = 1; i <= nrNoduri; ++i) {
            if (inDegree[i] == 0) {
                q.push(i);
            }
        }

        vector<int> ordine;
        while (!q.empty()) {
            int nod = q.front();
            q.pop();
            ordine.push_back(nod);

            // Reducem gradul intern al vecinilor și îi adăugăm în coadă dacă devin 0
            for (auto [vecin, _] : listaAdiacenta[nod]) {
                inDegree[vecin]--;
                if (inDegree[vecin] == 0) {
                    q.push(vecin);
                }
            }
        }

        // Dacă nu am parcurs toate nodurile, înseamnă că graful are cicluri
        if (ordine.size() != nrNoduri) {
            throw runtime_error("Graful are cicluri, sortarea topologica nu este posibila.");
        }

        return ordine;
    }

    void drumMaximSpreDestinatie() {
        vector<int> topSort = sortareTopologica();
        vector<int> lungimeMaxima(nrNoduri + 1, INT_MIN);
        vector<int> parinte(nrNoduri + 1, -1);

        for (int nod : topSort) {
            if (lungimeMaxima[nod] == INT_MIN) {lungimeMaxima[nod] = 0;}

            for (auto [vecin, _] : listaAdiacenta[nod]) {
                if (lungimeMaxima[vecin] < lungimeMaxima[nod] + 1) {
                    lungimeMaxima[vecin] = lungimeMaxima[nod] + 1;
                    parinte[vecin] = nod;
                }
            }
        }

        int nodFinal = -1, maxLungime = -1;
        for (int dest : destinatii) {
            if (lungimeMaxima[dest] > maxLungime) {
                maxLungime = lungimeMaxima[dest];
                nodFinal = dest;
            }
        }

        if (nodFinal == -1 || maxLungime == 0) {
            cout << "0\n";
            return;
        }

        cout << maxLungime << '\n';

        vector<int> drum;
        while (nodFinal != -1) {
            drum.push_back(nodFinal);
            nodFinal = parinte[nodFinal];
        }

        reverse(drum.begin(), drum.end());
        for (int nod : drum) {
            cout << nod << " ";
        }
        cout << '\n';
    }



    // ===================================================================================
    // 20. Edmonds Karp (graf orientat cu fiecare muchie avand capacitate pozitiva)
    // ===================================================================================

    void adaugaMuchie(int u, int v, int capacitate) {
        listaMuchii.push_back({u, v, capacitate});
    }


    int edmondsKarp(int start, int end) {
        vector<vector<int>> capacitate(nrNoduri + 1, vector<int>(nrNoduri + 1, 0));

        // Inițializăm matricea de capacitate folosind lista de muchii
        for (const auto &[u, v, greutate] : listaMuchii) {
            capacitate[u][v] += greutate; // Adăugăm capacitatea pe muchie
        }

        vector<vector<int>> flux(nrNoduri + 1, vector<int>(nrNoduri + 1, 0));
        vector<int> parinte(nrNoduri + 1); // Părintele fiecărui nod în calea găsită de BFS
        int fluxMaxim = 0;

        while (true) {
            fill(parinte.begin(), parinte.end(), -1); // Inițializăm părinții
            queue<int> q;
            q.push(start);
            parinte[start] = start;

            // BFS pentru găsirea unei căi de augmentare
            while (!q.empty() && parinte[end] == -1) {
                int nod = q.front();
                q.pop();
                for (int vecin = 1; vecin <= nrNoduri; ++vecin) {
                    if (parinte[vecin] == -1 && capacitate[nod][vecin] - flux[nod][vecin] > 0) {
                        parinte[vecin] = nod;
                        q.push(vecin);
                        if (vecin == end) break; // Am ajuns la destinație
                    }
                }
            }

            // Dacă nu există o cale de augmentare, ieșim
            if (parinte[end] == -1) break;

            // Determinăm fluxul maxim posibil pe calea găsită
            int fluxAdaugat = INT_MAX;
            for (int nod = end; nod != start; nod = parinte[nod]) {
                int prev = parinte[nod];
                fluxAdaugat = min(fluxAdaugat, capacitate[prev][nod] - flux[prev][nod]);
            }

            // Actualizăm fluxul pe calea găsită
            for (int nod = end; nod != start; nod = parinte[nod]) {
                int prev = parinte[nod];
                flux[prev][nod] += fluxAdaugat;
                flux[nod][prev] -= fluxAdaugat;
            }

            fluxMaxim += fluxAdaugat; // Adăugăm fluxul calea găsită
        }

        return fluxMaxim;
    }

};

int main() {
    string numeFisier="graf.in";

    // ===================================================================================
    // 1. Citire si afisare graf indiferent de tipul lui
    // ===================================================================================
    Graf graf;


    graf.citireDinFisier(numeFisier);

    /*
    cout << endl;
    graf.afisareInformatiiGraf();
    cout << endl;
    graf.afisareMatriceAdiacenta();
    cout << endl;
    graf.afisareListaAdiacenta();
    cout << endl;
    graf.afisareListaMuchii();*/

    // ===================================================================================
    // 10. Prim (graf neorientat cu ponderi pe muchii)
    // ===================================================================================


//    try {
//        vector<pair<int, int>> apm = graf.primAPM();
//        cout << "\nArborele Partial Minim (APM) rezultat folosind Prim:\n";
//        for (const auto& [u, v] : apm) {
//            cout << u << " - " << v << "\n";
//        }
//    } catch (const exception& e) {
//        cerr << e.what() << endl;
//    }

    //ex 1
    /*Fie  un graf neorientat cu  noduri, numerotate de la 1 la n, si m muchii. Sa se determine daca  are cel putin doi arbori
     *partiali distincti (= care difera prin cel putin o muchie). In caz afirmativ, sa se afiseze doi arbori partiali distincti
     *ai lui G.*/
    //graf.gasesteDoiArboriDistincti(graf);

    //ex 2
    /*Fie G un graf orientat aciclic (fara circuite) cu n noduri (numerotate de la  la ) si m arce. Se considera un sir  s de valori
     *naturale de lungime k cu elementele , numit sir de destinatii. Sa se determine un drum de lungime maxima in  G care se termina
     *in una dintre cele k destinatii.
     */
    //graf.drumMaximSpreDestinatie();


    //ex3
    /*Tara AF a fost lovita de o pandemie numita RESTANTA. Astfel, cercetatorii au descoperit un vaccin care sa rezolve aceasta problema
     *si l-au distribuit in m centre de vaccinare (numerotate de la 1 la m). Cele  n persoane din tara (numerotate de la 1 la n) vor sa se vaccineze,
     *doar ca acestea nu pot merge la orice centru, ci fiecare are o lista de centre la care poate ajunge. Totodata, pentru fiecare dintre cele m
     *centre se cunoaste numarul de doze de vaccin (adica numarul maxim de persoane care se pot vaccina la acel centru). Sa se determine numarul
     *minim de persoane care nu se pot vaccina, folosind un algoritm bazat pe algoritmul Ford-Fulkerson / Edmonds Karp.
     *
     *input: n,m,m valori(cate doze sunt la fiecare centru), n linii unde avem muchiile care sunt asa nr de centre la care poate merge persoana i si indicii acestor centre
     */
/***
    int n, m;
    cin >> n >> m;

    int sursa = 0, destinatie = n + m + 1;
    Graf graf(n + m + 2);//pentru ca nodurile sunt cele n persoane si cele m centre pe care le numerotam de la n+1 la n+m

    // Citirea numărului de doze disponibile în fiecare centru
    for (int i = 1; i <= m; i++) {
        int doze;
        cin >> doze;
        //muchia asta reprezinta cate persoane poate primi centrul de vaccinare
        graf.adaugaMuchie(n + i, destinatie, doze);
    }

    // Citirea informațiilor despre persoanele și centrele accesibile
    for (int i = 1; i <= n; i++) {//pt fiecare persoana
        int k;
        cin >> k;//se citeste k,nr de centre la care poate merge
        for (int j = 0; j < k; j++) {
            int centru;
            cin >> centru;
            //pt fiecare centru accesibil se adauga o muchie de la persoana catre centru cu capacitatea 1-pt ca
            //fiecare persoana poate fi vaccinata o singura data
            //deci fluxul maxim permis pe acea muchie e 1
            graf.adaugaMuchie(i, n + centru, 1);
        }
        // Conectăm persoana la sursă
        graf.adaugaMuchie(sursa, i, 1);
    }

    //aici bagam flux,pana acum practic ne am construit graful pe care bagam flux
    // fluxul ne va zice cate persoane se pot vaccina
    int fluxMaxim = graf.edmondsKarp(sursa, destinatie);

    // Numărul minim de persoane care NU se pot vaccina
    cout << n - fluxMaxim << endl;


    }
**/




/***

/***
 *  PROBLEMA 1- ASCENDENTI SI DESCENDENTI
    //sa se determine ascendentii si descendentii nodului c din arbore
#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <set>
using namespace std;

void cautaAscendenti(int radacina, int target, unordered_map<int, vector<int>>& tree, set<int>& visisted) {
    unordered_map<int,int> tati;
    queue<int> q;

    if (radacina == target) {
        cout << "0" << endl;
        return;
    }

    q.push(radacina);
    tati[radacina] = -1;

    while(!q.empty()) {
        int aux = q.front();
        q.pop();
        for(int vecin : tree[aux]) {
            if(tati.find(vecin) == tati.end()) {
                 tati[vecin] = aux;
                 q.push(vecin);
            }
        }
    }

    vector<int> ascendenti;
    int c = target;
    while(tati[c] != -1) {
        ascendenti.push_back(tati[c]);
        c = tati[c];
    }

    for(int a : ascendenti) {
        cout << a << " ";
        visisted.insert(a);
    }
    cout<<endl;

}


void cautaDescendenti(int target, unordered_map<int, vector<int>>& tree , set<int>& visited) {
    set<int> descendenti;
    queue<int> q;
    q.push(target);
    visited.insert(target);
    while(!q.empty()) {
        int c = q.front();
        q.pop();
        for(int vecin : tree[c]) {
            if(descendenti.find(vecin) == descendenti.end() && vecin != c && visited.find(vecin) == visited.end()) {
                descendenti.insert(vecin);
                visited.insert(vecin);
                q.push(vecin);
            }
        }
    }
    if(descendenti.size() == 0) {
        cout << "0" << endl;
    }
    else {
        for(int d : descendenti) {
            cout << d << ' ';
        }
        cout << endl;
    }
}


int main() {

    int n, r, c;
    cin >> n >> r >> c;

    unordered_map<int, vector<int>> tree;
    set <int> visited;

    for (int i = 0; i < n - 1; i++) {
        int x, y;
        cin >> x >> y;
        tree[x].push_back(y);
        tree[y].push_back(x);
    }

    cautaAscendenti(r, c, tree, visited);
    cautaDescendenti(c, tree, visited);


    return 0;
}


***/



////PROBLEMA 3
///
///retea de transport cu sursa 1 destinatia n
/// a) valoarea maxima a unui flux
///  b) crestem cu p arcele care pleaca din sursa si afisam diferenta dintre noul si vechiul flux
/***

#include <iostream>
#include <fstream>
#include <vector>
#include <tuple> // pentru std::tuple
#include <queue>
#include <algorithm>
#include <climits>

using namespace std;


//-------LEGENDA------------

 //   NEORIENTAT = 0 ; ORIENTAT = 1
 //   NEPONDERAT = 0 ; PONDERAT = 1


class Graf {
private:
    int nrNoduri, nrMuchii;
    bool orientat;
    bool ponderat;
    vector<vector<pair<int, int>>> listaAdiacenta;
    vector<vector<int>> matriceAdiacenta;
    vector<tuple<int, int, int>> listaMuchii;

    void initStructuri() {
        listaAdiacenta = vector<vector<pair<int, int>>>(nrNoduri + 1);
        matriceAdiacenta = vector<vector<int>>(nrNoduri + 1, vector<int>(nrNoduri + 1, 0));
        listaMuchii.clear();
    }
public:
    Graf() : nrNoduri(0), nrMuchii(0), orientat(false), ponderat(false) {}
    Graf(int n) : nrNoduri(n){}
    int getNumarNoduri() const {
        return nrNoduri;
    }


    void adaugaMuchie(int u, int v, int capacitate) {
        listaMuchii.push_back({u, v, capacitate});
    }

   vector<tuple<int, int, int>>& obtineListaMuchii()  {
        return listaMuchii;
    }


    int edmondsKarp(int start, int end) {
        vector<vector<int>> capacitate(nrNoduri + 1, vector<int>(nrNoduri + 1, 0));


        for (const auto &[u, v, greutate] : listaMuchii) {
            capacitate[u][v] += greutate;
        }

        vector<vector<int>> flux(nrNoduri + 1, vector<int>(nrNoduri + 1, 0));
        vector<int> parinte(nrNoduri + 1);
        int fluxMaxim = 0;

        while (true) {
            fill(parinte.begin(), parinte.end(), -1);
            queue<int> q;
            q.push(start);
            parinte[start] = start;


            while (!q.empty() && parinte[end] == -1) {
                int nod = q.front();
                q.pop();
                for (int vecin = 1; vecin <= nrNoduri; ++vecin) {
                    if (parinte[vecin] == -1 && capacitate[nod][vecin] - flux[nod][vecin] > 0) {
                        parinte[vecin] = nod;
                        q.push(vecin);
                        if (vecin == end) break;
                    }
                }
            }

            if (parinte[end] == -1) break;

            int fluxAdaugat = INT_MAX;
            for (int nod = end; nod != start; nod = parinte[nod]) {
                int prev = parinte[nod];
                fluxAdaugat = min(fluxAdaugat, capacitate[prev][nod] - flux[prev][nod]);
            }


            for (int nod = end; nod != start; nod = parinte[nod]) {
                int prev = parinte[nod];
                flux[prev][nod] += fluxAdaugat;
                flux[nod][prev] -= fluxAdaugat;
            }

            fluxMaxim += fluxAdaugat;
        }

        return fluxMaxim;
    }




};
int main() {
    int n,m,p, sursa, destinatie;
    cin >>n >> m >> p;
    Graf graf(n);
    sursa = 1;
    destinatie = n;

    for(int i = 0; i < m; i++) {
        int x, y, z;
        cin >> x >> y >> z ;
        graf.adaugaMuchie(x, y, z);
    }

    //pana aici e punctul a
    int fluxA = graf.edmondsKarp(sursa, destinatie);
    cout << fluxA<< endl;

    //punctul b
    int maxDif = 0;
    for (size_t i = 0; i < graf.obtineListaMuchii().size(); i++) {
        auto& muchie = graf.obtineListaMuchii()[i];

        int x = get<0>(muchie);
        int z = get<2>(muchie);

        if(x == sursa) {
            get<2>(muchie) += p;

            int fluxB = graf.edmondsKarp(sursa, destinatie);

            maxDif = max(maxDif, fluxB - fluxA);

            get<2>(muchie) = z;
        }
    }

    cout << maxDif <<endl;

    return 0;
}
***/



/***
problema 2 cum as face o asa in criza de timp,corecta partial
/// muchii dintr-o lista fac sau nu parte din cel putin un apm


#include <iostream>
#include <fstream>
#include <vector>
#include <tuple> // pentru std::tuple
#include <queue>
#include <algorithm>
#include <climits>
#include <functional>

using namespace std;


//-------LEGENDA------------

 //   NEORIENTAT = 0 ; ORIENTAT = 1
 //   NEPONDERAT = 0 ; PONDERAT = 1


class Graf {
private:
    int nrNoduri, nrMuchii;
    bool orientat;
    bool ponderat;
    vector<vector<pair<int, int>>> listaAdiacenta;
    vector<vector<int>> matriceAdiacenta;
    vector<tuple<int, int, int>> listaMuchii;

    void initStructuri() {
        listaAdiacenta = vector<vector<pair<int, int>>>(nrNoduri + 1);
        matriceAdiacenta = vector<vector<int>>(nrNoduri + 1, vector<int>(nrNoduri + 1, 0));
        listaMuchii.clear();
    }
public:
    Graf() : nrNoduri(0), nrMuchii(0), orientat(false), ponderat(false) {}
    Graf(int n) : nrNoduri(n){}
    int getNumarNoduri() const {
        return nrNoduri;
    }


    void adaugaMuchie(int u, int v, int capacitate) {
        listaMuchii.push_back({u, v, capacitate});
    }

   vector<tuple<int, int, int>>& obtineListaMuchii()  {
        return listaMuchii;
    }



    vector<tuple<int, int, int>> kruskalAPM() {
        vector<tuple<int, int, int>> apm;
        vector<int> parinte(nrNoduri + 1);


        for (int i = 1; i <= nrNoduri; ++i) {
            parinte[i] = i;
        }


        std::function<int(int)> gasesteRadacina = [&](int nod) -> int {
            if (parinte[nod] != nod) {
                parinte[nod] = gasesteRadacina(parinte[nod]);
            }
            return parinte[nod];
        };


        sort(listaMuchii.begin(), listaMuchii.end(), [](const auto& a, const auto& b) {
            return get<2>(a) < get<2>(b);
        });


        for (const auto& [u, v, greutate] : listaMuchii) {
            int radU = gasesteRadacina(u);
            int radV = gasesteRadacina(v);

            if (radU != radV) {
                apm.emplace_back(u, v, greutate);
                parinte[radU] = radV;
            }
        }

        return apm;
    }



};
int main() {
    int n,m,k;
    cin >>n >> m ;
    Graf graf(n);


    for(int i = 0; i < m; i++) {
        int x, y, z;
        cin >> x >> y >> z ;
        graf.adaugaMuchie(x, y, z);
    }


    cin >> k;
    vector<int> muchiiDeCautat(k);
    for (int i = 0; i < k; i++) {
        cin >> muchiiDeCautat[i];
    }

    vector<tuple<int, int, int>> apm = graf.kruskalAPM();


    for (int i = 0; i < k; i++) {
        int idx = muchiiDeCautat[i] - 1;
        auto [x, y, z] = graf.obtineListaMuchii()[idx];

        bool gasit = false;
        for (const auto& [u, v, greutate] : apm) {
            if (min(x, y) == u && max(x, y) == v && z == greutate) {
                gasit = true;
                break;
            }
        }

        if (gasit) {
            cout << "Da" << endl;
        } else {
            cout << "Nu" << endl;
        }
    }



    return 0;
}


***/



///PROBLEMA 1
///Fie G un graf orientat pondarat cu n noduri si m muchii.
///Sa se determine nodurile cu distanta cea mai mare fata de nodul 1.
///Distanta dintre doua noduri este definita ca cel mai scurt drum dintre acestea.
///
/***
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple> // pentru std::tuple
#include <queue>
#include <algorithm>
#include <functional>
#include <climits>


using namespace std;


//-------LEGENDA------------

//   NEORIENTAT = 0 ; ORIENTAT = 1
//   NEPONDERAT = 0 ; PONDERAT = 1



class Graf {
private:
    int nrNoduri, nrMuchii;
    bool orientat;
    bool ponderat;
    vector<vector<pair<int, long long>>> listaAdiacenta;


    void initStructuri() {
        listaAdiacenta = vector<vector<pair<int, long long>>>(nrNoduri + 1);
    }
public:
    Graf() : nrNoduri(0), nrMuchii(0) {}

    int getNumarNoduri() const {
        return nrNoduri;
    }

    void citireGraf() {

        cin >> nrNoduri >> nrMuchii ;
        initStructuri();

        for (int i = 0; i < nrMuchii; ++i) {
            int u, v;
            long long greutate;
            cin >> u >> v >> greutate;
            listaAdiacenta[u].emplace_back(v, greutate);

        }

    }


    void afisareListaAdiacenta() {
        cout << "Lista de adiacenta:" << endl;
        for (int i = 1; i <= nrNoduri; ++i) {
            cout << "Nod " << i << ":";
            for (auto [vecin, greutate]: listaAdiacenta[i]) {
                cout << " (" << vecin << ", " << greutate << ")";
            }
            cout << endl;
        }
    }




    vector<long long> dijkstra(int start) {

        vector<long long> dist(nrNoduri + 1, INT_MAX);
        priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<>> minHeap;
        dist[start] = 0;
        minHeap.push({0, start});

        while (!minHeap.empty()) {
            long long distCurenta = minHeap.top().first;
            int nodCurent = minHeap.top().second;
            minHeap.pop();

            if (distCurenta > dist[nodCurent]) continue;

            for (auto [vecin, greutate] : listaAdiacenta[nodCurent]) {
                if (dist[nodCurent] + greutate < dist[vecin]) {
                    dist[vecin] = dist[nodCurent] + greutate;
                    minHeap.push({dist[vecin], vecin});
                }
            }
        }

        return dist;
    }

};
int main() {
    Graf graf;

    graf.citireGraf();

    vector<long long> dist = graf.dijkstra(1);

    long long dist_max = *max_element(dist.begin() + 1, dist.end());

    vector<int> noduriMaxime;
    for (int i = 1; i <= graf.getNumarNoduri(); ++i) {
        if (dist[i] == dist_max) {
            noduriMaxime.push_back(i);
        }
    }

    sort(noduriMaxime.begin(), noduriMaxime.end());

    for (int nod : noduriMaxime) {
        cout << nod << " ";
    }

    return 0;
}

***/


///PROBLEMA 3
///Harta unui joc este formata din n camere si m portale. La inceputul fiecarei zile
///te afli in camera 1 si vrei sa ajungi in camera n (dupa ce ajungi in camera  se
///termina ziua si te intorci in camera 1). Un portal poate fi folosit cel mult o
///data de-a lungul intregului joc. Sa se determine numarul maxim de zile care poate
///fi jucat acest joc?
/***
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple> // pentru std::tuple
#include <climits>
#include <queue>
#include <algorithm>


using namespace std;


//-------LEGENDA------------

    //NEORIENTAT = 0 ; ORIENTAT = 1
   // NEPONDERAT = 0 ; PONDERAT = 1
//


class Graf {
private:
    int nrNoduri, nrMuchii;
    vector<vector<pair<int, int>>> listaAdiacenta;
    vector<vector<int>> matriceAdiacenta;
    vector<tuple<int, int, int>> listaMuchii;

    void initStructuri() {
        listaAdiacenta = vector<vector<pair<int, int>>>(nrNoduri + 1);
        matriceAdiacenta = vector<vector<int>>(nrNoduri + 1, vector<int>(nrNoduri + 1, 0));
        listaMuchii.clear();
    }
public:
    Graf() : nrNoduri(0), nrMuchii(0) {}

    int getNumarNoduri() const {
        return nrNoduri;
    }
    void citire() {

        cin >> nrNoduri >> nrMuchii ;

        initStructuri();

        for (int i = 0; i < nrMuchii; ++i) {
            int u, v, greutate = 1 ;
            cin >> u >> v;

            listaAdiacenta[u].emplace_back(v, greutate);
            matriceAdiacenta[u][v] = greutate;
            listaMuchii.emplace_back(u, v, greutate);
        }
    }

    void afisareMatriceAdiacenta() {
        cout << "Matricea de adiacenta:" << endl;
        for (int i = 1; i <= nrNoduri; ++i) {
            for (int j = 1; j <= nrNoduri; ++j) {
                cout << matriceAdiacenta[i][j] << " ";
            }
            cout << endl;
        }
    }

    void afisareListaAdiacenta() {
        cout << "Lista de adiacenta:" << endl;
        for (int i = 1; i <= nrNoduri; ++i) {
            cout << "Nod " << i << ":";
            for (auto [vecin, greutate]: listaAdiacenta[i]) {
                cout << " (" << vecin << ", " << greutate << ")";
            }
            cout << endl;
        }
    }

    void afisareListaMuchii() {
        cout << "Lista de muchii:" << endl;
        for (auto [u, v, greutate]: listaMuchii) {
            cout << "(" << u << ", " << v << ", " << greutate << ")\n";
        }
    }

    void afisareInformatiiGraf() {
        cout << "Numarul de noduri: " << nrNoduri << endl;
        cout << "Numarul de muchii: " << nrMuchii << endl;
    }


    int edmondsKarp(int start, int end) {
        vector<vector<int>> capacitate(nrNoduri + 1, vector<int>(nrNoduri + 1, 0));

        for (const auto &[u, v, greutate] : listaMuchii) {
            capacitate[u][v] += greutate;
        }

        vector<vector<int>> flux(nrNoduri + 1, vector<int>(nrNoduri + 1, 0));
        vector<int> parinte(nrNoduri + 1);
        int fluxMaxim = 0;

        while (true) {
            fill(parinte.begin(), parinte.end(), -1);
            queue<int> q;
            q.push(start);
            parinte[start] = start;

            while (!q.empty() && parinte[end] == -1) {
                int nod = q.front();
                q.pop();
                for (int vecin = 1; vecin <= nrNoduri; ++vecin) {
                    if (parinte[vecin] == -1 && capacitate[nod][vecin] - flux[nod][vecin] > 0) {
                        parinte[vecin] = nod;
                        q.push(vecin);
                        if (vecin == end) break;
                    }
                }
            }

            if (parinte[end] == -1) break;

            int fluxAdaugat = INT_MAX;
            for (int nod = end; nod != start; nod = parinte[nod]) {
                int prev = parinte[nod];
                fluxAdaugat = min(fluxAdaugat, capacitate[prev][nod] - flux[prev][nod]);
            }

            for (int nod = end; nod != start; nod = parinte[nod]) {
                int prev = parinte[nod];
                flux[prev][nod] += fluxAdaugat;
                flux[nod][prev] -= fluxAdaugat;
            }

            fluxMaxim += fluxAdaugat;
        }

        return fluxMaxim;
    }

};
int main() {

    Graf graf;
    graf.citire();

    int sursa = 1, destinatie = graf.getNumarNoduri();
    cout<<graf.edmondsKarp(sursa,destinatie)<<endl;

    return 0;

}

***/


///PROBLEMA 2
/***
 *Fie  un graf neorientat ponderat cu n noduri si m muchii.
 *Costul fiecarei muchii este un numar din multimea 0 1. Sa se raspunda la
 *intrebari de forma: "exista un drum de la x la y mergand doar pe muchii de cost c?".
//----------------------------------------EXERCITIU 2------------------------------------------
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
using namespace std;

bool bfs(int x, int y, int n, const vector<vector<int>>& graf) {
    vector<bool> vizitat(n + 1, false);
    queue<int> q;
    q.push(x);
    vizitat[x] = true;

    while (!q.empty()) {
        int nod = q.front();
        q.pop();

        if (nod == y) return true;  // Am ajuns la destinație

        for (int vecin : graf[nod]) {
            if (!vizitat[vecin]) {
                vizitat[vecin] = true;
                q.push(vecin);
            }
        }
    }
    return false;  // Nu s-a găsit drum între x și y
}

int main(){
    int n,m, Q;
    cin >> n >> m;
    // Construim două grafuri separate pentru muchii de cost 0 și 1
    vector<vector<int>> graf0(n + 1);
    vector<vector<int>> graf1(n + 1);

    for (int i = 0; i < m; ++i) {
        int x, y, c;
        cin >> x >> y >> c;
        if (c == 0) {
            graf0[x].push_back(y);
            graf0[y].push_back(x);
        } else {
            graf1[x].push_back(y);
            graf1[y].push_back(x);
        }
    }

    cin >> Q;
    for(int i=1; i<=Q; i++) {
        int x, y, c;
        cin >> x >> y >> c;
        if (c == 0) {
            cout << (bfs(x, y, n, graf0) ? "DA\n" : "NU\n");
        } else {
            cout << (bfs(x, y, n, graf1) ? "DA\n" : "NU\n");
        }
    }
    return 0;
}

***/


//---------------------------------- EXERCITIU 2 (sort top, dist maxime plecand din k noduri)  ---------------------
//Fie G un graf orientat aciclic (fara circuite) cu  noduri (numerotate de la 1 la n) si m arce.
// Se considera un sir s de valori naturale de lungime  k cu elementele , numit sir de surse.
// Sa se determine un drum de lungime maxima in G care porneste din una dintre cele k surse.

//-------------------------------------   EXERCITIU 3(flux, nr vacc nefolosite) ----------------------------------------
//Tara AF a fost lovita de o pandemie numita RESTANTA.
// Astfel, cercetatorii au descoperit un vaccin care sa rezolve aceasta problema si l-au distribuit in m centre
// (numerotate de la 1 la m) de vaccinare. Cele n persoane (numerotate de la 1 la n)
// din tara vor sa se vaccineze doar ca acestea nu pot merge la orice centru, ci fiecare are o lista de
// centre la care poate ajunge. Totodata, pentru fiecare dintre cele m centre se cunoaste numarul de doze de
// vaccin din centru (adica numarul maxim de persoane care se pot vaccina la acel centru). Sa se determine
// cate vaccinuri raman nefolosite daca se vaccineaza un numar maxim de persoane,
// folosind un algoritm bazat pe algoritmul Ford-Fulkerson / Edmonds Karp.


//------------------EXERCITIU 2-----------------------------------
/*#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits.h>

using namespace std;

int n, m, k;
vector<vector<int>> graf;
vector<int> surse;

// Funcție pentru citirea datelor de la tastatură
void citireDate() {
    cin >> n >> m >> k;

    graf.resize(n + 1);
    surse.resize(k);

    for (int i = 0; i < k; ++i) {
        cin >> surse[i];
    }

    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        graf[u].push_back(v);
    }
}

// Funcție pentru sortarea topologică folosind algoritmul Kahn (BFS)
vector<int> sortareTopologica() {
    vector<int> inDegree(n + 1, 0);

    // Calculăm gradul de intrare pentru fiecare nod
    for (int u = 1; u <= n; ++u) {
        for (int v : graf[u]) {
            inDegree[v]++;
        }
    }

    queue<int> q;
    for (int i = 1; i <= n; ++i) {
        if (inDegree[i] == 0) {
            q.push(i);
        }
    }

    vector<int> topoOrder;
    while (!q.empty()) {
        int node = q.front();
        q.pop();
        topoOrder.push_back(node);

        for (int neighbor : graf[node]) {
            inDegree[neighbor]--;
            if (inDegree[neighbor] == 0) {
                q.push(neighbor);
            }
        }
    }
    return topoOrder;
}

// Funcție pentru calcularea drumului maxim care pornește din surse
void drumMaximDinSurse() {
    vector<int> topoOrder = sortareTopologica();
    vector<int> dist(n + 1, INT_MIN);
    vector<int> parinte(n + 1, -1);

    // Inițializăm distanțele pentru surse
    for (int src : surse) {
        dist[src] = 0;
    }

    // Parcurgem topologic și calculăm distanțele maxime
    for (int node : topoOrder) {
        if (dist[node] == INT_MIN) continue;  // Dacă nodul nu a fost atins, îl sărim

        for (int neighbor : graf[node]) {
            if (dist[neighbor] < dist[node] + 1) {
                dist[neighbor] = dist[node] + 1;
                parinte[neighbor] = node;
            }
        }
    }

    // Determinăm lungimea maximă și nodul unde se termină
    int lungimeMaxima = 0, nodFinal = -1;
    for (int i = 1; i <= n; ++i) {
        if (dist[i] > lungimeMaxima) {
            lungimeMaxima = dist[i];
            nodFinal = i;
        }
    }

    cout << lungimeMaxima << endl;

    // Reconstruim drumul maxim
    vector<int> drum;
    while (nodFinal != -1) {
        drum.push_back(nodFinal);
        nodFinal = parinte[nodFinal];
    }

    reverse(drum.begin(), drum.end());
    for (int nod : drum) {
        cout << nod << " ";
    }
    cout << endl;
}

int main() {
    citireDate();
    drumMaximDinSurse();
    return 0;
}
*/

//-------------------- EXERCITIU 3 --------------------------------------------------------
/*#include <iostream>
#include <vector>
#include <queue>
#include <climits>

using namespace std;

const int INF = INT_MAX;
int n, m;  // n = numărul de persoane, m = numărul de centre
vector<vector<int>> capacitate, flux;
vector<int> parinte;

// Funcție pentru a adăuga muchii în rețeaua de flux
void adaugaMuchie(int u, int v, int cap) {
    capacitate[u][v] += cap;
}

// Implementarea algoritmului Edmonds-Karp pentru flux maxim
bool bfs(int sursa, int destinatie) {
    fill(parinte.begin(), parinte.end(), -1);
    queue<int> q;
    q.push(sursa);
    parinte[sursa] = sursa;

    while (!q.empty()) {
        int nod = q.front();
        q.pop();

        for (int vecin = 0; vecin <= n + m + 1; ++vecin) {
            if (parinte[vecin] == -1 && capacitate[nod][vecin] - flux[nod][vecin] > 0) {
                parinte[vecin] = nod;
                if (vecin == destinatie) {
                    return true;  // Am ajuns la destinație
                }
                q.push(vecin);
            }
        }
    }
    return false;
}

int edmondsKarp(int sursa, int destinatie) {
    int fluxMaxim = 0;

    while (bfs(sursa, destinatie)) {
        int fluxAdaugat = INF;

        // Determinăm fluxul minim de-a lungul drumului găsit
        for (int nod = destinatie; nod != sursa; nod = parinte[nod]) {
            int prev = parinte[nod];
            fluxAdaugat = min(fluxAdaugat, capacitate[prev][nod] - flux[prev][nod]);
        }

        // Actualizăm fluxurile
        for (int nod = destinatie; nod != sursa; nod = parinte[nod]) {
            int prev = parinte[nod];
            flux[prev][nod] += fluxAdaugat;
            flux[nod][prev] -= fluxAdaugat;
        }

        fluxMaxim += fluxAdaugat;
    }

    return fluxMaxim;
}

int main() {
    cin >> n >> m;

    int sursa = 0, destinatie = n + m + 1;
    capacitate.assign(n + m + 2, vector<int>(n + m + 2, 0));
    flux.assign(n + m + 2, vector<int>(n + m + 2, 0));
    parinte.resize(n + m + 2);

    // Citirea dozelor disponibile în centre
    for (int i = 1; i <= m; i++) {
        int doze;
        cin >> doze;
        adaugaMuchie(n + i, destinatie, doze);  // Conectăm centrele la destinație
    }

    // Citirea restricțiilor pentru fiecare persoană
    for (int i = 1; i <= n; i++) {
        int k;
        cin >> k;  // Numărul de centre accesibile pentru persoana i
        for (int j = 0; j < k; j++) {
            int centru;
            cin >> centru;
            adaugaMuchie(i, n + centru, 1);  // Capacitatea maximă pentru fiecare conexiune este 1
        }
        adaugaMuchie(sursa, i, 1);  // Conectăm fiecare persoană la sursă
    }

    // Calcularea fluxului maxim
    int fluxMaxim = edmondsKarp(sursa, destinatie);

    // Afișarea numărului de vaccinuri rămase nefolosite
    int totalDoze = 0;
    for (int i = 1; i <= m; i++) {
        totalDoze += capacitate[n + i][destinatie];
    }

    cout << totalDoze - fluxMaxim << endl;

    return 0;
}
*/



// ---------------------------------  EXERCITIU 3(flux maxim, creste cu p ) ----------------------------------------------
//O retea de transport N cu sursa 1 si destinatia n
//a)Sa se afiseze val max a unui Flux in N
//b) Se da un nr p. Se creste cu p capacitatea unui arc care INTRA IN DESTINATIE
//Fie Fp val max a fluxului in noua retea. Det. cu cat poate creste val fluxului maxim daca marim
//cu p cap.unui arc care iese din sursa? (Fp-F de la pct a)


/*#include <iostream>
#include <vector>
#include <tuple>
#include <queue>
#include <climits>

using namespace std;

// Implementarea algoritmului Edmonds-Karp pentru flux maxim
int edmondsKarp(int start, int end, int n, vector<tuple<int, int, int>> graf) {
    vector<vector<int>> capacitate(n + 1, vector<int>(n + 1, 0));

    // Inițializăm matricea de capacitate folosind lista de muchii
    for (const auto &[u, v, greutate] : graf) {
        capacitate[u][v] += greutate;  // Adăugăm capacitatea pe muchie
    }

    vector<vector<int>> flux(n + 1, vector<int>(n + 1, 0));
    vector<int> parinte(n + 1);  // Părintele fiecărui nod în calea găsită de BFS
    int fluxMaxim = 0;

    while (true) {
        fill(parinte.begin(), parinte.end(), -1);  // Inițializăm părinții
        queue<int> q;
        q.push(start);
        parinte[start] = start;

        // BFS pentru găsirea unei căi de augmentare
        while (!q.empty() && parinte[end] == -1) {
            int nod = q.front();
            q.pop();
            for (int vecin = 1; vecin <= n; ++vecin) {
                if (parinte[vecin] == -1 && capacitate[nod][vecin] - flux[nod][vecin] > 0) {
                    parinte[vecin] = nod;
                    q.push(vecin);
                    if (vecin == end) break;  // Am ajuns la destinație
                }
            }
        }

        // Dacă nu există o cale de augmentare, ieșim
        if (parinte[end] == -1) break;

        // Determinăm fluxul maxim posibil pe calea găsită
        int fluxAdaugat = INT_MAX;
        for (int nod = end; nod != start; nod = parinte[nod]) {
            int prev = parinte[nod];
            fluxAdaugat = min(fluxAdaugat, capacitate[prev][nod] - flux[prev][nod]);
        }

        // Actualizăm fluxul pe calea găsită
        for (int nod = end; nod != start; nod = parinte[nod]) {
            int prev = parinte[nod];
            flux[prev][nod] += fluxAdaugat;
            flux[nod][prev] -= fluxAdaugat;
        }

        fluxMaxim += fluxAdaugat;  // Adăugăm fluxul calea găsită
    }

    return fluxMaxim;
}

int main() {
    int n, m, p;
    cin >> n >> m >> p;

    vector<tuple<int, int, int>> graf;
    vector<tuple<int, int, int>> muchiiInDestinatie;

    for (int i = 0; i < m; i++) {
        int x, y, c;
        cin >> x >> y >> c;
        graf.emplace_back(x, y, c);

        // Stocăm muchiile care intră în nodul destinație (nodul n)
        if (y == n) {
            muchiiInDestinatie.emplace_back(x, y, c);
        }
    }

    // Calculează fluxul maxim inițial
    int flux_initial = edmondsKarp(1, n, n, graf);
    cout << flux_initial << "\n";

    // Testăm creșterea capacității fiecărei muchii care intră în destinație
    int max_diferenta = 0;
    for (auto &[u, v, c] : muchiiInDestinatie) {
        vector<tuple<int, int, int>> grafNou = graf;

        // Creștem capacitatea muchiei curente cu p
        for (auto &muchie : grafNou) {
            if (get<0>(muchie) == u && get<1>(muchie) == v && get<2>(muchie) == c) {
                get<2>(muchie) += p;
                break;
            }
        }

        int fluxNou = edmondsKarp(1, n, n, grafNou);
        max_diferenta = max(max_diferenta, fluxNou - flux_initial);
    }

    cout << max_diferenta << "\n";

    return 0;
}
*/











////inceput de baza
/***
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple> // pentru std::tuple
#include <stack>
#include <queue>
#include <algorithm>
#include <functional>
#include <map>
#include <numeric>
#include <set>

using namespace std;


//-------LEGENDA------------

//    NEORIENTAT = 0 ; ORIENTAT = 1
//    NEPONDERAT = 0 ; PONDERAT = 1
//


class Graf {
private:
    int nrNoduri, nrMuchii;
    bool orientat;  // Indică dacă graful este orientat        NEORIENTAT = 0 ; ORIENTAT = 1
    bool ponderat;  // Indică dacă graful este ponderat        NEPONDERAT = 0 ; PONDERAT = 1
    vector<vector<pair<int, int>>> listaAdiacenta; // Lista de adiacență (vecin, greutate)
    vector<vector<int>> matriceAdiacenta;          // Matricea de adiacență (greutăți)
    vector<tuple<int, int, int>> listaMuchii;      // Lista de muchii (nod1, nod2, greutate)

    void initStructuri() {
        listaAdiacenta = vector<vector<pair<int, int>>>(nrNoduri + 1);
        matriceAdiacenta = vector<vector<int>>(nrNoduri + 1, vector<int>(nrNoduri + 1, 0));
        listaMuchii.clear();
    }
public:
    Graf() : nrNoduri(0), nrMuchii(0), orientat(false), ponderat(false) {}

    int getNumarNoduri() const {
        return nrNoduri;
    }

    // ===================================================================================
    // 1. Citire si afisare graf indiferent de tipul lui
    // ===================================================================================
    void citireDinFisier(const string &numeFisier) {
        ifstream inputFile(numeFisier);
        if (!inputFile.is_open()) {
            cerr << "Eroare la deschiderea fisierului: " << numeFisier << endl;
            return;
        }

        inputFile >> nrNoduri >> nrMuchii >> orientat >> ponderat;
        if (inputFile.fail()) {
            cerr << "Eroare: Nu s-au putut citi numarul de noduri, muchii, orientarea sau tipul ponderat din fisier."
                 << endl;
            return;
        }

        initStructuri();

        for (int i = 0; i < nrMuchii; ++i) {
            int u, v, greutate = 1; // Greutatea implicită este 1 pentru grafuri neponderate
            inputFile >> u >> v;

            if (ponderat && !(inputFile >> greutate)) {
                cerr << "Eroare: Datele muchiilor sunt incomplete sau incorecte." << endl;
                return;
            }

            // Adaugă muchia în structurile de date
            listaAdiacenta[u].emplace_back(v, greutate);
            matriceAdiacenta[u][v] = greutate;
            listaMuchii.emplace_back(u, v, greutate);

            // Dacă graful nu este orientat, adaugă și muchia inversă
            if (!orientat) {
                listaAdiacenta[v].emplace_back(u, greutate);
                matriceAdiacenta[v][u] = greutate;
            }
        }

        inputFile.close();
        cout << "Datele au fost citite cu succes din fisierul: " << numeFisier << endl;
    }

    void afisareMatriceAdiacenta() {
        cout << "Matricea de adiacenta:" << endl;
        for (int i = 1; i <= nrNoduri; ++i) {
            for (int j = 1; j <= nrNoduri; ++j) {
                cout << matriceAdiacenta[i][j] << " ";
            }
            cout << endl;
        }
    }

    void afisareListaAdiacenta() {
        cout << "Lista de adiacenta:" << endl;
        for (int i = 1; i <= nrNoduri; ++i) {
            cout << "Nod " << i << ":";
            for (auto [vecin, greutate]: listaAdiacenta[i]) {
                cout << " (" << vecin << ", " << greutate << ")";
            }
            cout << endl;
        }
    }

    void afisareListaMuchii() {
        cout << "Lista de muchii:" << endl;
        for (auto [u, v, greutate]: listaMuchii) {
            cout << "(" << u << ", " << v << ", " << greutate << ")\n";
        }
    }

    void afisareInformatiiGraf() {
        cout << "Numarul de noduri: " << nrNoduri << endl;
        cout << "Numarul de muchii: " << nrMuchii << endl;
        cout << "Graful este " << (orientat ? "orientat" : "neorientat") << "." << endl;
        cout << "Graful este " << (ponderat ? "ponderat" : "neponderat") << "." << endl;
    }
};
int main() {
    string numeFisier="graf.in";
//    cout << "Introdu numele fisierului pentru citire: ";
//    cin >> numeFisier;

    // ===================================================================================
    // 1. Citire si afisare graf indiferent de tipul lui
    // ===================================================================================
    Graf graf;


    graf.citireDinFisier(numeFisier);

    cout << endl;
    graf.afisareInformatiiGraf();
    cout << endl;
    graf.afisareMatriceAdiacenta();
    cout << endl;
    graf.afisareListaAdiacenta();
    cout << endl;
    graf.afisareListaMuchii();
}
 ***/