package Old;
// swers for you…
//
// 这个代码是用来演示PBE加密算法的用法和效果的。
// PBE是Password-Based Encryption的缩写，意思是基于密码的加密。
// 它是一种利用用户提供的密码来生成强大的密钥（key）的方法。
// 密钥是一种二进制数据，用来对明文（plaintext）进行加密或解密，得到密文（ciphertext）。
// PBE算法使用用户的密码和一些额外的输入参数：
// 盐（salt）和迭代次数（iteration count），来产生随机且不可预测的密钥字节1。

/*
import java.security.MessageDigest;
import java.security.SecureRandom;
*/


/**
 * Updated Utility class
 */
public class Utils {
    private static final String digits = "0123456789abcdef";

    /**
     * Return length many bytes of the passed in byte array as a hex string.
     *
     * @param data   the bytes to be converted.
     * @param length the number of bytes in the data block to be converted.
     * @return a hex representation of length bytes of data.
     */
    public static String toHex(byte[] data, int length) {
        StringBuffer buf = new StringBuffer();

        for (int i = 0; i != length; i++) {
            int v = data[i] & 0xff;

            buf.append(digits.charAt(v >> 4));
            buf.append(digits.charAt(v & 0xf));
        }

        return buf.toString();
    }

    /**
     * Return the passed in byte array as a hex string.
     *
     * @param data the bytes to be converted.
     * @return a hex representation of data.
     */
    public static String toHex(byte[] data) {
        return toHex(data, data.length);
    }

    /**
     * Convert the passed in String to a byte array by
     * taking the bottom 8 bits of each character it contains.
     *
     * @param string the string to be converted
     * @return a byte array representation
     */
    public static byte[] toByteArray(
            String string) {
        byte[] bytes = new byte[string.length()];
        char[] chars = string.toCharArray();

        for (int i = 0; i != chars.length; i++) {
            bytes[i] = (byte) chars[i];
        }

        return bytes;
    }


}