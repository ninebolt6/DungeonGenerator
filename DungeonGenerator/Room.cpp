#include "Room.h"

Room::Room(const int x, const int y, const RoomType type) : Type(type), Location({ x, y }) {}

RoomType Room::GetRoomType() const
{
	return Type;
}

void Room::SetRoomType(const RoomType type)
{
	Type = type;
}

Room* Room::GetNeighbor(const Direction direction) const
{
	return Neighbor.at(direction);
}

const std::set<Room*>& Room::GetNeighbors() const
{
	return Neighbors;
}

void Room::SetNeighbor(const Direction direction, Room* room)
{
	Neighbor[direction] = room;
	Neighbors.emplace(room);
}

bool Room::IsConnected(const Direction direction)
{
	return Neighbor.find(direction) != Neighbor.end();
}

Direction Room::ReverseDirection(const Direction direction)
{
	switch (direction)
	{
	case Direction::Up:
		return Direction::Down;
	case Direction::Down:
		return Direction::Up;
	case Direction::Left:
		return Direction::Right;
	case Direction::Right:
		return Direction::Left;
	}
	// unreachable
	return Direction::Up;
}

const Location& Room::GetLocation() const
{
	return Location;
}

void Room::SetLocation(const int x, const int y)
{
	Location.X = x;
	Location.Y = y;
}

