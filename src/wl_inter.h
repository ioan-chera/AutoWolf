/*
 =============================================================================
 
 WL_INTER
 
 =============================================================================
 */

#ifndef WL_INTER_H_
#define WL_INTER_H_

class LoadingScreen
{
public:
    ~LoadingScreen();
    void Update(double current, double total);
    void Show();

private:
    bool showing = false;
};

void DrawHighScores();
void CheckHighScore (int32_t score,word other);
void Victory ();
void LevelCompleted ();
void ClearSplitVWB ();

void PreloadGraphics();



#endif

