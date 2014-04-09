package com.ichera.autowolf;

import java.io.File;
import java.util.ArrayList;

import org.libsdl.app.SDLActivity;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Environment;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.TextView;

import com.ichera.lib.filemanager.OpenActivity;

public class MainActivity extends Activity implements View.OnClickListener,
TextWatcher, CompoundButton.OnCheckedChangeListener
{
	private static final String PREFS_NAME = "AutoWolfLaunchPreferences";
	
	private static final String PREF_WOLFDIR = "wolfdir";
	private static final String PREF_TEDLEVEL = "tedlevel";
	private static final String PREF_SKILL = "skill";
	private static final String PREF_SECRETSTEP3 = "secretstep3";
	
	private static final int REQUEST_OPEN_FOLDER = 1;
	
	private String mWolfdir;
	private int mTedlevel;
	private int mSkillLevel;
	private boolean mSecretStep3;
	
	private Button mChooseButton;
	private EditText mWarpField;
	
	private RadioButton mBabyButton;
	private RadioButton mEasyButton;
	private RadioButton mMediumButton;
	private RadioButton mHardButton;
	
	private CheckBox mSecretStep3Box;
	
	private Button mStartButton;
	
	private ArrayList<String> mArgs;
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		
		mChooseButton = (Button)findViewById(R.id.choose_wolfdir_button); 
		mWarpField = (EditText)findViewById(R.id.warp_field);
		
		mBabyButton = (RadioButton)findViewById(R.id.baby_button);
		mEasyButton = (RadioButton)findViewById(R.id.easy_button);
		mMediumButton = (RadioButton)findViewById(R.id.medium_button);
		mHardButton = (RadioButton)findViewById(R.id.hard_button);
		
		mSecretStep3Box = (CheckBox)findViewById(R.id.check_secretstep3);
		
		mStartButton = (Button)findViewById(R.id.start_button);
		
		mChooseButton.setOnClickListener(this);
		mWarpField.addTextChangedListener(this);
		
		mBabyButton.setOnCheckedChangeListener(this);
		mEasyButton.setOnCheckedChangeListener(this);
		mMediumButton.setOnCheckedChangeListener(this);
		mHardButton.setOnCheckedChangeListener(this);
		
		mSecretStep3Box.setOnCheckedChangeListener(this);
		
		mStartButton.setOnClickListener(this);
		
		getData();
	}
	
	private void getData()
	{
		SharedPreferences settings = getSharedPreferences(PREFS_NAME, 0);
		
		mWolfdir = settings.getString(PREF_WOLFDIR, "");
		mTedlevel = settings.getInt(PREF_TEDLEVEL, 0);
		mSkillLevel = settings.getInt(PREF_SKILL, 3);
		mSecretStep3 = settings.getBoolean(PREF_SECRETSTEP3, false);
		
		updateUi();
	}
	
	private void putData()
	{
		SharedPreferences.Editor editor = getSharedPreferences(PREFS_NAME, 0).edit();
		
		if(mWolfdir != null && mWolfdir.length() > 0)
			editor.putString(PREF_WOLFDIR, mWolfdir);
		editor.putInt(PREF_TEDLEVEL, mTedlevel);
		editor.putInt(PREF_SKILL, mSkillLevel);
		editor.putBoolean(PREF_SECRETSTEP3, mSecretStep3);
		
		editor.commit();
	}
	
	private void updateUi()
	{
		updateUi(false);
	}
	private void updateUi(boolean mind)
	{
		if(mWolfdir == null || mWolfdir.length() == 0)
		{
			((TextView)findViewById(R.id.current_wolfdir)).setText(R.string.not_chosen_label);
			((Button)findViewById(R.id.start_button)).setEnabled(false);
		}
		else
		{
			((TextView)findViewById(R.id.current_wolfdir)).setText(mWolfdir);
			((Button)findViewById(R.id.start_button)).setEnabled(true);
		}
		if(!mind)
			mWarpField.setText(String.valueOf(mTedlevel));
		switch(mSkillLevel)
		{
		case 0:
			((RadioButton)findViewById(R.id.baby_button)).setChecked(true);
			break;
		case 1:
			((RadioButton)findViewById(R.id.easy_button)).setChecked(true);
			break;
		case 2:
			((RadioButton)findViewById(R.id.medium_button)).setChecked(true);
			break;
		default:
			((RadioButton)findViewById(R.id.hard_button)).setChecked(true);
			break;	
		}
		mSecretStep3Box.setChecked(mSecretStep3);
	}

	@Override
	public void onClick(View v)
	{
		if(v == mChooseButton)
		{
			Intent intent = new Intent(getApplicationContext(), OpenActivity.class);
			if(mWolfdir != null && mWolfdir.length() > 0)
				intent.putExtra(OpenActivity.EXTRA_CURRENT_PATH, mWolfdir);
			startActivityForResult(intent, REQUEST_OPEN_FOLDER);
		}
		else if(v == mStartButton)
		{
			writeArguments();
			Intent intent = new Intent(getApplicationContext(), SDLActivity.class);
			intent.putStringArrayListExtra(SDLActivity.EXTRA_ARGS, mArgs);
			startActivity(intent);
		}
	}
	
	@Override
    protected void onActivityResult(int requestCode, int resultCode, 
    		Intent data)
    {
    	super.onActivityResult(requestCode, resultCode, data);
    	if(requestCode == REQUEST_OPEN_FOLDER && resultCode == RESULT_OK)
    	{
    		mWolfdir = data.getStringExtra(OpenActivity.EXTRA_CURRENT_PATH);
    		updateUi();
    		putData();
    	}
    }

	@Override
	public void afterTextChanged(Editable arg0) 
	{
		// TODO Auto-generated method stub
		
	}

	@Override
	public void beforeTextChanged(CharSequence arg0, int arg1, int arg2,
			int arg3) 
	{
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onTextChanged(CharSequence arg0, int arg1, int arg2, int arg3) 
	{
		try
		{
			mTedlevel = Integer.valueOf(mWarpField.getText().toString());
		}
		catch(NumberFormatException e)
		{
			mTedlevel = 0;
		}
		updateUi(true);
		putData();
	}

	@Override
	public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) 
	{
		if(buttonView == mSecretStep3Box)
		{
			mSecretStep3 = mSecretStep3Box.isChecked();
			updateUi();
			putData();
			return;
		}
		if(isChecked)
		{
			if(buttonView == mBabyButton)
				mSkillLevel = 0;
			else if(buttonView == mEasyButton)
				mSkillLevel = 1;
			else if(buttonView == mMediumButton)
				mSkillLevel = 2;
			else if(buttonView == mHardButton)
				mSkillLevel = 3;
			updateUi();
			putData();
		}		
	}
	
	private void writeArguments()
	{
		if(mArgs == null)
			mArgs = new ArrayList<String>();
		else
			mArgs.clear();
		
		if(mWolfdir != null)
		{
			mArgs.add("--wolfdir");
			mArgs.add(mWolfdir);
		}
		mArgs.add("--tedlevel");
		mArgs.add(String.valueOf(mTedlevel));
		
		switch(mSkillLevel)
		{
		case 0:
			mArgs.add("--baby");
			break;
		case 1:
			mArgs.add("--easy");
			break;
		case 2:
			mArgs.add("--medium");
			break;
		case 3:
			mArgs.add("--hard");
			break;
		}
		
		if(mSecretStep3)
			mArgs.add("--secretstep3");
		
		// Disable joystick (it's the silly accelerometer)
		mArgs.add("--joystick");
		mArgs.add("-1");
		
		if(isExternalStorageReadable() && isExternalStorageWritable())
		{
			File homeDir = Environment.getExternalStorageDirectory();
			File settingsDir = new File(homeDir, "com.ichera.autowolf");
			settingsDir.mkdirs();
			if(settingsDir.isDirectory())
			{
				mArgs.add("--configdir");
				mArgs.add(settingsDir.getAbsolutePath());
				//Log.i("What", settingsDir.getAbsolutePath());
			}
		}
	}
	
	/* Checks if external storage is available for read and write */
	private static boolean isExternalStorageWritable() {
	    String state = Environment.getExternalStorageState();
	    if (Environment.MEDIA_MOUNTED.equals(state)) {
	        return true;
	    }
	    return false;
	}

	/* Checks if external storage is available to at least read */
	private static boolean isExternalStorageReadable() {
	    String state = Environment.getExternalStorageState();
	    if (Environment.MEDIA_MOUNTED.equals(state) ||
	        Environment.MEDIA_MOUNTED_READ_ONLY.equals(state)) {
	        return true;
	    }
	    return false;
	}
}
