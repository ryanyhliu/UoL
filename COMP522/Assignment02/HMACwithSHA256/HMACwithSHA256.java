package Assignment02.HMACwithSHA256;

import java.security.MessageDigest;

import javax.crypto.*;

public class HMACwithSHA256 {
    public static void main(String[] args) throws Exception {
        Sender sender = new Sender();
        Receiver receiver = new Receiver();

        String message = "This is a secret message";
        byte[] hmac = sender.createHMAC(message);

        long startTime = System.nanoTime();
        boolean isValid = receiver.verifyHMAC(sender.getSecretKey(), hmac, message);
        long endTime = System.nanoTime();

        System.out.println("Is HMAC valid? " + isValid);
        System.out.println("Verification time: " + (endTime - startTime) + " nanoseconds");
    }
}

class Sender {
    private SecretKey secretKey;

    public Sender() throws Exception {
        KeyGenerator keyGenerator = KeyGenerator.getInstance("HmacSHA256");
        secretKey = keyGenerator.generateKey();
    }

    public byte[] createHMAC(String message) throws Exception {
        Mac mac = Mac.getInstance("HmacSHA256");
        mac.init(secretKey);
        return mac.doFinal(message.getBytes());
    }

    public SecretKey getSecretKey() {
        return secretKey;
    }
}

class Receiver {
    public boolean verifyHMAC(SecretKey secretKey, byte[] hmac, String message) throws Exception {
        Mac mac = Mac.getInstance("HmacSHA256");
        mac.init(secretKey);
        byte[] calculatedHmac = mac.doFinal(message.getBytes());
        return MessageDigest.isEqual(hmac, calculatedHmac);
    }
}
