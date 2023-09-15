#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <list>
#include <map>
#include <stdexcept> 
#include <sstream>
#include <stack>
#include <unordered_set>
#include <algorithm>
#include <stdio.h>
using namespace std;
class graph {
private:
    map<string, unordered_set<string>> G;
    map<string, list<string>> K;
    map<string, int> N, C;
    map<string, double> rank, norm;
    map<double, string> score;
public:
    graph();
    void search(string s);
    void pagerank();
    vector<string> sites;
    void erase();
};

graph::graph() {
    fstream g;
    string line, word;
    int counter = 0;
    g.open("Web graph.csv", ios::in);
    while (!g.eof()) {
        counter = 0;
        getline(g, line);
        stringstream s(line);
        string first;
        unordered_set<string> second;
        while (getline(s, word, ',')) {
            if (counter == 0) {
                first = word;
                counter++;
            }
            else {
                second.insert(word);
            }
        }
        if (first != "") {
            if (G.find(first) != G.end()) {
                G.at(first).insert(word);
            }
            else
                G.insert({ first, second });
        }
    }
    g.close();
    g.open("Keyword.csv", ios::in);
    while (!g.eof()) {
        counter = 0;
        getline(g, line);
        stringstream s(line);
        string first;
        list<string> f;
        vector<string> second;
        while (getline(s, word, ',')) {
            if (counter == 0) {
                f.push_back(word);
                first = word;
                counter++;
            }
            else {
                second.push_back(word);
            }
        }
        for (int i = 0; i < second.size(); i++) {
            if (K.find(second[i]) != K.end()) {
                if (second[i] != "")
                    K.at(second[i]).push_back(first);
            }
            else {
                if (second[i] != "")
                    K.insert({ second[i], f });
            }
        }
    }
    g.close();
    g.open("Number of impressions.csv", ios::in);
    while (!g.eof()) {
        counter = 0;
        getline(g, line);
        stringstream s(line);
        string first;
        int second;
        while (getline(s, word, ',')) {
            if (counter == 0) {
                first = word;
                counter++;
            }
            else {
                second = stoi(word);
            }
        }
        if (first != "")
        N.insert({ first, second });
    }
    g.close();
    g.open("Number of clicks.csv", ios::in);
    while (!g.eof()) {
        counter = 0;
        getline(g, line);
        stringstream s(line);
        string first;
        int second;
        while (getline(s, word, ',')) {
            if (counter == 0) {
                first = word;
                counter++;
            }
            else {
                second = stoi(word);
            }
        }
        if (first != "")
        C.insert({ first, second });
    }
    g.close();
    pagerank();
}

void graph::erase() {
    sites.clear();
    rank.clear();
    score.clear();
}

void graph::pagerank() {
    for (auto& i : G) {
        rank.insert({ i.first, 1.0 / 30.0 });
    }
    double max, min;
    for (int i = 0; i < 30; i++) {
        max = 0; min = 1.0;
        for (auto& j : G) {
            double sum = 0;
            for (auto& z : G) {
                if (z.second.find(j.first) != z.second.end()) {
                    auto it = rank.find(z.first);
                    sum += it->second / z.second.size();
                }
            }
            rank.find(j.first)->second = sum;
            if (sum < min) { min = sum; }
            if (sum > max) { max = sum; }
        }
    }
    for (auto& i : rank) {
        norm.insert({ i.first, ((i.second - min) / (max - min)) });
    }
}

void graph::search(string s) {
    stack<string> w;
    stack<string> k;
    string x = "", q = "";
    bool prev = 0;
    bool in = 0;
    int c = 0, c2 = 0;
    if (s[s.size() - 1] == ' ') {
        while (s[s.size() - 1] == ' ') {
            s.pop_back();
        }
    }
    for (int i = 0; i < s.size(); i++) {
        if (s[i] == '"' && in == 1) {
            q += ' ' + x;
            c2--;
            for (int i = 0; i < c; i++) {
                w.pop();
                if (i < c2) {
                    k.pop();
                }
            }
            w.push(q);
            q = "", x = "";
            in = 0;
            c = 0; c2 = 0;
        }
        else if (s[i] == '"' && (i == 0 || s[i - 1] == ' ') && i != s.size()) {
            in = 1;
        }
        else if (s[i] != ' ' && i != s.size()) {
            x += s[i];
            if (i == s.size() - 1) {
                w.push(x);
            }
        }
        else if (x != "") {
            if (in == 1) {
                if (c != 0) q += ' ';
                q += x;
                c++;
            }
            if (x == "AND" || x == "OR") {
                k.push(x);
                c2++;
                prev = 0;
            }
            else {
                if (prev == 1) {
                    k.push("OR");
                    if (in == 1) {
                        c2++;
                    }
                }
                w.push(x);
                prev = 1;
            }
            x = "";
        }
    }
    c = int(w.size());
    unordered_set<string> valid;
    unordered_set<string> current;
    unordered_set<string> previous, pre;
    bool v = 0;
    if (c == 1) {
        auto it = K.find(w.top());
        w.pop();
        for (auto& it2 : it->second) {
            valid.insert(it2);
        }
    }
    else {
        for (int i = 0; i <= c; i++) {
            if (i == c) {
                for (auto& j : previous) {
                    valid.insert(j);
                }
            }
            else if (i < c) {
                auto it = K.find(w.top());
                w.pop();
                for (auto& it2 : it->second) {
                    current.insert(it2);
                }
                if (i != 0) {
                    if (k.size() != 0 && k.top() == "OR") {
                        for (auto& j : previous) {
                            valid.insert(j);
                        }
                        v = 0;
                    }
                    else if (k.size() != 0 && k.top() == "AND") {
                        pre = previous;
                        previous.clear();
                        for (auto& j : current) {
                            if (pre.find(j) != pre.end()) {
                                previous.insert(j);
                            }
                        }
                        v = 1;
                    }
                    k.pop();
                }
                if (!v) previous = current;
                current.clear();
            }
        }
    }

    for (auto& i : valid) {
        double x = (0.4 * norm.find(i)->second) + ((1 - ((0.1 * N.find(i)->second) / (1 + 0.1 * N.find(i)->second))) * norm.find(i)->second + ((0.1 * N.find(i)->second) / (1 + 0.1 * N.find(i)->second)) * (C.find(i)->second / N.find(i)->second) * 100) * 0.4;
        score.insert({ x , i });
    }
    int i = 1;
    for (auto it = score.rbegin(); it != score.rend(); ++it)
    {
        cout << i << ") " << it->second << endl;
        i++;
        sites.push_back(it->second);
    }

}

bool cmp(pair<string, double>& a, pair<string, double>& b)
{
    return a.second > b.second;
}

int main() {
    int n = 1;
    int x = 0, k = 0;
    string s;
    graph g;
    int l = 0;
    int z = 0, f = 0;;
    cout << "welcome!" << endl;
    cout << "what would you like to do?" << endl;
    cout << "1) new search" << endl;
    cout << "2) exit" << endl;
    cout << "type in your choice: " << endl;
    cin >> x;
    cin.ignore();
    if (x == 1) {
        cout << "enter search:" << endl;
        getline(cin, s);
    }
    while (n == 1) {
        if (x == 1) {
            if (!l && f) {
                cout << "welcome!" << endl;
                cout << "what would you like to do?" << endl;
                cout << "1) new search" << endl;
                cout << "2) exit" << endl;
                cout << "type in your choice: " << endl;
                cin >> x;
                cin.ignore();
                cout << "enter search:" << endl;
                getline(cin, s);
            }
            f++;
            cout << "search results:" << endl;

            g.search(s);

            cout << "1) would you like a webpage to open" << endl << "2) new search" << endl << "3) exit" << endl << "type in your choice:" << endl;
            cin >> k;
            while (k > 3 || k < 1) {
                cout << "invalid, renter number: ";
                cin >> k;
                cout << endl;
            }
            if (k == 2) {
                l = 0;
                g.erase();
                continue;
            }
            else if (k == 3) {
                x = 2;
            }
            else  if (k == 1) {
                cout << "choose website" << endl;
                cin >> z;
                if (z > g.sites.size()) {
                    cout << "invalid input, re-enter" << endl;
                    cin >> z;
                }
                if (z <= g.sites.size()) {
                    cout << "you are viewing: " << g.sites[z - 1] << endl;
                }
                cout << "would you like to :" << endl;
                cout << "1) back to search results" << endl;
                cout << "2) new search" << endl;
                cout << "3) exit" << endl;
                cout << "type in your choice: " << endl;
                cin >> k;
                while (k > 3 || k < 1) {
                    cout << "invalid, renter number: ";
                    cin >> k;
                    cout << endl;
                }
                if (k == 2) {
                    l = 0;
                    g.erase();
                    continue;
                }
                if (k == 3) {
                    x = 2;
                }
                if (k == 1) {
                    l = 1;
                }
            }
        }
        if (x == 2) {
            cout << "thank you for using our service" << endl;
            return 0;
        }
        else if (x != 1) {
            cout << "not a valid number enter again" << endl;
            x = 1;
            n = 1;
        }
    }
    return 0;
}