#include <iostream>
#include <string>
using namespace std;

long long power(long long base, long long exp, long long mod) {
    long long res = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp % 2 == 1) res = (res * base) % mod;
        exp = exp / 2;
        base = (base * base) % mod;
    }
    return res;
}

bool isPrime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; ++i)
        if (n % i == 0) return false;
    return true;
}

string xorEncrypt(const string &text, long long key) {
    string out = text;
    unsigned char k = static_cast<unsigned char>(key & 0xFF);
    for (size_t i = 0; i < text.size(); ++i)
        out[i] = text[i] ^ k;
    return out;
}

void showHex(const string &s) {
    for (unsigned char c : s)
        printf("%02X", c);
    printf("\n");
}

int main() {
    long long p, g;
    cout << "Enter prime number p: ";
    cin >> p;
    while (!isPrime((int)p)) {
        cout << "Not prime. Enter prime p again: ";
        cin >> p;
    }
    cout << "Enter generator g: ";
    cin >> g;

    long long a, b;
    cout << "Enter Alice private key: ";
    cin >> a;
    cout << "Enter Bob private key: ";
    cin >> b;

    cout << "\nPublic values: p=" << p << " g=" << g << "\n";

    long long A = power(g, a, p);
    long long B = power(g, b, p);

    cout << "Alice public = " << A << "\n";
    cout << "Bob public   = " << B << "\n";

    int mode;
    cout << "\nChoose mode: 1 normal  2 MITM\nEnter: ";
    cin >> mode;
    cin.ignore();

    if (mode == 1) {
        cout << "\nNormal communication\n";
        long long sharedA = power(B, a, p);
        long long sharedB = power(A, b, p);
        cout << "Alice shared = " << sharedA << "\n";
        cout << "Bob   shared = " << sharedB << "\n";

        string msg;
        cout << "\nEnter message Alice sends: ";
        getline(cin, msg);

        string enc = xorEncrypt(msg, sharedA);
        string dec = xorEncrypt(enc, sharedB);

        cout << "\nEncrypted (hex): ";
        showHex(enc);
        cout << "Bob reads: " << dec << "\n";
    }
    else if (mode == 2) {
        cout << "\nMan-in-the-Middle simulation (automatic tampering)\n";
        long long e1, e2;
        cout << "Enter Eve private key for Alice side: ";
        cin >> e1;
        cout << "Enter Eve private key for Bob side: ";
        cin >> e2;
        cin.ignore();

        long long E_A = power(g, e1, p); 
        long long E_B = power(g, e2, p);

        cout << "\nEve sends fake public to Alice: " << E_A << "\n";
        cout << "Eve sends fake public to Bob:   " << E_B << "\n";

        long long aliceShared = power(E_A, a, p); 
        long long bobShared   = power(E_B, b, p); 
        long long eveWithA    = power(A, e1, p);  
        long long eveWithB    = power(B, e2, p);  

        cout << "\nKeys after interception:\n";
        cout << "Alice<->Eve = " << aliceShared << "\n";
        cout << "Eve<->Bob   = " << bobShared << "\n";
        cout << "Eve with A  = " << eveWithA << "\n";
        cout << "Eve with B  = " << eveWithB << "\n";

        string msg;
        cout << "\nAlice sends message: ";
        getline(cin, msg);

        string encFromAlice = xorEncrypt(msg, aliceShared);
        cout << "Alice sends (hex): ";
        showHex(encFromAlice);

        
        string eveReads = xorEncrypt(encFromAlice, eveWithA);
        cout << "Eve reads: " << eveReads << "\n";

        string tampered = eveReads;
        for (size_t i = 0; i < tampered.size(); ++i) {
            if (isprint((unsigned char)tampered[i])) {
                tampered[i] = static_cast<char>(tampered[i] + 1); 
                break;
            }
        }
        cout << "Eve modifies message.\n";

        string encToBob = xorEncrypt(tampered, eveWithB);
        cout << "Eve forwards (hex): ";
        showHex(encToBob);

        string bobReads = xorEncrypt(encToBob, bobShared);
        cout << "Bob reads: " << bobReads << "\n";
    }
    else {
        cout << "Invalid mode\n";
    }

    return 0;
}