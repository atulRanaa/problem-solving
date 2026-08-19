import java.util.*;
import java.io.*;
class TestClass {
    public static void main(String args[] ) throws Exception {
        Scanner s = new Scanner(System.in);
        long t = s.nextLong();
        long i;
           while (t-- > 0) {
                long  n = s.nextLong();

                for (i = 0; (n & (1<<i)) > 0; ++i);
                System.out.println((n | (1<<i)));
           }
    }
}

