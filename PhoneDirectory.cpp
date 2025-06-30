#include<bits/stdc++.h>
using namespace std;
#define int long long


class PhoneDirectory_key_number {
private:
    struct Node {
        array<int, 10> next{}; // for digits 0-9
        string name;           // non-empty if number exists
        Node() {}
    };

    vector<Node> trie;

    int search(string_view number) {
        int root = 0;
        for (const char &digit : number) {
            int d = digit - '0';
            if (!trie[root].next[d]) return -1;
            root = trie[root].next[d];
        }
        return root;
    }

    void prefix_search_dfs(int root, string &temp, vector<pair<string, string>> &result) {
        if (!root || result.size() >= 1000) return;
        if (!trie[root].name.empty()) {
            result.emplace_back(temp, trie[root].name);
            if (result.size() >= 1000) return;
        }
        for (int i = 0; i < 10; ++i) {
            if (trie[root].next[i]) {
                temp.push_back('0' + i);
                prefix_search_dfs(trie[root].next[i], temp, result);
                temp.pop_back();
            }
        }
    }

public:
    PhoneDirectory_key_number() : trie(1) {}

    void add_contact(string_view number, const string &name) {
        int root = 0;
        for (const char &digit : number) {
            int d = digit - '0';
            if (!trie[root].next[d]) {
                trie[root].next[d] = trie.size();
                trie.emplace_back(Node());
            }
            root = trie[root].next[d];
        }
        trie[root].name = name;
    }

    void delete_contact(string_view number) {
        int root = search(number);
        if (root == -1) return;
        trie[root].name.clear();
    }

    void edit_contact(string_view number, const string &new_name) {
        int root = search(number);
        if (root == -1 || trie[root].name.empty()) return;
        trie[root].name = new_name;
    }

    bool exact_search(string_view number, string &name) {
        int root = search(number);
        if (root == -1 || trie[root].name.empty()) return false;
        name = trie[root].name;
        return true;
    }

    bool prefix_search(string_view prefix, vector<pair<string, string>> &result) {
        int root = search(prefix);
        if (root == -1) return false;
        string temp(prefix);
        prefix_search_dfs(root, temp, result);
        return true;
    }
};


class PhoneDirectory_key_name {
private:
    vector<vector<vector<bool>>> dp;
    deque<tuple<string, int, int>> history;
    struct Node {
        array<int, 256> next{};
        bool end = false;
        set<pair<int, int>> numbers; // Store number with timestamp
        Node() {}
    };

    vector<Node> trie;

    int search(const string &key) {
        int root = 0;
        for (auto &i : key) {
            if (!trie[root].next[static_cast<unsigned char>(i)]) return -1;
            root = trie[root].next[static_cast<unsigned char>(i)];
        }
        return root;
    }

    void prefix_search_dfs(int root, string &temp, vector<pair<string, int>> &result) {
        if (!root || (int)result.size() >= 1000) return;
        if (trie[root].end) {
            for (auto &entry : trie[root].numbers) {
                result.emplace_back(temp, entry.first);
                if ((int)result.size() >= 1000) return;
            }
        }
        for (int i = 0; i < 256; i++) {
            if (trie[root].next[i]) {
                temp.push_back('\0' + i);
                prefix_search_dfs(trie[root].next[i], temp, result);
                temp.pop_back();
            }
        }
    }

    void fuzzy_search_contact(string &contact, int root, int edit_dist, int i, vector<pair<string,int>> &result, int max_edit = 5, int max_results = 100) {
        if ((int)result.size() >= max_results) return;
        if (edit_dist > max_edit) return;
        if (dp[edit_dist][i][root]) return;
        dp[edit_dist][i][root] = true;

        if (i == (int)contact.size()) {
            if (trie[root].end) {
                for (auto &entry : trie[root].numbers) {
                    result.emplace_back(contact, entry.first);
                    if ((int)result.size() >= max_results) return;
                }
            }
            return;
        }

        if (trie[root].end && edit_dist <= max_edit) {
            for (auto &entry : trie[root].numbers) {
                result.emplace_back(contact, entry.first);
                if ((int)result.size() >= max_results) return;
            }
        }

        unsigned char c_index = (unsigned char)contact[i];

        // Case 1: Exact match - move forward without increasing edit distance
        if (trie[root].next[c_index]) {
            fuzzy_search_contact(contact, trie[root].next[c_index], edit_dist, i + 1, result, max_edit, max_results);
            if ((int)result.size() >= max_results) return;
        }

        // Case 2: Substitution and insertion - try all other children except exact match
        for (int c = 0; c < 256; c++) {
            if (c == c_index || !trie[root].next[c]) continue;

            // Substitution: move forward in both contact and trie, increase edit_dist
            fuzzy_search_contact(contact, trie[root].next[c], edit_dist + 1, i + 1, result, max_edit, max_results);
            if ((int)result.size() >= max_results) return;

            // Insertion: move forward in trie only, increase edit_dist
            fuzzy_search_contact(contact, trie[root].next[c], edit_dist + 1, i, result, max_edit, max_results);
            if ((int)result.size() >= max_results) return;
        }

        // Case 3: Deletion - skip one char in contact, increase edit_dist
        fuzzy_search_contact(contact, root, edit_dist + 1, i + 1, result, max_edit, max_results);
    }

    void get_all_contacts_dfs(int root, string &temp, vector<tuple<string, int, int>> &result) {
        if (!root) return;
        if (trie[root].end) {
            for (auto &entry : trie[root].numbers) {
                result.emplace_back(temp, entry.first, entry.second);
            }
        }
        for (int i = 0; i < 256; i++) {
            if (trie[root].next[i]) {
                temp.push_back('\0' + i);
                get_all_contacts_dfs(trie[root].next[i], temp, result);
                temp.pop_back();
            }
        }
    }

    void sort_by_name(vector<tuple<string, int, int>> &contacts) {
        sort(contacts.begin(), contacts.end());
    }

    void sort_by_number(vector<tuple<string, int, int>> &contacts) {
        sort(contacts.begin(), contacts.end(), [](const auto &a, const auto &b) {
            return get<1>(a) < get<1>(b);
        });
    }

    void sort_by_date_modified(vector<tuple<string, int, int>> &contacts) {
        sort(contacts.begin(), contacts.end(), [](const auto &a, const auto &b) {
            return get<2>(a) > get<2>(b); // most recent first
        });
    }

public:
    PhoneDirectory_key_name() : trie(1, Node()) {}

    void add_contact(const string &key, int number) {
        history.emplace_back(key, number, time(0));
        if(history.size() > 100) history.pop_front();

        int root = 0;
        for (auto &i : key) {
            if (!trie[root].next[static_cast<unsigned char>(i)]) {
                trie[root].next[static_cast<unsigned char>(i)] = trie.size();
                trie.emplace_back(Node());
            }
            root = trie[root].next[static_cast<unsigned char>(i)];
        }
        trie[root].end = true;
        trie[root].numbers.insert({number, time(0)});
    }

    bool exact_search(const string &key, vector<pair<string, int>> &result) {
        int root = search(key);
        if (root == -1 || !trie[root].end) return false;
        for (auto &entry : trie[root].numbers) {
            result.emplace_back(key, entry.first);
        }
        return true;
    }

    bool prefix_search(const string &key, vector<pair<string, int>> &result) {
        int root = search(key);
        if (root == -1) return false;
        string temp = key;
        prefix_search_dfs(root, temp, result);
        return true;
    }

    bool fuzzy_search(const string &contact, vector<pair<string, int>> &result) {
        int max_edit = 5;
        int max_results = 100;
        long long estimated_size = 1LL * max_edit * contact.size() * trie.size();
        if (estimated_size > 1e7) {
            cout << "Fuzzy search aborted: input size too large to handle efficiently." << endl;
            return false;
        }
        dp.assign(max_edit + 1, vector<vector<bool>>(contact.size() + 1, vector<bool>(trie.size(), false)));
        fuzzy_search_contact(const_cast<string&>(contact), 0, 0, 0, result, max_edit, max_results);
        return !result.empty();
    }

    void delete_contact(const string &key) {
        int root = search(key);
        if (root == -1) return;
        trie[root].end = false;
        trie[root].numbers.clear();
    }

    void delete_contact(const string &key, int number) {
        int root = search(key);
        if (root == -1) return;
        auto it = trie[root].numbers.lower_bound({number, 0});
        if (it != trie[root].numbers.end() && it->first == number) {
            trie[root].numbers.erase(it);
        }
        if (trie[root].numbers.empty()) trie[root].end = false;
    }

    bool edit_contact(const string &old_name, int old_number, const string &new_name, int new_number) {
        int old_root = search(old_name);
        if (old_root == -1) return false;

        auto it = trie[old_root].numbers.lower_bound({old_number, 0});
        if (it == trie[old_root].numbers.end() || it->first != old_number) return false;

        trie[old_root].numbers.erase(it);
        if (trie[old_root].numbers.empty()) trie[old_root].end = false;

        add_contact(new_name, new_number);
      
        return true;
    }

    bool sort_contacts(string &sort_type, vector<tuple<string, int, int>> &result) {
        string temp;
        get_all_contacts_dfs(0, temp, result);
        if (sort_type == "NAME") sort_by_name(result);
        else if (sort_type == "NUMBER") sort_by_number(result);
        else if (sort_type == "DATE MODIFIED") sort_by_date_modified(result);
        else return false;
        return true;
    }

    void get_all_contacts(vector<tuple<string, int, int>> &result) {
        string temp;
        get_all_contacts_dfs(0, temp, result);
    }

    // Public function to export contacts to CSV
    bool export_to_csv(const string &filename) {
        vector<tuple<string, int, int>> contacts;
        get_all_contacts(contacts);

        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Failed to open file for writing: " << filename << endl;
            return false;
        }

        file << "Name,Number,TimeModified\n";
        for (auto &entry : contacts) {
            file << get<0>(entry) << ", " << get<1>(entry) << ", " << get<2>(entry) << "\n";
        }
        file.close();
        return true;
    }

    // Public function to get recent search history
    void search_history(vector<tuple<string, int, int>> &result) {
        result.assign(history.begin(), history.end());
    }
};



class PhoneDirectoryHandler {
private:
    PhoneDirectory_key_name nameTrie;
    PhoneDirectory_key_number numberTrie;

    void addContact() {
        string name;
        int number;
        cout << "Enter name: ";
        cin >> ws;
        getline(cin, name);
        cout << "Enter number: ";
        cin >> number;
        nameTrie.add_contact(name, number);
        numberTrie.add_contact(to_string(number), name);
    }

    void deleteContact() {
    cout << "Delete by (1) Name or (2) Number? ";
    int choice;
    cin >> choice;
    if (choice == 1) {
        string name;
        cout << "Enter name: ";
        cin >> ws;
        getline(cin, name);

        vector<pair<string, int>> found;
        if (nameTrie.exact_search(name, found)) {
            if (found.size() > 1) {
                cout << "Multiple numbers found for this name. Provide number to delete specific contact or press 0 to delete all:\n";
                for (auto &[n, num] : found) {
                    cout << n << ": " << num << endl;
                }
                int number;
                cin >> number;
                if (number == 0) {
                    // Delete all contacts with this name using overloaded function
                    nameTrie.delete_contact(name);
                    // Also delete corresponding numbers from numberTrie
                    for (auto &[n, num] : found) {
                        numberTrie.delete_contact(to_string(num));
                    }
                } 
                else {
                    nameTrie.delete_contact(name, number);
                    numberTrie.delete_contact(to_string(number));
                }
            } 
            else {
                // Only one contact, delete all contacts for this name
                nameTrie.delete_contact(name);
                numberTrie.delete_contact(to_string(found[0].second));
            }
        }
        else {
            cout << "No such name found.\n";
        }

    }
    else {
        string number;
        cout << "Enter number: ";
        cin >> number;
        string name;
        if (numberTrie.exact_search(number, name)) {
            nameTrie.delete_contact(name, stoi(number));
            numberTrie.delete_contact(number);
        } 
        else {
            cout << "No such number found.\n";
        }
    }
}


    void exactSearch() {
        cout << "Search by (1) Name or (2) Number? ";
        int choice;
        cin >> choice;
        if (choice == 1) {
            string name;
            cout << "Enter name: ";
            cin >> ws;
            getline(cin, name);
            vector<pair<string, int>> result;
            if (nameTrie.exact_search(name, result)) {
                for (auto &[n, num] : result) cout << n << ": " << num << endl;
            } else cout << "Contact not found.\n";
        } 
        else {
            string number;
            cout << "Enter number: ";
            cin >> number;
            string name;
            if (numberTrie.exact_search(number, name)) cout << number << ": " << name << endl;
            else cout << "Contact not found.\n";
        }
    }

    void prefixSearch() {
        cout << "Prefix search by (1) Name or (2) Number? ";
        int choice;
        cin >> choice;
        if (choice == 1) {
            string prefix;
            cout << "Enter name prefix: ";
            cin >> ws;
            getline(cin, prefix);
            vector<pair<string, int>> result;
            if (nameTrie.prefix_search(prefix, result)) {
                for (auto &[n, num] : result) cout << n << ": " << num << endl;
            } else cout << "No matching contacts.\n";
        } 
        else {
            string prefix;
            cout << "Enter number prefix: ";
            cin >> prefix;
            vector<pair<string, string>> result;
            if (numberTrie.prefix_search(prefix, result)) {
                for (auto &[num, name] : result) cout << num << ": " << name << endl;
            } else cout << "No matching contacts.\n";
        }
    }

    void fuzzySearch() {
        string query;
        cout << "Enter name for fuzzy search: ";
        cin >> ws;
        getline(cin, query);
        vector<pair<string, int>> result;
        if (nameTrie.fuzzy_search(query, result)) {
            for (auto &[n, num] : result) cout << n << ": " << num << endl;
        } else cout << "No similar contacts found.\n";
    }

    void sortContacts() {
        string type;
        cout << "Sort by (NAME/NUMBER/DATE_MODIFIED): ";
        cin >> type;
        vector<tuple<string, int, int>> result;
        nameTrie.sort_contact(type, result);
        for (auto &[name, num, time] : result) {
            cout << name << ": " << num << " (" << time << ")" << endl;
        }
    }

    void printAll() {
        vector<tuple<string, int, int>> result;
        nameTrie.get_all_contacts(result);
        for (auto &[name, num, time] : result) {
            cout << name << ": " << num << " (" << time << ")" << endl;
        }
    }

    void exportContacts() {
        nameTrie.export_contacts("contacts.csv");
    }

    void importContacts() {
        vector<pair<string, int>> result;
        nameTrie.import_contacts("contacts.csv", result);
        for (auto &[name, number] : result) {
            numberTrie.add_contact(to_string(number), name);
        }
    }

    void printHelp() {
        cout << "\nAvailable commands:\n";
        cout << "addContact           - Add a new contact\n";
        cout << "delete               - Delete a contact\n";
        cout << "exactSearch          - Search a contact by exact name or number\n";
        cout << "prefixSearch         - Find contacts starting with a name/number\n";
        cout << "fuzzySearch          - Find similar names (typo-tolerant)\n";
        cout << "sortContacts         - Sort contacts by name/number/date\n";
        cout << "printAll             - Display all stored contacts\n";
        cout << "exportContacts       - Save all contacts to a file\n";
        cout << "importContacts       - Load contacts from a file\n";
        cout << "help                 - Show this help menu\n";
        cout << "exit                 - Exit the program\n\n";
    }

public:
    void run() {
        cout << "Welcome to the Phone Directory! Type 'help' to see available commands.\n";
        string cmd;
        while (true) {
            cout << "\n> ";
            cin >> cmd;
            if (cmd == "addContact") addContact();
            else if (cmd == "delete") deleteContact();
            else if (cmd == "exactSearch") exactSearch();
            else if (cmd == "prefixSearch") prefixSearch();
            else if (cmd == "fuzzySearch") fuzzySearch();
            else if (cmd == "sortContacts") sortContacts();
            else if (cmd == "printAll") printAll();
            else if (cmd == "exportContacts") exportContacts();
            else if (cmd == "importContacts") importContacts();
            else if (cmd == "help") printHelp();
            else if (cmd == "exit") break;
            else cout << "Unrecognized command. Type 'help' to see available commands.\n";
        }
    }
};





int32_t main(){
    ios::sync_with_stdio(0);
    cin.tie(0);
    
    //freopen("ride.in" , "r" , stdin);
    //freopen("ride.out" , "w" , stdout);
    
	 
    return 0;
}



/*
✅ Features Already Implemented
Name-keyed Trie:

add_contact(name, number)

delete_contact(name)

delete_contact(name, number)

exact_search(name, result)

prefix_search(name, result)

prefix_search_dfs(root, temp, result)

search(name)

🔜 Features To Implement (Planned)
📚 Search Extensions:
fuzzy_search(name, max_edit=5, max_results=100) using trie + DP

search_history (store last 100 searches with time(0) timestamp)

📑 Listing Features:
List contacts sorted by name

List contacts sorted by number

List contacts sorted by last modified time

💾 Data Persistence:
import_contacts(filename)

export_contacts(filename)

📞 Dual Key Structure:
A second Trie where number is the key, and contact name is the value.
(Useful for reverse lookup, deletions, etc.)
*/
