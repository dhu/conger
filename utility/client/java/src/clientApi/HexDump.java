 import java.io.File;
 import java.io.FileInputStream;
 import java.io.FileNotFoundException;
 import java.io.IOException;
 import java.io.UnsupportedEncodingException;


 public class HexDump {


    public static void main(String[] args) {
        try {
            String fileName = args[0];
            try {
                int width = 16;
                byte[] bytes = read(fileName);
                for (int index = 0; index < bytes.length; index += width) {
                    printHex(bytes, index, width);
                    printAscii(bytes, index, width);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        } catch (Exception e) {
            System.out.println("borealis.client.HexDump [filename]");
        }
    }
 
    private static byte[] read(String inputFileName)
        throws FileNotFoundException, IOException {
        File theFile = new File(inputFileName);
        FileInputStream input = new FileInputStream(theFile);
        int length = (int)theFile.length();
        byte[] bytes = new byte[length];
        int bytesRead = 0;
        while (bytesRead < bytes.length) {
            bytesRead = input.read(bytes, bytesRead, bytes.length - bytesRead);
            if (bytesRead == -1) {
                break;
            }
        }
        return bytes;
    }


    private static void printHex(byte[] bytes, int offset, int width) {
        for (int index = 0; index < width; index++) {
            if (index + offset < bytes.length) {
                String hexString = Integer.toHexString(bytes[index + offset]);
                System.out.print((hexString.length() < 2 ? "0" : "") + hexString + " ");
            } else {
                System.out.print("  ");
            }
        }
    }


    private static void printAscii(byte[] bytes, int index, int width)
        throws Exception {
        if (index < bytes.length) {
            width = (index + width) > bytes.length ? bytes.length - index : width;
            System.out.println(
                ":"
                    + new String(bytes, index, width, "UTF-8").replaceAll("\r\n", " ").replaceAll(
                        "\n",
                        " "));
        } else {
            System.out.println();
        }
    }

}

