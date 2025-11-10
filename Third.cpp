#include <iostream>
#include <string>
using namespace std;

// Encryption: E = (P + key + i) % 26
string encrypt(const string &text, int key) {
    string result = "";
    for (int i = 0; i < (int)text.length(); i++) {
        char ch = text[i];
        if (ch >= 'A' && ch <= 'Z') {
            int p = ch - 'A';
            int c = (p + key + i) % 26;
            result += (char)(c + 'A');
        } else {
            result += ch; // keep symbols or spaces unchanged
        }
    }
    return result;
}

// Decryption: D = (C - key - i + 26) % 26
string decrypt(const string &text, int key) {
    string result = "";
    for (int i = 0; i < (int)text.length(); i++) {
        char ch = text[i];
        if (ch >= 'A' && ch <= 'Z') {
            int c = ch - 'A';
            int p = (c - key - i + 26) % 26;
            result += (char)(p + 'A');
        } else {
            result += ch;
        }
    }
    return result;
}

int main() {
    cout << "===== Encryption/Decryption Algorithm =====" << endl;

    string text;
    cout << "Enter text (UPPERCASE only): ";
    getline(cin, text); // read full line including spaces

    int key;
    cout << "Enter key (number): ";
    cin >> key;

    string encrypted = encrypt(text, key);
    cout << "Encrypted Text: " << encrypted << endl;

    string decrypted = decrypt(encrypted, key);
    cout << "Decrypted Text: " << decrypted << endl;

    return 0;
}
