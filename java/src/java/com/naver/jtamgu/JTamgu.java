/*
 *  NAVER LABS Europe
 *  Project: Tamgu
 *  Copyright (C) 2017 NAVER LABS Europe, All Rights Reserved
 *
 */

package com.naver.jtamgu;

import java.io.File;
import java.util.Vector;

/**
 * Description of the Class
 *
 * @author roux @created April 10, 2017
 */
public class JTamgu {

   /**
     * Constructor for the JTamgu object
     */
    public JTamgu() {
        try {
            DynLoaderFromJar.loadLibFromJar();
        }
        catch(Throwable ex) {
            System.out.println("Exception during loading 'jtamgu'");
        }
    }

    public JTamgu(boolean withOutLoading) {
        if (withOutLoading == false) {
            try {
                DynLoaderFromJar.loadLibFromJar();
            }
            catch(Throwable ex) {
                System.out.println("Exception during loading 'jtamgu'");
            }
        }
    }

    //------------------------------------------------------------------------

    private native int LoadProgramImplementation(String filename,String args) throws Exception;

    /**
     * Load a Tamgu program
     *
     * @param filename of the Tamgu program to load
     * @param args arguments as a string, where each parameter is separated with a space
     * @return 
     * @exception Exception
     */
    public synchronized int LoadProgram(String filename,String args) throws Exception {
        return LoadProgramImplementation(filename, args);
    }
    //------------------------------------------------------------------------
    
    private native int LoadStringProgramImplementation(String code,String args) throws Exception;
    
    /**
     * Load a Tamgu program
     *
     * @param filename of the Tamgu program to load
     * @param args arguments as a string, where each parameter is separated with a space
     * @return
     * @exception Exception
     */
    public synchronized int LoadProgramAsString(String code,String args) throws Exception {
        return LoadStringProgramImplementation(code, args);
    }
    //------------------------------------------------------------------------

    private native String ExecuteFunctionImplementation(int handler, String code, String[] args)
            throws Exception;

    /**
     * Execute a Tamgu program from String
     *
     * @param code of the Tamgu program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return 
     * @exception Exception
     */
    public synchronized String ExecuteFunction(int handler, String code,String[] args) throws Exception {
        return ExecuteFunctionImplementation(handler, code, args);
    }
    //------------------------------------------------------------------------

    private native String[] ExecuteFunctionArrayImplementation(int handler, String code, String[] args)
            throws Exception;

    /**
     * Execute a Tamgu program from String
     *
     * @param code of the Tamgu program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return 
     * @exception Exception
     */
    public synchronized String[] ExecuteFunctionArray(int handler, String code,String[] args) throws Exception {
        return ExecuteFunctionArrayImplementation(handler, code, args);
    }

    //------------------------------------------------------------------------

    private native int CleanImplementation(int handler) throws Exception;

    /**
     * Clean One Tamgu Program
     *
     */
    public synchronized int Clean(int handler) throws Exception {
        return CleanImplementation(handler);
    }

    //------------------------------------------------------------------------

    private native int CleanAllImplementation() throws Exception;

    /**
     * Clean all Tamgu Programs
     *
     */
    public synchronized int CleanAll() throws Exception {
        return CleanAllImplementation();
    }
    
    //------------------------------------------------------------------------

    private native String TamguVersionImplementation() throws Exception;
    
    /**
     * Return Version Number of Current Tamgu Implementation
     *
     */
    public synchronized String TamguVersion() throws Exception {
        return TamguVersionImplementation();
    }
}
