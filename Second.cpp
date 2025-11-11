
#include <iostream>
#include <string>
#include <limits>
#include <cctype>
using namespace std;

long long gcd(long long a, long long b)
{ 
    return b==0 ? a : gcd(b, a%b); 
}

long long modInverse(long long e, long long phi){
    for(long long d=1; d<phi; d++){
        if((e*d)%phi==1) return d;
    }
    return -1;
}

long long powerMod(long long base, long long exp, long long mod){
    long long result = 1 % mod;
    base %= mod;
    while (exp > 0){
        if (exp & 1) result = (result * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}

int main(){
    long long p, q, e;

    cout << "Enter first prime number (p): " << endl;
    cin >> p;

    cout << "Enter second prime number (q): " << endl;
    cin >> q;

    long long n = p * q;
    long long phi = (p - 1) * (q - 1);

    if (n <= 126) {
        cout << "ERROR: n = " << n
             << " is too small for ASCII characters (need n > 126)."
             << "\nPick bigger primes (e.g., p=17, q=19 or p=17, q=23) and try again.\n";
        return 0;
    }

    cout << "Enter value of e (coprime with " << phi << "): " << endl;
    cin >> e;

    long long d = modInverse(e, phi);

    cout << "\nPublic Key (e, n): (" << e << ", " << n << ")\n";
    cout << "Private Key (d, n): (" << d << ", " << n << ")\n";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "\nEnter message (text or number): " << endl;
    string msg; getline(cin, msg);

    // Check if msg is all digits
    bool all_digits = !msg.empty();
    for(char c: msg){
        if(!isdigit(static_cast<unsigned char>(c))) { all_digits = false; break; }
    }

    if (all_digits) {
        // Treat as single integer block if it fits
        long long m = 0;
        for(char c: msg){ m = m*10 + (c - '0'); }

        if (m >= n) {
            cout << "\n[Single-block mode skipped] The number " << m
                 << " >= n (" << n << "). Choose larger primes so n > " << m
                 << ", or input shorter number. Falling back to char-by-char.\n";
        } else {
            cout << "\nEncrypting as ONE INTEGER block...\n";
            long long ciph = powerMod(m, e, n);
            cout << "Ciphertext: " << ciph << "\n";

            cout << "\nDecrypting...\n";
            long long dec = powerMod(ciph, d, n);
            cout << "Decrypted number: " << dec << "\n";
            return 0;
        }
    }

    // Fallback: character-by-character encryption
    cout << "\nEncrypting char-by-char...\n";
    int len = (int)msg.size();
    long long* cipherArray = new long long[len];
    cout << "Encrypted message: ";
    for(int i=0;i<len;i++){
        long long m = (unsigned char)msg[i]; // ASCII value
        cipherArray[i] = powerMod(m, e, n);
        cout << cipherArray[i] << " ";
    }

    cout << "\n\nDecrypting...\n";
    string decryptedMsg; decryptedMsg.reserve(len);
    for(int i=0;i<len;i++){
        long long dec = powerMod(cipherArray[i], d, n);
        decryptedMsg.push_back((char)dec);
    }
    cout << "Decrypted message: " << decryptedMsg << "\n";

    delete[] cipherArray;
    return 0;
}
