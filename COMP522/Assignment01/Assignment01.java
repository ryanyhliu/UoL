package Assignment01;
// Password List
//
// P@$$W0rD
// Thisismypassword
// VeryLongP@$$W0rD

import javax.crypto.Cipher;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.PBEKeySpec;
import javax.crypto.spec.PBEParameterSpec;
import java.lang.reflect.Array;
import java.security.*;
import java.security.spec.InvalidKeySpecException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;

import javax.crypto.Cipher;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.PBEKeySpec;
import javax.crypto.spec.PBEParameterSpec;



public class Assignment01{
    public static void main(String[] args) throws Exception{
        task03_TimeCalculator();
    }

    public static void task03_TimeCalculator() throws Exception{
        // Salt
        byte[] salt = {(byte) 0xc7, (byte) 0x73, (byte) 0x21, (byte) 0x8c, (byte) 0x7e, (byte) 0xc8, (byte) 0xee, (byte) 0x99};
        // Iteration count
        int count = 512;
        //Initialization of the password
        char[] password = "P@".toCharArray();

        // Create PBE parameter set
        PBEParameterSpec pbeParamSpec = new PBEParameterSpec(salt, count);

        // Generate a key
        PBEKeySpec pbeKeySpec = new PBEKeySpec(password); // Parameter for key generation
        SecretKeyFactory keyFac = SecretKeyFactory.getInstance("PBEWithMD5AndDES"); // Instance of SecretKeyFactory for password-based encryption
        Key pbeKey = keyFac.generateSecret(pbeKeySpec);

        // Set a timer
        long timerStart = System.nanoTime();

        // Encryption
        Cipher pbeCipher = Cipher.getInstance("PBEWithMD5AndDES"); // Create PBE Cipher
        pbeCipher.init(Cipher.ENCRYPT_MODE, pbeKey, pbeParamSpec); // Initialize PBE Cipher with key and parameters


        String plaintext = "This is another example";
        byte[] bytearr_plaintext = plaintext.getBytes();
        byte[] ciphertext = pbeCipher.doFinal(bytearr_plaintext);

        System.out.println("EncryptTime: " +  (System.nanoTime() - timerStart));



        // Yongheng's code Decryption
        // Reset timer
        timerStart = System.nanoTime();

        // input: protocol, key, param, ciphertext
        Cipher newCipher = Cipher.getInstance("PBEWithMD5AndDES");
        newCipher.init(Cipher.DECRYPT_MODE, pbeKey, pbeParamSpec);
        byte[] bytearr_newPlaintext = newCipher.doFinal(ciphertext);
        String newPlaintext = new String(bytearr_newPlaintext);
        
        System.out.println("DecryptTime: " +  (System.nanoTime() - timerStart));
        System.out.println("Result: plaintext is: " + newPlaintext);

        burteforceAttack_withoutPassword(salt, count, bytearr_plaintext, ciphertext);
        burteforceAttack_withoutIterCount(salt, password, bytearr_plaintext, ciphertext);
    }

    public static void burteforceAttack_withoutPassword(byte[] salt, int iterCount, byte[] plaintext, byte[] ciphertext) throws Exception{
        System.out.println("---ATTACK w/o Password---");
        // Set timer
        long timerStart = System.nanoTime();
        List<String> tryPasswords = new ArrayList<String>();

        boolean flag = false;
        while (!flag) {
            tryPasswords = passwordGenerator();
            for (String tryPassword : tryPasswords){
                System.out.print("\rTrying: " + tryPassword + " ... ");
                // password -> key
                String str_password = tryPassword;
                char[] chararr_password = str_password.toCharArray();
                PBEKeySpec pbeKeySpec = new PBEKeySpec(chararr_password);

                SecretKeyFactory keyFac = SecretKeyFactory.getInstance("PBEWithMD5AndDES"); // using DES and MD5
                Key pbeKey = keyFac.generateSecret(pbeKeySpec);
                PBEParameterSpec pbeParamSpec = new PBEParameterSpec(salt, iterCount);
                Cipher pbeCipher = Cipher.getInstance("PBEWithMD5AndDES");
                pbeCipher.init(Cipher.ENCRYPT_MODE, pbeKey, pbeParamSpec);

                byte[] verifytext = pbeCipher.doFinal(plaintext);
                if (Arrays.equals(verifytext, ciphertext)) {
                    flag = true;
                    System.out.println("\nFIND: Password is " + tryPassword);
                    System.out.println("Attack Time: " +  (System.nanoTime() - timerStart));
                    return;
                }
                
            }
        }
    }

    // Define a global variable to store the current length
    public static int current_length_password = 1;
    // Define a global variable to store the current index
    public static int current_index_password = 0;

    public static List<String> passwordGenerator(){
        // Character set
        String chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345678!@#$%^&*()`~-_=+[]{};:'\",.<>/?\\|";
        // Max length of password
        int max_length = 8;
        // The number of passwords generated each time
        int limit = 20;

        // Create a List object to store a list of passwords
        List<String> stringList = new ArrayList<>();
        // Create a loop until the size of the list of strings reaches the limit or exceeds the maximum length
        while (stringList.size() < limit && current_length_password <= max_length) {
            StringBuilder string = new StringBuilder();
            // Based on the current index, the character at each position is calculated
            for (int i = 0; i < current_length_password; i++) {
                // Calculate the index of the character at the current position in chars, using the modulus operation
                int index = (current_index_password / (int) Math.pow(chars.length(), i)) % chars.length();
                // Based on the index, get a character from chars
                char c = chars.charAt(index);
                // The character is added to the beginning of the string because it is calculated from the least bit to the most bit
                string.insert(0, c);
            }
            stringList.add(string.toString());
            current_index_password++;
            // If the current index is equal to the length of the chars raised to the current_length power, 
            // it means that strings of the current length have been generated, 
            // and we need to increase the length and reset the index
            if (current_index_password == Math.pow(chars.length(), current_length_password)) {
                current_length_password++;
                current_index_password = 0;
            }
        }
        return stringList;
    }


    public static void burteforceAttack_withoutIterCount(byte[] salt, char[] password, byte[] plaintext, byte[] ciphertext) throws Exception{
        System.out.println("---ATTACK w/o IterCount---");
        // Set timer
        long timerStart = System.nanoTime();

        for (int exponent = 1; exponent <= 13; exponent++) {
            System.out.print("\rTrying: 2 ^ " + exponent + " ... ");
            boolean flag = false;
            while (!flag) {
                // password -> key
                PBEKeySpec pbeKeySpec = new PBEKeySpec(password);
                SecretKeyFactory keyFac = SecretKeyFactory.getInstance("PBEWithMD5AndDES"); 
                Key pbeKey = keyFac.generateSecret(pbeKeySpec);
                PBEParameterSpec pbeParamSpec = new PBEParameterSpec(salt, (int) Math.pow(2, exponent)); // iterCount
                Cipher pbeCipher = Cipher.getInstance("PBEWithMD5AndDES");
                pbeCipher.init(Cipher.ENCRYPT_MODE, pbeKey, pbeParamSpec);

                byte[] verifytext = pbeCipher.doFinal(plaintext);
                if (Arrays.equals(verifytext, ciphertext)) {
                    flag = true;
                    System.out.println("\nFIND: Itercount is " + (int) Math.pow(2, exponent));
                    System.out.println("Attack Time: " +  (System.nanoTime() - timerStart));
                    return;
                }
                break;
            }
        }
    }
}