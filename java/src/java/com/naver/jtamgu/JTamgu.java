/*
 *  NAVER LABS Europe
 *  Project: Tamgu
 *  Copyright (C) 2017 NAVER LABS Europe, All Rights Reserved
 *
 */

package com.naver.jtamgu;

import java.io.File;
import java.util.Vector;
import java.util.Map;
import java.util.List;

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
    private native boolean GetErrorImplementation(int handler, List<String> messages, List<String> files, List<Integer> lines) throws Exception;

    /**
     * Load a Tamgu program
     *
     * @param filename of the Tamgu program to load
     * @param args arguments as a string, where each parameter is separated with a space
     * @return
     * @exception Exception
     */
    public synchronized boolean GetError(int handler, List<String> messages, List<String> files, List<Integer> lines) throws Exception {
        return GetErrorImplementation(handler, messages, files, lines);
    }
    //------------------------------------------------------------------------
    private native boolean CheckProgramImplementation(String filename,String args, List<String> messages, List<Integer> lines, List<Integer> positions) throws Exception;

    /**
     * Load a Tamgu program
     *
     * @param filename of the Tamgu program to load
     * @param args arguments as a string, where each parameter is separated with a space
     * @return
     * @exception Exception
     */
    public synchronized boolean CheckProgram(String filename,String args, List<String> messages, List<Integer> lines, List<Integer> positions) throws Exception {
        return CheckProgramImplementation(filename, args, messages, lines, positions);
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
    
    private native int LoadAStringProgramImplementation(int handler, String code) throws Exception;
    
    /**
     * Load a Tamgu program
     *
     * @param filename of the Tamgu program to load
     * @param args arguments as a string, where each parameter is separated with a space
     * @return
     * @exception Exception
     */
    public synchronized int LoadAProgramAsString(int handler, String code) throws Exception {
        return LoadAStringProgramImplementation(handler, code);
    }
    //------------------------------------------------------------------------
    private native int CountinstancesImplementation() throws Exception;

    /**
     * Returns the number of active instances in memory
     *
     * @exception Exception
     */
    public synchronized int Countinstances() throws Exception {
        return CountinstancesImplementation();
    }
    //------------------------------------------------------------------------
    private native int CountactiveinstancesImplementation() throws Exception;

    /**
     * Returns the number of active instances in memory
     *
     * @exception Exception
     */
    public synchronized int Countactiveinstances() throws Exception {
        return CountactiveinstancesImplementation();
    }
    //------------------------------------------------------------------------
    private native int MaxcountinstancesImplementation() throws Exception;

    /**
     * Returns the number of active instances in memory
     *
     * @exception Exception
     */
    public synchronized int Maxcountinstances() throws Exception {
        return MaxcountinstancesImplementation();
    }
    //------------------------------------------------------------------------
    private native int MaxcountactiveinstancesImplementation() throws Exception;

    /**
     * Returns the number of active instances in memory
     *
     * @exception Exception
     */
    public synchronized int Maxcountactiveinstances() throws Exception {
        return MaxcountactiveinstancesImplementation();
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

    private native String ExecuteSpaceFunctionImplementation(int handler, int idcode, String code, String[] parameters)
            throws Exception;

    /**
     * Execute a Tamgu program from String
     *
     * @param code of the Tamgu program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return
     * @exception Exception
     */
    public synchronized String ExecuteSpaceFunction(int handler, int idcode, String code, String[] parameters) throws Exception {
        return ExecuteSpaceFunctionImplementation(handler, idcode, code, parameters);
    }
    //------------------------------------------------------------------------

    private native String[] ExecuteSpaceFunctionArrayImplementation(int handler, int idcode, String code, String[] parameters)
            throws Exception;

    /**
     * Execute a Tamgu program from String
     *
     * @param code of the Tamgu program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return
     * @exception Exception
     */
    public synchronized String[] ExecuteSpaceFunctionArray(int handler, int idcode, String code,String[] parameters) throws Exception {
        return ExecuteSpaceFunctionArrayImplementation(handler, idcode, code, parameters);
    }
    //------------------------------------------------------------------------

    private native String ExecuteFunctionImplementationWithGlobals(int handler, int idcode, Map<String, String> globalvariables, String code, String[] args)
            throws Exception;

    /**
     * Execute a Tamgu program from String
     *
     * @param code of the Tamgu program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return
     * @exception Exception
     */
    public synchronized String ExecuteFunctionWithGlobals(int handler, int idcode, Map<String, String> globalvariables, String code, String[] args) throws Exception {
        return ExecuteFunctionImplementationWithGlobals(handler, idcode, globalvariables, code, args);
    }
    //------------------------------------------------------------------------
    private native boolean Setfloatiteratorimplementation(int handler, int idcode, String name, float[] args)
            throws Exception;

    /**
     * Execute a Tamgu program from String
     *
     * @param code of the Tamgu program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return
     * @exception Exception
     */
    public synchronized boolean Setfloatiterator(int handler, int idcode, String name, float[] args) throws Exception {
        return Setfloatiteratorimplementation(handler, idcode, name, args);
    }
    //------------------------------------------------------------------------
    private native boolean Setlongiteratorimplementation(int handler, int idcode, String name, long[] args)
            throws Exception;

    /**
     * Execute a Tamgu program from String
     *
     * @param code of the Tamgu program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return
     * @exception Exception
     */
    public synchronized boolean Setlongiterator(int handler, int idcode, String name, long[] args) throws Exception {
        return Setlongiteratorimplementation(handler, idcode, name, args);
    }
    //------------------------------------------------------------------------
    private native boolean Setdoubleiteratorimplementation(int handler, int idcode, String name, double[] args)
            throws Exception;

    /**
     * Execute a Tamgu program from String
     *
     * @param code of the Tamgu program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return
     * @exception Exception
     */
    public synchronized boolean Setdoubleiterator(int handler, int idcode, String name, double[] args) throws Exception {
        return Setdoubleiteratorimplementation(handler, idcode, name, args);
    }
    //------------------------------------------------------------------------
    private native void TamguIdleImplementation(int diff, List<Integer> idles) throws Exception;
    /**
    * Detect idle Tamgu interpreters
    *
    * @param diff is the time in seconds that serves as a time limit
    * @param idles list of Tamgu idle handlers
    * @exception Exception
    */
    public synchronized void TamguIdle(int diff, List<Integer> idles) throws Exception {
        TamguIdleImplementation(diff, idles);
    }
    //------------------------------------------------------------------------
    private native boolean Setstringiteratorimplementation(int handler, int idcode, String name, String[] args)
            throws Exception;

    /**
     * Execute a Tamgu program from String
     *
     * @param code of the Tamgu program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return
     * @exception Exception
     */
    public synchronized boolean Setstringiterator(int handler, int idcode, String name, String[] args) throws Exception {
        return Setstringiteratorimplementation(handler, idcode, name, args);
    }
    //------------------------------------------------------------------------
    private native boolean SetListFloatiteratorimplementation(int handler, int idcode, String name, List<Float> args)
            throws Exception;

    /**
     * Execute a Tamgu program from String
     *
     * @param code of the Tamgu program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return
     * @exception Exception
     */
    public synchronized boolean SetListFloatiterator(int handler, int idcode, String name, List<Float> args) throws Exception {
        return SetListFloatiteratorimplementation(handler, idcode, name, args);
    }
    //------------------------------------------------------------------------
    private native boolean SetListIntiteratorimplementation(int handler, int idcode, String name, List<Integer> args)
            throws Exception;

    /**
     * Execute a Tamgu program from String
     *
     * @param code of the Tamgu program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return
     * @exception Exception
     */
    public synchronized boolean SetListIntiterator(int handler, int idcode, String name, List<Integer> args) throws Exception {
        return SetListIntiteratorimplementation(handler, idcode, name, args);
    }
    //------------------------------------------------------------------------
    private native boolean SetListDoubleiteratorimplementation(int handler, int idcode, String name, List<Double> args)
            throws Exception;

    /**
     * Execute a Tamgu program from String
     *
     * @param code of the Tamgu program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return
     * @exception Exception
     */
    public synchronized boolean SetListDoubleiterator(int handler, int idcode, String name, List<Double> args) throws Exception {
        return SetListDoubleiteratorimplementation(handler, idcode, name, args);
    }
    //------------------------------------------------------------------------
    private native boolean SetListLongiteratorimplementation(int handler, int idcode, String name, List<Long> args)
            throws Exception;

    /**
     * Execute a Tamgu program from String
     *
     * @param code of the Tamgu program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return
     * @exception Exception
     */
    public synchronized boolean SetListLongiterator(int handler, int idcode, String name, List<Long> args) throws Exception {
        return SetListLongiteratorimplementation(handler, idcode, name, args);
    }
    //------------------------------------------------------------------------
    private native boolean SetListStringiteratorimplementation(int handler, int idcode, String name, List<String> args)
            throws Exception;

    /**
     * Execute a Tamgu program from String
     *
     * @param code of the Tamgu program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return
     * @exception Exception
     */
    public synchronized boolean SetListStringiterator(int handler, int idcode, String name, List<String> args) throws Exception {
        return SetListStringiteratorimplementation(handler, idcode, name, args);
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

    private native int CleanErrorsImplementation(int handler) throws Exception;

    /**
     * Clean One Tamgu Program
     *
     */
    public synchronized int CleanErrors(int handler) throws Exception {
        return CleanErrorsImplementation(handler);
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
    //------------------------------------------------------------------------
    private native void TamguSetGarbageModeImplementation(int setgarbage) throws Exception;
    
    /**
     * Set the garbage mode
     *
     */
    public synchronized void TamguSetGarbageMode(int setgarbage) throws Exception {
        TamguSetGarbageModeImplementation(setgarbage);
    }

    private native void ScanGarbageImplementation(Map<String, Integer> garbage) throws Exception;
    
    /**
     * Set the garbage mode
     *
     */
    public synchronized void ScanGarbage(Map<String, Integer> garbage) throws Exception {
        ScanGarbageImplementation(garbage);
    }

    private native void ScanGarbagelongImplementation(Map<Integer, Integer> garbage) throws Exception;
    
    /**
     * Set the garbage mode
     *
     */
    public synchronized void ScanGarbagelong(Map<Integer, Integer> garbage) throws Exception {
        ScanGarbagelongImplementation(garbage);
    }
    //------------------------------------------------------------------------
    
    private native void TamguCatchJNICrashImplementation() throws Exception;
        
    /**
     * Return Version Number of Current Tamgu Implementation
     *
     */
    public synchronized void TamguCatchJNICrash() throws Exception {
        TamguCatchJNICrashImplementation();
    }

}
