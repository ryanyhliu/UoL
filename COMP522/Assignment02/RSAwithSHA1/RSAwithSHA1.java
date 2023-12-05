package Assignment02.RSAwithSHA1;

import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.MessageDigest;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.SecureRandom;
import javax.crypto.Cipher;




public class RSAwithSHA1 {
    public static void main(String[] args) throws Exception {
        Sender sender = new Sender();
        Verifier verifier = new Verifier();

        String message = "This is a secret message";

        
        byte[] digest = sender.generateDigest(message);
        byte[] encryptedDigest = sender.encryptDigest(digest);
        
        // Set timer
        long startTime = System.nanoTime();
        boolean isValid = verifier.verify(encryptedDigest, message.getBytes(), sender.getPublicKey());
        long endTime = System.nanoTime();

        System.out.println("Is message valid? " + isValid);
        System.out.println("Verification time: " + (endTime - startTime) + " nanoseconds");
        
    }
}


/**
 * 生成 摘要Digest
 * 生成 RSA KeyPair
 * 加密 Digest
 */
class Sender {
    private PublicKey publicKey;
    private PrivateKey privateKey;

    public Sender() throws Exception {
        // RSA KeyPair 生成器
        KeyPairGenerator keyPairgenerator = KeyPairGenerator.getInstance("RSA"); 
        SecureRandom random = new SecureRandom();
        keyPairgenerator.initialize(512, random);
        // 生成 RSA KeyPair
        KeyPair pair = keyPairgenerator.generateKeyPair();
        this.privateKey = pair.getPrivate();
        this.publicKey = pair.getPublic();
    }

    /**
     * digest -RSA-> encryptedDigest
     * @param digest
     * @return encrypedDigest
     * @throws Exception
     */
    public byte[] encryptDigest(byte[] digest) throws Exception {
        Cipher cipher = Cipher.getInstance("RSA/ECB/PKCS1Padding");
        cipher.init(Cipher.ENCRYPT_MODE, privateKey);
        return cipher.doFinal(digest);
    }

    public PublicKey getPublicKey() {
        return publicKey;
    }

    /**
     * text -SHA1-> digest
     * @param message 消息原文
     * @return digest
     * @throws Exception
     */
    public byte[] generateDigest(String message) throws Exception {
        MessageDigest sha1 = MessageDigest.getInstance("SHA1");
        sha1.update(message.getBytes());
        return sha1.digest();
    }
}

class Verifier {
    /**
     * 因为Hash不可逆 (SHA1)
     * 所以只能比较digest (text -SHA1-> digest -RSA-> encryptedDigest)
     * @param originalDigest 接收到的digest
     * @param message 真实的原文 (text)
     * @param pubKey
     * @return
     * @throws Exception
     */
    public boolean verify(byte[] originalDigest, byte[] message, PublicKey pubKey) throws Exception {
        // 解密摘要 encryptedDigest -RSA-> digest
        Cipher cipher = Cipher.getInstance("RSA/ECB/PKCS1Padding");
        cipher.init(Cipher.DECRYPT_MODE, pubKey);
        byte[] decryptedDigest = cipher.doFinal(originalDigest);

        // 计算消息的摘要 text -SHA1-> digest
        MessageDigest sha1 = MessageDigest.getInstance("SHA1");
        sha1.update(message);
        byte[] messageDigest = sha1.digest();

        // 比较摘要
        return MessageDigest.isEqual(decryptedDigest, messageDigest);
    }
}


