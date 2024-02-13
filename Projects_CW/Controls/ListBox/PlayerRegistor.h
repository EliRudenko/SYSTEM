#pragma once

#include "Player.h"
#include <vector>
using namespace std;

class PlayerRegister
{
public:
	vector<Player> players;

	PlayerRegister()
	{
        players = {
            {TEXT("Haas, Jonathan"), TEXT("Midfield"), 18, 4 },
            {TEXT("Pai, Jyothi"), TEXT("Forward"), 36, 12 },
            {TEXT("Hanif, Kerim"), TEXT("Back"), 26, 0 },
            {TEXT("Anderberg, Michael"), TEXT("Back"), 24, 2 },
            {TEXT("Jelitto, Jacek"), TEXT("Midfield"), 26, 3 },
            {TEXT("Raposo, Rui"), TEXT("Back"), 24, 3},
            {TEXT("Joseph, Brad"), TEXT("Forward"), 13, 3 },
            {TEXT("Bouchard, Thomas"), TEXT("Forward"), 28, 5 },
            {TEXT("Salmre, Ivo "), TEXT("Midfield"), 27, 7 },
            {TEXT("Camp, David"), TEXT("Midfield"), 22, 3 },
            {TEXT("Kohl, Franz"), TEXT("Goalkeeper"), 17, 0 }
        };
	}


};

