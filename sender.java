package assi6;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.math.BigInteger;
import java.net.Socket;
import java.security.MessageDigest;
import java.util.Random;
import java.util.Scanner;

public class sender {
    public static boolean isPrime(int num) {
        if (num <= 1)
            return false;
        for (int i = 2; i <= Math.sqrt(num); i++)
            if (num % i == 0)
                return false;
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

    public static BigInteger[] generateKeypair() {
        Random rand = new Random();
        int p = 0, q = 0;
        while (!isPrime(p))
            p = rand.nextInt(400) + 100;
        while (!isPrime(q) || q == p)
            q = rand.nextInt(400) + 100;

        BigInteger n = BigInteger.valueOf(p).multiply(BigInteger.valueOf(q));
        BigInteger phi = BigInteger.valueOf(p - 1).multiply(BigInteger.valueOf(q - 1));

        BigInteger e = BigInteger.valueOf(rand.nextInt(phi.intValue() - 2) + 2);
        while (!e.gcd(phi).equals(BigInteger.ONE)) {
            e = BigInteger.valueOf(rand.nextInt(phi.intValue() - 2) + 2);
        }

        BigInteger d = e.modInverse(phi);
        return new BigInteger[] { e, d, n };
    }

    public static BigInteger[] rsaEncrypt(String msg, BigInteger e, BigInteger n) {
        BigInteger[] cipher = new BigInteger[msg.length()];
        for (int i = 0; i < msg.length(); i++) {
            cipher[i] = BigInteger.valueOf((int) msg.charAt(i)).modPow(e, n);
        }
        return cipher;
    }

    public static BigInteger[] generateSignature(String msg, BigInteger d, BigInteger n) throws Exception {
        String hash = sha256(msg);
        BigInteger[] signature = new BigInteger[hash.length()];
        for (int i = 0; i < hash.length(); i++) {
            signature[i] = BigInteger.valueOf(hash.charAt(i)).modPow(d, n);
        }
        return signature;
    }

    public static String sha256(String msg) throws Exception {
        MessageDigest digest = MessageDigest.getInstance("SHA-256");
        byte[] hash = digest.digest(msg.getBytes("UTF-8"));
        StringBuilder hex = new StringBuilder();
        for (byte b : hash)
            hex.append(String.format("%02x", b));
        return hex.toString();
    }

    public static void main(String[] args) throws Exception {
        int PORT = 65432;
        String HOST = "localhost";
        Socket socket = new Socket(HOST, PORT);

        BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        PrintWriter out = new PrintWriter(socket.getOutputStream(), true);

        // Generate sender keypair
        BigInteger[] keys = generateKeypair();
        BigInteger e_s = keys[0], d_s = keys[1], n_s = keys[2];
        System.out.println("Sender Public Key: (" + e_s + ", " + n_s + ")");
        System.out.println("Sender Private Key: (" + d_s + ", " + n_s + ")");

        // Receive receiver public key
        String recvData = in.readLine();
        String[] parts = recvData.split(",");
        BigInteger e_r = new BigInteger(parts[0]);
        BigInteger n_r = new BigInteger(parts[1]);
        System.out.println("Received Receiver Public Key: (" + e_r + ", " + n_r + ")");

        // Send sender public key
        out.println(e_s + "," + n_s);

        // Input message
        Scanner sc = new Scanner(System.in);
        System.out.print("\nEnter message to send: ");
        String message = sc.nextLine();

        // Encrypt message with receiver public key
        BigInteger[] cipher = rsaEncrypt(message, e_r, n_r);

        // Generate digital signature
        BigInteger[] signature = generateSignature(message, d_s, n_s);

        // Send both as payload
        out.println(java.util.Arrays.toString(cipher) + "|" + java.util.Arrays.toString(signature));
        System.out.println("\n📤 Message and signature sent successfully!");
        System.out.println("Encrypted Message: " + java.util.Arrays.toString(cipher));
        System.out.println("Signature: " + java.util.Arrays.toString(signature));

        socket.close();
    }
}