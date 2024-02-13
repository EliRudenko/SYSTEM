#pragma once

#include <Windows.h>

class Player
{
public:
    TCHAR achName[MAX_PATH];
    TCHAR achPosition[12];
    int nGamesPlayed;
    int nGoalsScored;
};

