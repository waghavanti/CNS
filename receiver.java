package assi6;

import java.io.*;
import java.net.*;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.util.Random;

public class receiver {
    // ---- Utility functions ----
    public static boolean isPrime(int num) {
        if (num <= 1) return false;
        for (int i = 2; i <= Math.sqrt(num); i++)
            if (num % i == 0) return false;
        return true;
    }

    public static int gcd(int a, int b) {
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }

    public static int modInverse(int a, int m) {
        a = a % m;
        for (int x = 1; x < m; x++)
            if ((a * x) % m == 1)
                return x;
        return 1;
    }

    public static int[] generatePrime() {
        Random rand = new Random();
        int num = rand.nextInt(400) + 100;
        while (!isPrime(num)) num++;
        return new int[]{num};
    }

    public static BigInteger[] generateKeypair() {
        Random rand = new Random();
        int p = 0, q = 0;
        while (!isPrime(p)) p = rand.nextInt(400) + 100;
        while (!isPrime(q) || q == p) q = rand.nextInt(400) + 100;

        BigInteger n = BigInteger.valueOf(p).multiply(BigInteger.valueOf(q));
        BigInteger phi = BigInteger.valueOf(p - 1).multiply(BigInteger.valueOf(q - 1));

        BigInteger e = BigInteger.valueOf(rand.nextInt(phi.intValue() - 2) + 2);
        while (!e.gcd(phi).equals(BigInteger.ONE)) {
            e = BigInteger.valueOf(rand.nextInt(phi.intValue() - 2) + 2);
        }

        BigInteger d = e.modInverse(phi);
        return new BigInteger[]{e, d, n};
    }

    public static BigInteger[] rsaEncrypt(String msg, BigInteger e, BigInteger n) {
        BigInteger[] cipher = new BigInteger[msg.length()];
        for (int i = 0; i < msg.length(); i++) {
            cipher[i] = BigInteger.valueOf((int) msg.charAt(i)).modPow(e, n);
        }
        return cipher;
    }

    public static String rsaDecrypt(BigInteger[] cipher, BigInteger d, BigInteger n) {
        StringBuilder msg = new StringBuilder();
        for (BigInteger c : cipher) {
            msg.append((char) c.modPow(d, n).intValue());
        }
        return msg.toString();
    }

    public static BigInteger[] generateSignature(String msg, BigInteger d, BigInteger n) throws Exception {
        String hash = sha256(msg);
        BigInteger[] signature = new BigInteger[hash.length()];
        for (int i = 0; i < hash.length(); i++) {
            signature[i] = BigInteger.valueOf(hash.charAt(i)).modPow(d, n);
        }
        return signature;
    }

    public static boolean verifySignature(String msg, BigInteger[] signature, BigInteger e, BigInteger n) throws Exception {
        String hash = sha256(msg);
        StringBuilder decryptedHash = new StringBuilder();
        for (BigInteger c : signature) {
            decryptedHash.append((char) c.modPow(e, n).intValue());
        }
        return decryptedHash.toString().equals(hash);
    }

    public static String sha256(String msg) throws Exception {
        MessageDigest digest = MessageDigest.getInstance("SHA-256");
        byte[] hash = digest.digest(msg.getBytes("UTF-8"));
        StringBuilder hex = new StringBuilder();
        for (byte b : hash) hex.append(String.format("%02x", b));
        return hex.toString();
    }

    public static void main(String[] args) throws Exception {
        int PORT = 65432;
        ServerSocket server = new ServerSocket(PORT);
        System.out.println("🔐 Receiver waiting for sender connection...");

        Socket socket = server.accept();
        System.out.println("✅ Connected to sender: " + socket.getInetAddress());

        BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        PrintWriter out = new PrintWriter(socket.getOutputStream(), true);

        // Generate receiver keypair
        BigInteger[] keys = generateKeypair();
        BigInteger e_r = keys[0], d_r = keys[1], n_r = keys[2];
        System.out.println("Receiver Public Key: (" + e_r + ", " + n_r + ")");
        System.out.println("Receiver Private Key: (" + d_r + ", " + n_r + ")");

        // Send public key to sender
        out.println(e_r + "," + n_r);

        // Receive sender’s public key
        String senderPubData = in.readLine();
        String[] parts = senderPubData.split(",");
        BigInteger e_s = new BigInteger(parts[0]);
        BigInteger n_s = new BigInteger(parts[1]);
        System.out.println("Received Sender Public Key: (" + e_s + ", " + n_s + ")");

        // Receive message and signature
        String payload = in.readLine();
        String[] msgParts = payload.split("\\|");
        String[] cipherParts = msgParts[0].replace("[", "").replace("]", "").split(", ");
        String[] sigParts = msgParts[1].replace("[", "").replace("]", "").split(", ");

        BigInteger[] cipher = new BigInteger[cipherParts.length];
        for (int i = 0; i < cipherParts.length; i++)
            cipher[i] = new BigInteger(cipherParts[i]);

        BigInteger[] signature = new BigInteger[sigParts.length];
        for (int i = 0; i < sigParts.length; i++)
            signature[i] = new BigInteger(sigParts[i]);

        // Decrypt and verify
        String decrypted = rsaDecrypt(cipher, d_r, n_r);
        System.out.println("\n📩 Decrypted Message: " + decrypted);

        if (verifySignature(decrypted, signature, e_s, n_s)) {
            System.out.println("✅ Signature verified. Message integrity OK.");
        } else {
            System.out.println("❌ Invalid signature! Message tampered.");
        }

        socket.close();
        server.close();
    }
}

