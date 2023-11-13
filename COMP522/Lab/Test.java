import java.lang.reflect.Array;
import java.security.*;
import java.security.spec.InvalidKeySpecException;
import java.util.Arrays;
import javax.crypto.Cipher;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.PBEKeySpec;
import javax.crypto.spec.PBEParameterSpec;


public class Test
{
    public static void main(String[] args) throws Exception
    {
        task01PBE();
        task02RandomKeyRSA();


    }

    public static void task01PBE() throws Exception{
        // Salt
        byte[] salt = {(byte) 0xc7, (byte) 0x73, (byte) 0x21, (byte) 0x8c, (byte) 0x7e, (byte) 0xc8, (byte) 0xee, (byte) 0x99};
        // Iteration count
        int count = 2048;

        //Initialization of the password
        char[] password = "123456".toCharArray();

        // Create PBE parameter set
        PBEParameterSpec pbeParamSpec = new PBEParameterSpec(salt, count);
        //Create parameter for key generation
        PBEKeySpec pbeKeySpec = new PBEKeySpec(password);
        // Create instance of SecretKeyFactory for password-based encryption
        SecretKeyFactory keyFac = SecretKeyFactory.getInstance("PBEWithMD5AndDES"); // using DES and MD5

        // Generate a key
        Key pbeKey = keyFac.generateSecret(pbeKeySpec);

        // Set a timer
        long timerStart = System.nanoTime();

        // Encryption
        // Create PBE Cipher
        Cipher pbeCipher = Cipher.getInstance("PBEWithMD5AndDES");
        // Initialize PBE Cipher with key and parameters
        pbeCipher.init(Cipher.ENCRYPT_MODE, pbeKey, pbeParamSpec);
        // Our plaintext
        String plaintext = "This is another example";
        byte[] cleartext = plaintext.getBytes();
        // Encrypt the plaintext
        byte[] ciphertext = pbeCipher.doFinal(cleartext);
        System.out.println("cipher : " + Utils.toHex(ciphertext));

        System.out.println("EncryptTime:" +  (System.nanoTime() - timerStart));



        // Yongheng's code
        // Reset timer
        timerStart = System.nanoTime();

        // Initialize PBE Cipher with key and parameters
        pbeCipher.init(Cipher.DECRYPT_MODE, pbeKey, pbeParamSpec);
        // decrypt the ciphertext
        byte[] newPlaintext = pbeCipher.doFinal(ciphertext); // Params is an encrypted byte array
        String StringPlaintext = new String (newPlaintext);
        System.out.println("Result: " + StringPlaintext);

        System.out.println("decryptTime:" +  (System.nanoTime() - timerStart));
    }

    public static void task02RandomKeyRSA() throws Exception{
        /* Security.addProvider(new BouncyCastleProvider()); */
        byte[]           input = new byte[] { (byte)0xbe, (byte)0xef };
        Cipher	         cipher = Cipher.getInstance("RSA/ECB/PKCS1Padding");
        SecureRandom     random = new SecureRandom();

        // create the keys
        KeyPairGenerator generator = KeyPairGenerator.getInstance("RSA");

        generator.initialize(512, random);

        KeyPair          pair = generator.generateKeyPair();
        Key              pubKey = pair.getPublic();
        Key              privKey = pair.getPrivate();

        System.out.println("input : " + Utils.toHex(input));

        // encryption step
        cipher.init(Cipher.ENCRYPT_MODE, pubKey); // Encrypt
        byte[] cipherText = cipher.doFinal(input); // Cipher Text
        System.out.println("cipher: " + Utils.toHex(cipherText));

        // decryption step
        cipher.init(Cipher.DECRYPT_MODE, privKey); // Decrypt
        byte[] plainText = cipher.doFinal(cipherText); // Plain Text
        System.out.println("plain : " + Utils.toHex(plainText));


        // System.out.printf("\n-------------------------------------------------\n");
        ////////////////////////////////////////////////////////// Task01
        ////////////////////////////////////////////////////////// Sender
        MessageDigest messageDigest = MessageDigest.getInstance("SHA-1");
        messageDigest.update(plainText); // 传入明文, 进行哈希计算
        byte[] digestHash = messageDigest.digest(); // 得到Hash值

        // 使用KeyPairGenerator生成RSA密钥对
        KeyPairGenerator newGenerator = KeyPairGenerator.getInstance("RSA");
        newGenerator.initialize(512, random);
        KeyPair newKeyPair = generator.generateKeyPair();
        Key newPublicKey = newKeyPair.getPublic();
        Key newPrivateKey = newKeyPair.getPrivate();

        // 用私钥加密哈希值,生成数字签名
        Cipher newCipher = Cipher.getInstance("RSA");
        newCipher.init(Cipher.ENCRYPT_MODE, newPrivateKey);
        byte[] signature = newCipher.doFinal(digestHash);

        ////////////////////////////////////////////////////////// Verifier
        // 解密签名 用公钥
        newCipher.init(Cipher.DECRYPT_MODE, newPublicKey);
        byte[] decrypt_signature = newCipher.doFinal(signature);

        // 验证: 比较解密的签名, 和发送方的签名
        System.out.printf("01: ");
        if (Arrays.equals(digestHash, decrypt_signature)){
            System.out.printf(("Same"));
        }else {
            System.out.printf("Not Same");
        }

        
        



        // System.out.printf("\n-------------------------------------------------\n");
        ///////////////////////////////////////////////////////////////////// Task02
        // Generating DSA KeyPair
        KeyPairGenerator keyPairGenerator = KeyPairGenerator.getInstance("DSA");
        keyPairGenerator.initialize(512);
        KeyPair keyPair02 = keyPairGenerator.generateKeyPair();

        //////////////////////////////////////////////////////////////////// Sender
        Signature sender_DSAHash = Signature.getInstance("SHA1withDSA");
        sender_DSAHash.initSign(keyPair02.getPrivate());
        sender_DSAHash.update(plainText);
        byte[] signature02 = sender_DSAHash.sign(); // 用私钥签名消息, 生成完全体签名

        // 消息, 签名, 公钥 都给接收方

        ////////////////////////////////////////////////////////////////// Receiver
        Signature receiver_DSAHash = Signature.getInstance("SHA1withDSA");
        receiver_DSAHash.initVerify(keyPair02.getPublic());
        receiver_DSAHash.update(plainText);
        boolean verify = receiver_DSAHash.verify(signature02);

        System.out.printf("02: " + verify);
    }

    public static String passwordGenerator(){
        return "";
    }
}