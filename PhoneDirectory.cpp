class PhoneDirectory_key_name {
private:
    vector<vector<vector<bool>>> dp;
    struct Node {
        array<int, 256> next{};
        bool end = false;
        set<int> numbers; // Store multiple numbers with efficient erase
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
            for (auto &number : trie[root].numbers) {
                result.emplace_back(temp, number);
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
                for (auto &number : trie[root].numbers) {
                    result.emplace_back(contact, number);
                    if ((int)result.size() >= max_results) return;
                }
            }
            return;
        }

        if (trie[root].end && edit_dist <= max_edit) {
            for (auto &number : trie[root].numbers) {
                result.emplace_back(contact, number);
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

public:
    PhoneDirectory_key_name() : trie(1, Node()) {}

    void add_contact(const string &key, int number) {
        int root = 0;
        for (auto &i : key) {
            if (!trie[root].next[static_cast<unsigned char>(i)]) {
                trie[root].next[static_cast<unsigned char>(i)] = trie.size();
                trie.emplace_back(Node());
            }
            root = trie[root].next[static_cast<unsigned char>(i)];
        }
        trie[root].end = true;
        trie[root].numbers.insert(number);
    }

    bool exact_search(const string &key, vector<pair<string, int>> &result) {
        int root = search(key);
        if (root == -1 || !trie[root].end) return false;
        for (auto &number : trie[root].numbers) {
            result.emplace_back(key, number);
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
        trie[root].numbers.erase(number);
        if (trie[root].numbers.empty()) trie[root].end = false;
    }
};


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