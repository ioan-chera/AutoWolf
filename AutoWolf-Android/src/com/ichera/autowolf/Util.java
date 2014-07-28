package com.ichera.autowolf;

import java.io.File;

public class Util 
{
	private static final String[] WOLF_FILES = new String[]
			{
				"gamemaps",
				"maphead",
				"vswap",
				"vgagraph",
				"audiohed",
				"audiot",
				"vgadict",
				"vgahead"
			};
	
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
	
	public static boolean fileListContainsWolf(File[] fs)
	{
		if(fs == null || fs.length < 8)
			return false;
		int eqcount = 0;
		for(File g : fs)
		{
			String t = g.getName();
			for(String s : WOLF_FILES)
			{
				if(t.length() < s.length())
					continue;
				if(t.substring(0, s.length()).equalsIgnoreCase(s))
				{
					eqcount++;
					break;
				}
			}
		}
		
		if(eqcount >= WOLF_FILES.length)
			return true;
		
		return false;
	}
}
