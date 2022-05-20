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
            Map<String, String> globals = new HashMap<>();
            globals.put("s", "Mon S");
            globals.put("i", "999");
            System.out.println("Execute: " + tamgufile);
            // create a JTamgu object to load the dynamic library in the VM
            jatan = new JTamgu();
            String version = jatan.TamguVersion();
            System.out.println("Version is:"+version);
            String code = new String(Files.readAllBytes(Paths.get(tamgufile)));
            System.out.println(code);
            int h=jatan.LoadProgramAsString(code,"");
            System.out.println("Handler:"+h);
            String[] arg=new String[2];
            arg[0]="100";
            arg[1]="mon test";
            String res=jatan.ExecuteFunctionWithGlobals(h, 0, globals, "rappel",arg);
            System.out.println("Result:"+res);
            for (Map.Entry<String, String> entry : globals.entrySet()) {
                System.out.println(entry.getKey() + ":" + entry.getValue());
            }
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

