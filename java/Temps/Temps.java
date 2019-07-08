import com.naver.jtamgu.*;
import java.io.*;
import java.util.*;

/**
*  Description of the Class
*
* @author     roux
* @created    4 Juin 2018
*/

public class Temps {
    /**
    *  Description of the Method
    *
    * @param  grmFile  Description of the Parameter
    */
    public Temps(long mxi, long mxj) {
        long i;
        long j;
        long k=0;
        long lastRefreshTime1 = System.currentTimeMillis();
        int pos=0;

        long tableau[] = new long[2000*2000];
        for (i=0; i< mxi; i+=1) {
            for (j=0; j< mxj; j+=1) {
                k=5*(i+j)*(i-j+23)*i*j;
                tableau[pos]=k;
                pos++;
            }
        }

        long lastRefreshTime2 = System.currentTimeMillis();        
        long diff=lastRefreshTime2-lastRefreshTime1;
        System.out.println(java.util.Arrays.toString(tableau));
        System.out.println("Temps:"+diff);

    }



    /**
    *  Description of the Method
    *
    * @param  args  Description of the Parameter
    */
    public final static void main(String args[]) {
        Temps test = new Temps(Integer.parseInt(args[0]),Integer.parseInt(args[1]));
    }
}



