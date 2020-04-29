#include <iostream>
#include <fstream>
#include <cstring>
#include <queue>
#include <set>

using namespace std;

const int dim = 50;

struct NFA {
    int nrStari = 0, m = 0, q0 = 0, k = 0, nrTranzitii = 0;
    set<int> tranzitii[dim][dim];
    set<int> stariFinale;
    char alfabet[dim] = "";
};

struct DFA {
    int nrStari = 0, m = 0, q0 = 0, k = 0, nrTranzitii = 0;
    set<int> stariFinale;
    int tranzitii[dim][dim] = {{}}, stareMax = 0;
    char alfabet[dim] = "";
};

struct compus {
    int nume = 0;
    set<int> componente = {};
};

int cautCaracter(char c, char v[]) {
    for (int i = 0; i < strlen(v); ++i)
        if (v[i] == c)
            return i;
    return -1;
}

void initializare(DFA &a) {
    int i, j;
    for (i = 0; i < dim; ++i)
        for (j = 0; j < dim; ++j)
            a.tranzitii[i][j] = -1;
}

int getNume(compus stariCompuse[], int nrStariCompuse, const compus &stare) {
    int i;
    for (i = 1; i <= nrStariCompuse; ++i)
        if (stariCompuse[i].componente == stare.componente)
            return stariCompuse[i].nume;
    return -1;
}

compus cautStareCompusa(compus stariCompuse[], int nrStariCompuse, int nume) {
    int i;
    compus stare;
    stare.nume = -1;
    for (i = 1; i <= nrStariCompuse; ++i)
        if (stariCompuse[i].nume == nume)
            return stariCompuse[i];
    return stare;
}

void citire(NFA &a) {
    ifstream fin("NFA.in");
    int i, stare1, stare2, poz;
    char tranzitie;
    fin >> a.nrStari >> a.m;
    for (i = 0; i < a.m; ++i)
        fin >> a.alfabet[i];
    fin >> a.q0 >> a.k;
    for (i = 1; i <= a.k; ++i) {
        int x;
        fin >> x;
        a.stariFinale.insert(x);
    }
    fin >> a.nrTranzitii;
    for (i = 1; i <= a.nrTranzitii; ++i) {
        fin >> stare1 >> tranzitie >> stare2;
        poz = cautCaracter(tranzitie, a.alfabet);
        a.tranzitii[stare1][poz].insert(stare2);
    }
    fin.close();
}

void afisare(DFA a) {
    int i, j;
    set<int>::iterator k;
    bool afisat;
    cout << a.nrStari << "\n";
    cout << a.m << "\n";
    cout << a.alfabet << "\n";
    cout << a.q0 << "\n";
    cout << a.k << "\n";
    for (k = a.stariFinale.begin(); k != a.stariFinale.end(); ++k)
        cout << *k << " ";
    cout << "\n\n";
    for (i = 0; i < a.stareMax + 1; ++i) {
        afisat = false;
        for (j = 0; j < a.m; ++j) {
            if (a.tranzitii[i][j] != -1) {
                cout << a.tranzitii[i][j] << " ";
                a.nrTranzitii++;
                afisat = true;
            }
        }
        if (afisat) cout << "\n";
    }
    cout << "\n" << a.nrTranzitii << "\n";
    for (i = 0; i < a.stareMax + 1; ++i)
        for (j = 0; j < a.m; ++j)
            if (a.tranzitii[i][j] != -1)
                cout << i << " " << a.alfabet[j] << " " << a.tranzitii[i][j] << "\n";
}


void NFAtoDFA(const NFA a, DFA &b) {
    queue<int> coada;
    coada.push(a.q0);
    int stare, i, vizitat[a.nrStari], nrStariCompuse = 0, cautare;
    compus stariCompuse[dim], stareNoua;
    stareNoua.nume = 0;
    set<int>::iterator j, k;
    b.q0 = a.q0;
    strcpy(b.alfabet, a.alfabet);
    b.m = a.m;
    for (i = 0; i < a.nrStari; ++i)
        vizitat[i] = 0;
    vizitat[a.q0] = 1;
    while (!coada.empty()) {
        stare = coada.front();
        coada.pop();
        b.nrStari++;
        if (stare < a.nrStari) {
            for (i = 0; i < a.m; ++i) {
                for (j = a.tranzitii[stare][i].begin(); j != a.tranzitii[stare][i].end(); ++j) {
                    stareNoua.componente.insert(*j);
                    stareNoua.nume = *j;
                }
                if (stareNoua.componente.size() == 1) {
                    if (vizitat[stareNoua.nume] == 0) {
                        vizitat[stareNoua.nume] = 1;
                        coada.push(stareNoua.nume);
                    }
                    b.tranzitii[stare][i] = stareNoua.nume;
                } else if (!stareNoua.componente.empty()) {
                    cautare = getNume(stariCompuse, nrStariCompuse, stareNoua);
                    if (cautare == -1) {
                        stareNoua.nume = nrStariCompuse + a.nrStari;
                        stariCompuse[++nrStariCompuse] = stareNoua;
                        coada.push(stareNoua.nume);
                        b.tranzitii[stare][i] = stareNoua.nume;
                    } else
                        b.tranzitii[stare][i] = cautare;
                }
                stareNoua.componente.clear();
                stareNoua.nume = 0;
            }
        } else {
            compus stareCurenta;
            stareCurenta = cautStareCompusa(stariCompuse, nrStariCompuse, stare);
            for (i = 0; i < a.m; ++i) {
                for (j = stareCurenta.componente.begin(); j != stareCurenta.componente.end(); ++j) {
                    for (k = a.tranzitii[*j][i].begin(); k != a.tranzitii[*j][i].end(); ++k) {
                        stareNoua.componente.insert(*k);
                        stareNoua.nume = *k;
                    }
                }
                if (stareNoua.componente.size() == 1) {
                    if (vizitat[stareNoua.nume] == 0) {
                        vizitat[stareNoua.nume] = 1;
                        coada.push(stareNoua.nume);
                    }
                    b.tranzitii[stare][i] = stareNoua.nume;
                } else if (!stareNoua.componente.empty()) {
                    cautare = getNume(stariCompuse, nrStariCompuse, stareNoua);
                    if (cautare == -1) {
                        stareNoua.nume = nrStariCompuse + a.nrStari;
                        stariCompuse[++nrStariCompuse] = stareNoua;
                        coada.push(stareNoua.nume);
                        b.tranzitii[stare][i] = stareNoua.nume;
                    } else
                        b.tranzitii[stare][i] = cautare;
                }
                stareNoua.componente.clear();
                stareNoua.nume = 0;
            }
        }
    }
    b.stareMax = stariCompuse[nrStariCompuse].nume;
    for(j = a.stariFinale.begin(); j != a.stariFinale.end(); ++j)
        if(vizitat[*j])
            b.stariFinale.insert(*j);
    for(i = 1; i <= nrStariCompuse; ++i)
        for(j = stariCompuse[i].componente.begin(); j != stariCompuse[i].componente.end(); ++j)
            if(a.stariFinale.count(*j))
                b.stariFinale.insert(stariCompuse[i].nume);
    b.k = b.stariFinale.size();
}

int main() {
    NFA a;
    DFA b;
    initializare(b);
    citire(a);
    NFAtoDFA(a, b);
    afisare(b);
    return 0;
}