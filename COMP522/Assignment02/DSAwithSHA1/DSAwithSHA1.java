package Assignment02.DSAwithSHA1;

import java.security.*;



public class DSAwithSHA1 {
    public static void main(String[] args) throws Exception {
        Sender sender = new Sender();
        Receiver receiver = new Receiver();

        String message = "This is a secret message";
        byte[] signature = sender.signMessage(message);

        long startTime = System.nanoTime();
        boolean isValid = receiver.verifySignature(sender.getPublicKey(), signature, message);
        long endTime = System.nanoTime();

        System.out.println("Is signature valid? " + isValid);
        System.out.println("Verification time: " + (endTime - startTime) + " nanoseconds");
    }
}



class Sender {
    private PrivateKey privateKey;
    private PublicKey publicKey;

    public Sender() throws Exception {
        KeyPairGenerator keyGen = KeyPairGenerator.getInstance("DSA");
        SecureRandom random = SecureRandom.getInstance("SHA1PRNG");
        keyGen.initialize(1024, random);
        KeyPair pair = keyGen.generateKeyPair();
        privateKey = pair.getPrivate();
        publicKey = pair.getPublic();
    }

    public byte[] signMessage(String message) throws Exception {
        Signature dsa = Signature.getInstance("SHA1withDSA");
        dsa.initSign(privateKey);
        dsa.update(message.getBytes());
        return dsa.sign();
    }

    public PublicKey getPublicKey() {
        return publicKey;
    }
}


class Receiver {
    public boolean verifySignature(PublicKey publicKey, byte[] signature, String message) throws Exception {
        Signature dsa = Signature.getInstance("SHA1withDSA");
        dsa.initVerify(publicKey);
        dsa.update(message.getBytes());
        return dsa.verify(signature);
    }
}
