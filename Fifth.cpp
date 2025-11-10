#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include <iomanip>

using namespace std;

// rotate-left for 32-bit
static inline uint32_t rotl32(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

// Manual SHA-1 implementation -> 40-char lowercase hex
string sha1(const string& input) {
    // Convert input to bytes (treat std::string as UTF-8 bytes already)
    const uint8_t* data = reinterpret_cast<const uint8_t*>(input.data());
    size_t len = input.size();

    uint64_t bitLen = static_cast<uint64_t>(len) * 8ULL;
    size_t padLen = (56 - (len + 1) % 64 + 64) % 64; // bytes to reach 56 mod 64
    size_t total = len + 1 + padLen + 8;             // +0x80 +padding +8 length

    vector<uint8_t> buf(total, 0);
    // copy data
    for (size_t i = 0; i < len; ++i) buf[i] = data[i];
    // append 0x80
    buf[len] = 0x80;
    // append big-endian length
    for (int i = 0; i < 8; ++i) {
        buf[total - 1 - i] = static_cast<uint8_t>((bitLen >> (8 * i)) & 0xFF);
    }

    uint32_t h0 = 0x67452301u;
    uint32_t h1 = 0xEFCDAB89u;
    uint32_t h2 = 0x98BADCFEu;
    uint32_t h3 = 0x10325476u;
    uint32_t h4 = 0xC3D2E1F0u;

    // process 512-bit blocks
    for (size_t b = 0; b < buf.size(); b += 64) {
        uint32_t w[80] = {0};

        // first 16 words: big-endian
        for (int i = 0; i < 16; ++i) {
            size_t j = b + i * 4;
            w[i] = (uint32_t(buf[j]) << 24) |
                   (uint32_t(buf[j + 1]) << 16) |
                   (uint32_t(buf[j + 2]) << 8) |
                   (uint32_t(buf[j + 3]));
        }
        // extend to 80 words
        for (int i = 16; i < 80; ++i) {
            w[i] = rotl32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        }

        uint32_t a = h0, b1 = h1, c = h2, d = h3, e = h4;

        for (int i = 0; i < 80; ++i) {
            uint32_t f, k;
            if (i < 20) {
                f = (b1 & c) | ((~b1) & d);
                k = 0x5A827999u;
            } else if (i < 40) {
                f = b1 ^ c ^ d;
                k = 0x6ED9EBA1u;
            } else if (i < 60) {
                f = (b1 & c) | (b1 & d) | (c & d);
                k = 0x8F1BBCDCu;
            } else {
                f = b1 ^ c ^ d;
                k = 0xCA62C1D6u;
            }
            uint32_t tmp = rotl32(a, 5) + f + e + k + w[i];
            e = d;
            d = c;
            c = rotl32(b1, 30);
            b1 = a;
            a = tmp;
        }

        h0 += a; h1 += b1; h2 += c; h3 += d; h4 += e;
    }

    // produce digest (20 bytes, big-endian) → hex
    ostringstream oss;
    oss << hex << nouppercase << setfill('0');
    auto put_word = [&](uint32_t v) {
        oss << setw(2) << ((v >> 24) & 0xFF);
        oss << setw(2) << ((v >> 16) & 0xFF);
        oss << setw(2) << ((v >> 8) & 0xFF);
        oss << setw(2) << (v & 0xFF);
    };
    put_word(h0); put_word(h1); put_word(h2); put_word(h3); put_word(h4);
    return oss.str();
}

int main() {
    cout << "=== SHA-1 Transmission Simulation (User1 -> User2) ===\n";

    // User1
    cout << "\nUser1: Enter message to send: ";
    string user1Message;
    getline(cin, user1Message);

    string user1Hash = sha1(user1Message);
    cout << "\n[User1] Message: " << user1Message << "\n";
    cout << "[User1] SHA-1: " << user1Hash << "\n";

    cout << "\n[Network] Sending message and hash to User2...\n";

    // User2 (can alter message)
    cout << "\nUser2: Enter the received message: ";
    string user2Received;
    getline(cin, user2Received);

    cout << "\n[User2] Received (claimed) SHA-1 from network: " << user1Hash << "\n";
    string recomputed = sha1(user2Received);
    cout << "[User2] Recomputed SHA-1 of received message: " << recomputed << "\n";

    if (recomputed == user1Hash) {
        cout << "\nHash match — message integrity verified (no alteration).\n";
    } else {
        cout << "\nHash mismatch — message was altered during transmission!\n";
    }
    return 0;
}
