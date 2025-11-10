#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
using namespace std;

string caesar(string t, int k, bool enc){
    string r; 
    for(char c: t){
        if(isalpha((unsigned char)c))
        {
            char base = isupper((unsigned char)c) ? 'A' : 'a';
            int s = enc ? (c-base+k)%26 : (c-base-(k%26)+26)%26;
            r += char(base+s);
        } 
        else r += c;
    }
    return r;
}


string mono(string t, bool enc){
    string key="QWERTYUIOPASDFGHJKLZXCVBNM", A="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unordered_map<char,char> m;

    for(int i=0;i<26;i++) 
    {
    if (enc)
    m[A[i]] = key[i];
    else
    m[key[i]] = A[i];
    }


    for(char &c:t)
    c=toupper((unsigned char)c);

    string r; 

    for(char c:t) 
    r += isalpha((unsigned char)c)? m[c]:c;
    
    return r;
}

string vigenere(string t, string k, bool enc)
{
    for(char &c:t) 
    c=toupper((unsigned char)c);
    for(char &c:k) 
    c=toupper((unsigned char)c);
    string r; 
    int j=0; 
    int n=k.size();
    for(char c:t)
    {
        if(isalpha((unsigned char)c))
        {
            int s=k[j]-'A';
            if(enc)
                r += char(((c - 'A') + s) % 26 + 'A');   // encrypt
            else
                r += char(((c - 'A') + (26 - s)) % 26 + 'A'); // decrypt
            j=(j+1)%n;
        } 
        else r+=c;
    }
    return r;
}

string remove_spaces(string s){
    string t; 
    for(char c:s) 
    {
    if(!isspace((unsigned char)c)) 
    t+=c;
    }
    
    return t;
}

string rail_encrypt(string t, int rails){
    if(rails<=1) return t;
    vector<string> f(rails); int r=0,d=1;
    for(char c:t){
        f[r]+=c; r+=d;
        if(r==0||r==rails-1) d=-d;
    }
    string out; for(auto &x:f) out+=x; return out;
}

string rail_decrypt(string t, int rails){
    if(rails<=1) return t;
    int n=t.size(), cyc=2*rails-2, idx=0;
    vector<char> dec(n);
    for(int r=0;r<rails;r++){
        int pos=r; bool down=true;
        while(pos<n){
            dec[pos]=t[idx++];
            pos += (r==0||r==rails-1)?cyc:(down?2*(rails-r-1):2*r);
            down=!down;
        }
    }
    return string(dec.begin(),dec.end());
}

string vernam(string t, string k){
    for(char &c:t) c=toupper((unsigned char)c);
    for(char &c:k) c=toupper((unsigned char)c);
    string r; r.reserve(t.size());
    for(size_t i=0;i<t.size();i++)
        r += isalpha((unsigned char)t[i]) ? char(((t[i]-'A')^(k[i]-'A'))+'A') : t[i];
    return r;
}


int main(){
    char again;
    do{
        cout<<"\n===== Classical Encryption Techniques =====\n"
            <<"1. Caesar Cipher\n2. Monoalphabetic Cipher\n3. Vigenere Cipher\n4. Rail Fence Cipher\n5. Vernam Cipher\n"
            <<"Enter choice (1-5): ";

        int ch; 
        if(!(cin>>ch)) 
        return 0; 
        cin.ignore();

        cout<<"Encrypt (E) or Decrypt (D)? ";
        char act; 
        cin>>act; 
        act=toupper(act); 
        cin.ignore();
        bool enc = (act=='E');

        if(ch==1){
            string t; int k;
            cout<<"Enter text: "; 
            getline(cin,t);
            cout<<"Enter key: "; 
            cin>>k; 
            cin.ignore();
            cout<<"Result: "<<caesar(t,k,enc)<<"\n";
        }
        else if(ch==2){
            cout<<"Enter text (UPPERCASE): ";
            string t; 
            getline(cin,t);
            cout<<"Result: "<<mono(t,enc)<<"\n";
        }
        else if(ch==3){
            string t,k; 
            cout<<"Enter text: "; 
            getline(cin,t);
            cout<<"Enter key: "; 
            getline(cin,k);
            cout<<"Result: "<<vigenere(t,k,enc)<<"\n";
        }
        else if(ch==4){
            string t; 
            int n;
            cout<<"Enter text: "; 
            getline(cin,t); 
            t=remove_spaces(t);
            cout<<"Enter rails: "; 
            cin>>n; 
            cin.ignore();
            cout<<"Result: "<<(enc? rail_encrypt(t,n):rail_decrypt(t,n))<<"\n";
        }
        else if(ch==5){
            string t,k; 
            cout<<"Enter text: "; 
            getline(cin,t);
            cout<<"Enter key (same length as text): "; 
            getline(cin,k);
            if(t.size()!=k.size()) 
            cout<<"Error: Key length must be same as text!\n";
            else cout<<"Result: "<<vernam(t,k)<<"\n";
        }
        else cout<<"Invalid choice!\n";

        cout<<"\nContinue? (Y/N): "; cin>>again; cin.ignore(); again=toupper(again);
    }while(again=='Y');

    cout<<"Thank you! Exiting...\n";
    return 0;
}
