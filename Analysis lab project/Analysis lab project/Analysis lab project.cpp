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
        if (G.find(first) != G.end()) {
            G.at(first).insert(word);
        }
        else
            G.insert({ first, second });
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
    g.open("Number of impressions.csv");
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
        N.insert({ first, second });
    }
    g.close();
    g.open("Number of clicks.csv");
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
        C.insert({ first, second });
    }
    g.close();
    pagerank();
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
                if (in == 1) {
                    c++;
                }
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
    
    /*fstream g;
    string line, word;
    fstream inp;
    inp.open("nNumber of impressions.csv", ios::out | ios::app);
    g.open("Number of impressions.csv");
    int counter = 0;
    int second;
    while (!g.eof()) {
        second = 0;
        counter = 0;
        bool c = 0;
        getline(g, line);
        stringstream s(line);
        string first;
        while (getline(s, word, ',')) {
            if (counter == 0) {
                first = word;
                for (auto& i : valid) {
                    if (word == i) {
                        c = 1;
                    }
                }
                    counter++;
            }
            else {
                second = stoi(word);
                if (c == 1) {
                    second++;
                    c = 0;
                }
            }
            inp << word << "," << to_string(second) << endl;
        }
    }
    remove("Number of impressions.csv");
    rename("nNumber of impressions.csv", "Number of impressions.csv");
    remove("nNumber of impressions.csv");
    g.close();
    inp.close();*/

    for (auto& i : valid) {
        double x = (0.4 * norm.find(i)->second) + ((1 - ((0.1 * N.find(i)->second) / (1 + 0.1 * N.find(i)->second))) * norm.find(i)->second + ((0.1 * N.find(i)->second) / (1 + 0.1 * N.find(i)->second)) * (C.find(i)->second / N.find(i)->second) * 100) * 0.4;
        score.insert({x , i});
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
    bool n = 1;
    int x = 0, k = 0;
    string s;
    graph g;
    bool l = 0;
    int z = 0;
    int f = 0;
    while (n == 1) {
        cout << "welcome!" << endl;
        cout << "what would you like to do?" << endl;
        cout << "1) new search" << endl;
        cout << "2) exit" << endl;
        cout << "type in your choice: " << endl;
        cin >> x;
        while (x == 1) {
            if (!l) {
                cout << "enter search:" << endl;
                cin >> s;
            }
            cout << "search results:" << endl;
            cout << "1) would you like a webpage to open" << endl;
            cout << "2) new search" << endl;
            cout << "3) exit" << endl;
            cout << "type in your choice: " << endl;
            cin >> f;
            g.search(s);


            if (f == 2) {
                    x = 1;
            }
            else if (f == 3) {
                break;
            }
            else  if (f == 1) {
                cout << "choose website" << endl;
                cin >> z;
                if (z > g.sites.size()) {
                    cout << "invalid input, re-enter" << endl;
                    cin >> z;
                }
                if (z <= g.sites.size()) {
                    cout << "you are viewing" << g.sites[z - 1] << endl;
                }
               /* fstream gr;
                string line, word;
                fstream inp;
                inp.open("nNumber of clicks.csv", ios::out | ios::app);
                gr.open("Number of clicks.csv");
                int counter = 0;
                int second;
                while (!gr.eof()) {
                    second = 0;
                    counter = 0;
                    bool c = 0;
                    getline(gr, line);
                    stringstream s(line);
                    string first;
                    while (getline(s, word, ',')) {
                        if (counter == 0) {
                            first = word;
                                if (word == g.sites[z]) {
                                    c = 1;
                                }
                            counter++;
                        }
                        else {
                            second = stoi(word);
                            if (c == 1) {
                                second++;
                                c = 0;
                            }
                        }
                        inp << word << "," << to_string(second) << endl;
                    }
                }
                remove("Number of clicks.csv");
                rename("nNumber of clicks.csv", "Number of clicks.csv");
                remove("nNumber of clicks.csv");
                gr.close();
                inp.close();*/
                cout << "would you like to :" << endl;
                cout << "1) back to search results" << endl;
                cout << "2) new search" << endl;
                cout << "3) exit" << endl;
                cout << "type in your choice: " << endl;
                if (k == 2) {
                    x = 1;
                    l = 0;
                }
                if (k == 3) {
                    x = 0;
                    f = 0;
                    z = 0;
                    break;
                }
                if (k == 1) {
                    x = 1;
                    l = 1;
                }
            }
        }
        if (x == 2) {
            cout << "thank you for using our service" << endl;
            break;
        }
        else {
            cout << "not a valid number enter again" << endl;
            cin >> x;
            n = 1;
        }
    }
}