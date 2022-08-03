#include "api.hpp"
#include <string>
#include <map>
#include <iostream>
#include <algorithm>


bool cmp(std::pair<std::string, int> &a, std::pair<std::string, int> &b) {
    return a.second < b.second;
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

std::string dfa2re(DFA &d) {
    Alphabet alp = d.get_alphabet();
    std::vector<std::pair<std::string, int>> state_mp;
    std::map<std::string, int> state_tr;
    std::set<std::string> str_set = d.get_states();
    std::set<std::string>::iterator it;
    for (it = str_set.begin(); it != str_set.end(); it++) {
        for (const char *i = alp.begin(); i < alp.end(); i++) {
            if (d.has_trans(*it, *i)) {
                state_tr[*it] += 1;
                state_tr[d.get_trans(*it, *i)] += 1;
            }
        }
    }
    std::map<std::pair<std::string, std::string>, std::string> trans;
    for (it = str_set.begin(); it != str_set.end(); it++) {
        for (const char *i = alp.begin(); i < alp.end(); i++) {
            if (d.has_trans(*it, *i)) {
                trans[std::make_pair(*it, d.get_trans(*it, *i))] += *i;
                trans[std::make_pair(*it, d.get_trans(*it, *i))] += '|';
            }
        }
        if (d.is_final(*it)) {
            trans[std::make_pair(*it, "final")] += '$';
        }
        if (d.is_initial(*it)) {
            trans[std::make_pair("start", *it)] += '$';
        }
    }
    std::map<std::pair<std::string, std::string>, std::string>::iterator its;
    for (its = trans.begin(); its != trans.end(); its++) {
        if (its->second[its->second.size() - 1] == '|') {
            its->second.pop_back();
        }
    }

    std::map<std::string, int>::iterator it2;
    for (it2 = state_tr.begin(); it2 != state_tr.end(); it2++) {
        state_mp.push_back(std::make_pair(it2->first, it2->second));
    }
    std::sort(state_mp.begin(), state_mp.end(), cmp);
    for (int j = 0; j < state_mp.size(); j++) {
        std::set<std::string> int_states;
        std::set<std::string> ext_states;
        std::string cicle;
        std::map<std::pair<std::string, std::string>, std::string> del;
        for (its = trans.begin(); its != trans.end(); its++) {
            if (its->first.first == state_mp[j].first && its->first.second != state_mp[j].first) {
                ext_states.insert(its->first.second);
                del.insert(*its);
            }
            if (its->first.first == state_mp[j].first && its->first.second == state_mp[j].first) {
                cicle = its->second;
                del.insert(*its);
            }
            if (its->first.first != state_mp[j].first && its->first.second == state_mp[j].first) {
                int_states.insert(its->first.first);
                del.insert(*its);
            }
        }
        std::set<std::string>::iterator ite;
        std::set<std::string>::iterator iti;
        for (iti = int_states.begin(); iti != int_states.end(); iti++) {
            for (ite = ext_states.begin(); ite != ext_states.end(); ite++) {
                if (trans[std::make_pair(*iti, *ite)].size() > 0) {
                    if (cicle.size() > 0) {
                        std::string tmp = "|((" + trans[std::make_pair(*iti, state_mp[j].first)] + ")(" + cicle + ")*(" + trans[std::make_pair(state_mp[j].first, *ite)] + "))";
                        trans[std::make_pair(*iti, *ite)] += tmp;
                    } else {
                        std::string tmp = "|((" + trans[std::make_pair(*iti, state_mp[j].first)] + ")(" + trans[std::make_pair(state_mp[j].first, *ite)] + "))";
                        trans[std::make_pair(*iti, *ite)] += tmp;
                    }
                } else {
                    if (cicle.size() > 0) {
                        std::string tmp = "((" + trans[std::make_pair(*iti, state_mp[j].first)] + ")(" + cicle + ")*(" + trans[std::make_pair(state_mp[j].first, *ite)] + "))";
                        trans[std::make_pair(*iti, *ite)] += tmp;
                    } else {
                        std::string tmp = "((" + trans[std::make_pair(*iti, state_mp[j].first)] + ")(" + trans[std::make_pair(state_mp[j].first, *ite)] + "))";
                        trans[std::make_pair(*iti, *ite)] += tmp;
                    }
                }
            }
        }
        std::map<std::pair<std::string, std::string>, std::string>::iterator its1;
        for (its1 = del.begin(); its1 != del.end(); its1++) {
            trans.erase(std::make_pair(its1->first.first, its1->first.second));
        }
    }
    trans[std::make_pair("start", "final")] = ReplaceAll(trans[std::make_pair("start", "final")], "$", "");
    return trans[std::make_pair("start", "final")];
}
