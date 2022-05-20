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
    public Test(String tamgufile, String externalfile) {
        try {
            System.out.println("Execute: " + tamgufile);
            // create a JTamgu object to load the dynamic library in the VM
            jatan = new JTamgu();

            //First we read the main code, where the frame description is
            String code = new String(Files.readAllBytes(Paths.get(tamgufile)));
            int h = jatan.LoadProgramAsString(code,"");
            System.out.println("Handler:"+h);

            //Second we read the code where the function is
            String external_code = new String(Files.readAllBytes(Paths.get(externalfile)));
            int idcode = jatan.LoadAProgramAsString(h, external_code);
            System.out.println("IdCode:"+idcode);

            
            String[] arg=new String[1];
            arg[0] = "333";
            
            String res=jatan.ExecuteSpaceFunction(h, idcode, "rappel", arg);
            System.out.println("Result:"+res);
            
            Map<String, String> globals = new HashMap<>();
            globals.put("nb", "999");
            arg[0] = "1";

            res=jatan.ExecuteFunctionWithGlobals(h, idcode, globals, "modifie",arg);
            System.out.println("Result:"+res);
            for (Map.Entry<String, String> entry : globals.entrySet()) {
                System.out.println(entry.getKey() + " --> " + entry.getValue());
            }
            
            List<Float> list = new ArrayList<Float>();
            list.add(10.9f);
            list.add(10.8f);
            list.add(0.9f);
            list.add(100f);
            list.add(10f);
            list.add(900f);
            list.add(800f);
            list.add(110f);
            list.set(1, 999f);

            jatan.SetListFloatiterator(h, idcode, "jvect", list);
            jatan.ExecuteSpaceFunction(h, idcode, "modification", arg);
            
            for (int i = 0; i < list.size(); i++) {
                System.out.println("" +i+"_"+ list.get(i));
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
            Test test = new Test(args[0], args[1]);
        } catch (Exception ex) {
            System.out.println(ex);
            ex.printStackTrace();
        }
    }
}

