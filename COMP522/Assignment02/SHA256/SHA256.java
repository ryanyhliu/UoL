package Assignment02.SHA256;

import java.security.MessageDigest;

public class SHA256 {
    public static void main(String[] args) throws Exception {
        Sender sender = new Sender();
        Receiver receiver = new Receiver();

        String message = "This is a secret message";
        byte[] digest = sender.generateDigest(message);
        
        long startTime = System.nanoTime();
        boolean isValid = receiver.verifyDigest(digest, message);
        long endTime = System.nanoTime();

        System.out.println("Is message valid? " + isValid);
        System.out.println("Verification time: " + (endTime - startTime) + " nanoseconds");
    }
}

class Sender {
    public byte[] generateDigest(String message) throws Exception {
        MessageDigest sha256 = MessageDigest.getInstance("SHA-256");
        sha256.update(message.getBytes());
        return sha256.digest();
    }
}

class Receiver {
    public boolean verifyDigest(byte[] digest, String message) throws Exception {
        MessageDigest sha256 = MessageDigest.getInstance("SHA-256");
        sha256.update(message.getBytes());
        byte[] calculatedDigest = sha256.digest();
        return MessageDigest.isEqual(digest, calculatedDigest);
    }
}
