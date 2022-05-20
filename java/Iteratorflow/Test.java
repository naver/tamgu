import com.naver.jtamgu.*;
import java.io.*;
import java.util.*;
import java.nio.file.*;


/**
 *  Description of the Class
 *
 * @author     roux
 * @created    23 January 2017
 */

public class Test {
    int gHandler;
    JTamgu jatan;

    /**
     *  Description of the Method
     *
     * @param  grmFile  Description of the Parameter
     */
    public Test(String tamgufile) {
        try {
            System.out.println("Execute: " + tamgufile);
            // create a JTamgu object to load the dynamic library in the VM
            jatan = new JTamgu();
            String version = jatan.TamguVersion();
            System.out.println("Version is:"+version);
            String code = new String(Files.readAllBytes(Paths.get(tamgufile)));
            System.out.println(code);
            int h=jatan.LoadProgramAsString(code,"");
            System.out.println("Handler:"+h);
            String[] arg=new String[0];
            float[] valeurs = new float[320];
            for (int i = 0; i < 320; i++) {
                valeurs[i] = i;
            }
            
            String[] others = new String[10];
            others[0] = "First";
            others[1] = "we";
            others[2] = "read";
            others[3] = "a";
            others[4] = "book";
            others[5] = "with";
            others[6] = "some";
            others[7] = "nice";
            others[8] = "images";
            others[9] = ".";

            jatan.Setfloatiterator(h, 0, "jv", valeurs);
            jatan.Setstringiterator(h, 0, "its", others);
            String res=jatan.ExecuteFunction(h, "rappel",arg);
            System.out.println("Result:"+res);
        }
        catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }

    /**
     *  Description of the Method
     *
     * @param  args  Description of the Parameter
     */
    public final static void main(String args[]) {
        try {
            Test test = new Test(args[0]);
        } catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }
}

