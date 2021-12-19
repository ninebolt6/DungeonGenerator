#pragma once
#include <vector>
#include "Room.h"

class Dungeon
{
public:
	Dungeon(const int width = 5, const int height = 5);

	void GenerateDungeon();
	void Print();

private:
	int Width;
	int Height;
	std::vector<std::vector<Room>> Rooms;
	std::vector<std::vector<bool>> Roads;
	std::vector<Location> Seen;

	void ConnectRoomsByRandomDirection();
	bool CanReachAt(const Room& from, const Room& to);
	Room* GetNextRoom(const int line, const int row, const Direction direction);
	Room* SelectRoomRandomly(std::set<RoomType>& excludes);
	static std::string RoomTypeToStr(const RoomType type);
};