#pragma once

#include <iostream>
#include <set>
#include <unordered_map>

enum class Direction
{
	Up = 0,
	Down,
	Left,
	Right
};

enum class RoomType
{
	Normal = 0,
	Start,
	Goal,
};

struct Location
{
	int X;
	int Y;

	Location(const int x, const int y) : X(x), Y(y) {}
	Location operator -(const Location& rhs) const
	{
		return Location{ X - rhs.X, Y - rhs.Y };
	}

	bool operator ==(const Location& rhs) const
	{
		return (X == rhs.X) && (Y == rhs.Y);
	}
};

class Room
{
public:
	Room(const int x, const int y, const RoomType type = RoomType::Normal);

	RoomType GetRoomType() const;
	void SetRoomType(RoomType type);
	Room* GetNeighbor(Direction direction) const;
	const std::set<Room*>& GetNeighbors() const;
	void SetNeighbor(Direction direction, Room* room);
	bool IsConnected(const Direction direction);
	static Direction ReverseDirection(const Direction direction);
	const Location& GetLocation() const;
	void SetLocation(const int x, const int y);

private:
	RoomType Type;
	std::unordered_map<Direction, Room*> Neighbor;
	std::set<Room*> Neighbors;
	Location Location;
};