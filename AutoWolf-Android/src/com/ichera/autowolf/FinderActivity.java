package com.ichera.autowolf;

import java.io.File;
import java.util.ArrayList;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;

/*
 * Workflow:
 * 
 * - If no paths are stored locally, start a search
 * - If any paths stored locally are invalid, start a search
 * - Otherwise, just display the paths
 */

public class FinderActivity extends Activity 
{
	public static final String PATH = "path";
	
	private static final String PREFS_NAME = "FinderActivityPrefs";
	private static final String PREF_PATHS = "Paths";
	
	private View mWaitingPanel, mResultsPanel;
	private TextView mResultsTitle, mResultsSubtitle, mProgressText;
	
	private ListView mResultsList;
	
	private String[] mPaths;
	
	private AsyncTask<Void, String, ArrayList<File>> mAsyncTask;
	
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_finder);
		
		mWaitingPanel = findViewById(R.id.waiting_panel);
		mResultsPanel = findViewById(R.id.results_panel);
		
		mResultsTitle = (TextView)findViewById(R.id.results_panel_title);
		mResultsSubtitle = (TextView)findViewById(R.id.results_panel_subtitle);
		mProgressText = (TextView)findViewById(R.id.progress_text);
		
		mResultsList = (ListView)findViewById(R.id.results_list);
		mResultsList.setOnItemClickListener(new AbsListView.OnItemClickListener() 
		{
			@Override
			public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
					long arg3) 
			{
				String path = mPaths[arg2];
				Intent data = new Intent();
				data.putExtra(PATH, path);
				setResult(RESULT_OK, data);
				finish();
			}
		});
		
		findViewById(R.id.search_again).setOnClickListener(new View.OnClickListener() 
		{
			
			@Override
			public void onClick(View v) 
			{				
				mPaths = null;
				startSearch();
			}
		});
		
		SharedPreferences settings = getSharedPreferences(PREFS_NAME, 0);
		String[] paths = settings.getString(PREF_PATHS, "").split("\\n");
		if(paths.length == 1 && paths[0].length() == 0)
			paths = null;	// nope
		
		if(paths == null)
		{
			mPaths = null;
			startSearch();
		}
		else
		{
			ArrayList<File> fs = new ArrayList<File>(paths.length);
			for(String p : paths)
			{
				if(p != null && p.trim().length() > 0)
				{
					fs.add(new File(p));
				}
			}
			showResultsPanel(fs);
		}
	}
	
	private void showWaitingPanel()
	{
		mResultsPanel.setVisibility(View.GONE);
		mWaitingPanel.setVisibility(View.VISIBLE);
	}
	
	private void showSorryPanel()
	{
		mWaitingPanel.setVisibility(View.GONE);
		mResultsPanel.setVisibility(View.VISIBLE);
		
		mResultsTitle.setText(R.string.no_found_results);
		mResultsSubtitle.setText(R.string.no_found_explanation);
	}
	
	private void showResultsPanel(ArrayList<File> results)
	{
		mPaths = new String[results.size()];
		int i = 0;
		for(File f : results)
		{
			mPaths[i++] = f.getPath();
		}
		
		showResultsPanel();
	}
	
	private void showResultsPanel()
	{
		mWaitingPanel.setVisibility(View.GONE);
		mResultsPanel.setVisibility(View.VISIBLE);
		
		mResultsTitle.setText(R.string.found_results);
		mResultsSubtitle.setText(R.string.pick_one);
		
		updateList();
	}
	
	private void updateList()
	{
		
		
		if(mResultsList.getAdapter() == null)
			mResultsList.setAdapter(new PathsAdapter());
		else
			((PathsAdapter)mResultsList.getAdapter()).notifyDataSetChanged();
	}
	
	private void startSearch()
	{
		showWaitingPanel();
		
		(mAsyncTask = new AsyncTask<Void, String, ArrayList<File>>() 
		{	
			private int iteration = 0;
			
			@Override
			protected ArrayList<File> doInBackground(Void... params) 
			{
				File base = Environment.getExternalStorageDirectory();
				if(base == null || !base.isDirectory())
					return null;
				
				ArrayList<File> ret = new ArrayList<File>(10);
				workSearch(base, ret);
				return ret;
			}
			
			private void workSearch(File dir, ArrayList<File> validDirs)
			{
				File[] fs = dir.listFiles();
				if(fs == null)
					return;
				
				if(iteration++ % 64 == 0)
					publishProgress(dir.getPath());
				
				int eqcount = 0;
				for(File g : fs)
				{
					if(isCancelled())
						return;
					if(fs.length >= 8 && Util.fileIsOfWolf(g.getName()))
						++eqcount;
					if(g.isDirectory())
						workSearch(g, validDirs);
				}
				if(eqcount >= 8)
					validDirs.add(dir);
			}
			
			@Override
			protected void onCancelled()
			{
				mProgressText.setVisibility(View.GONE);
			}
			
			@Override
			protected void onProgressUpdate(String... paths)
			{
				mProgressText.setVisibility(View.VISIBLE);
				mProgressText.setText(paths[0] != null ? paths[0] : "");
			}
			
			@Override
			protected void onPostExecute(ArrayList<File> result)
			{
				mProgressText.setVisibility(View.GONE);
				mAsyncTask = null;
				SharedPreferences settings = getSharedPreferences(PREFS_NAME, 0);
				SharedPreferences.Editor editor = settings.edit();
				
				if(result == null || result.size() == 0)
				{
					showSorryPanel();
					editor.remove(PREF_PATHS);
					editor.commit();
					return;
				}
				
				StringBuilder sb = new StringBuilder(100);
				for(File f : result)
				{
					sb.append(f.getPath());
					sb.append('\n');
				}
				
				editor.putString(PREF_PATHS, sb.toString());
				
				editor.commit();
				
				showResultsPanel(result);
			}
		}).execute();
	}
	
	@Override
	protected void onDestroy()
	{
		super.onDestroy();
		if(mAsyncTask != null && !mAsyncTask.isCancelled())
			mAsyncTask.cancel(true);
	}
	
	
	
	private class PathsAdapter extends BaseAdapter
	{

		@Override
		public int getCount() 
		{
			return mPaths != null ? mPaths.length : 0;
		}

		@Override
		public Object getItem(int arg0) 
		{
			return mPaths != null ? mPaths[arg0] : "";
		}

		@Override
		public long getItemId(int arg0)
		{
			return 0;
		}

		@Override
		public View getView(int pos, View cvView, ViewGroup vg) 
		{
			if(cvView == null)
			{
				final float scale = getResources().getDisplayMetrics().density;
				TextView tv = new TextView(FinderActivity.this);
				AbsListView.LayoutParams alvlp = new AbsListView.LayoutParams
						(ViewGroup.LayoutParams.MATCH_PARENT, (int)(scale * 48));
				tv.setLayoutParams(alvlp);
				tv.setTextSize(TypedValue.COMPLEX_UNIT_SP, 20);
				tv.setGravity(Gravity.CENTER_VERTICAL);
				cvView = tv;
			}
			((TextView)cvView).setText(new File((String)getItem(pos)).getName());
			return cvView;
		}
		
	}
}
