#include <iostream>

#include "Dungeon.h"

int main()
{
	Dungeon dungeon(5, 5);
	dungeon.GenerateDungeon();
	dungeon.Print();
}