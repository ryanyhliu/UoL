package Old;
import javax.crypto.Cipher;
import javax.crypto.SecretKey;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.PBEKeySpec;
import javax.crypto.spec.SecretKeySpec;
import java.security.spec.KeySpec;
// import java.security.spec.PBEKeySpec;
import java.util.Arrays;
import java.util.Base64;

public class BruteForcePasswordCracker {
    public static void main(String[] args) throws Exception {
        String encryptedData = "a"; // Replace with your encrypted data
        byte[] salt = "RandomSalt".getBytes(); // Use a secure random salt in practice
        int iterations = 10000; // Choose an appropriate number of iterations
        int keyLength = 128; // Key length (in bits)

        // Initialize brute force range (e.g., lowercase letters and digits)
        String characterSet = "abcdefghijklmnopqrstuvwxyz0123456789";

        // Try all possible combinations in the character set
        for (int len = 1; len <= 6; len++) { // Example: Check for passwords up to length 6
            bruteForce(encryptedData, salt, iterations, keyLength, characterSet, "", len);
        }
    }

    public static void bruteForce(String encryptedData, byte[] salt, int iterations, int keyLength, String characterSet, String currentPassword, int length) throws Exception {
        if (length == 0) {
            // Try decrypting with the currentPassword
            SecretKey key = generateKeyFromPassword(currentPassword, salt, iterations, keyLength);
            try {
                Cipher cipher = Cipher.getInstance("AES");
                cipher.init(Cipher.DECRYPT_MODE, key);
                byte[] decryptedData = cipher.doFinal(Base64.getDecoder().decode(encryptedData));
                System.out.println("Password found: " + currentPassword);
                System.out.println("Decrypted Data: " + new String(decryptedData));
            } catch (Exception e) {
                // Incorrect password
            }
            return;
        }
        for (int i = 0; i < characterSet.length(); i++) {
            String newCandidate = currentPassword + characterSet.charAt(i);
            bruteForce(encryptedData, salt, iterations, keyLength, characterSet, newCandidate, length - 1);
        }
    }

    public static SecretKey generateKeyFromPassword(String password, byte[] salt, int iterations, int keyLength) throws Exception {
        SecretKeyFactory factory = SecretKeyFactory.getInstance("PBKDF2WithHmacSHA256");
        KeySpec spec = new PBEKeySpec(password.toCharArray(), salt, iterations, keyLength);
        SecretKey secretKey = factory.generateSecret(spec);
        return new SecretKeySpec(secretKey.getEncoded(), "AES");
    }
}
