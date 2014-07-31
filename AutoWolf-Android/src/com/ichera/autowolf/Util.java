package com.ichera.autowolf;

import java.io.File;
import java.util.Locale;
import java.util.Set;
import java.util.TreeSet;

public class Util 
{
	public static final Set<String> WOLF_FILES;
	
	static
	{
		WOLF_FILES = new TreeSet<String>();
		WOLF_FILES.add("gamemaps");
		WOLF_FILES.add("maphead");
		WOLF_FILES.add("vswap");
		WOLF_FILES.add("vgagraph");
		WOLF_FILES.add("audiohed");
		WOLF_FILES.add("audiot");
		WOLF_FILES.add("vgadict");
		WOLF_FILES.add("vgahead");
	}
	
	public static boolean dirIsValid(String dir)
	{
		if(dir == null || dir.length() == 0)
			return false;
		return dirIsValid(new File(dir));
	}
	
	public static boolean dirIsValid(File f)
	{
		if(!f.isDirectory())
			return false;
		return fileListContainsWolf(f.listFiles());
	}
	
	public static boolean fileIsOfWolf(String t)
	{
		if(t.length() > 4 && 
				WOLF_FILES.contains(
						t.substring(0, t.length() - 4)
						.toLowerCase(Locale.US)))
		{
			return true;
		}
		return false;
	}
	
	public static boolean fileListContainsWolf(File[] fs)
	{
		if(fs == null || fs.length < 8)
			return false;
		int eqcount = 0;
		for(File g : fs)
		{
			String t = g.getName();
			if(fileIsOfWolf(t))
				++eqcount;
		}
		
		if(eqcount >= WOLF_FILES.size())
			return true;
		
		return false;
	}
}
