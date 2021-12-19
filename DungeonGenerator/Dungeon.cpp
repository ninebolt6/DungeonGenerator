#include "Dungeon.h"
#include <iostream>
#include <random>

Dungeon::Dungeon(const int width, const int height) : Width(width), Height(height)
{
	Rooms.reserve(Height);
}

/*
 * �_���W�����𐶐����܂�
 */
void Dungeon::GenerateDungeon()
{
	// ����������vector�̏�����
	Rooms.clear();
	for (int i = 0; i < Height; i++)
	{
		std::vector<Room> line;
		line.reserve(Width);

		for(int j = 0; j < Width; j++)
		{
			line.emplace_back(Room{ j, i, RoomType::Normal });
		}
		Rooms.emplace_back(line);
	}

	// �������Ȃ���
	ConnectRoomsByRandomDirection();

	// �X�^�[�g�����߂�
	std::set<RoomType> excludes { RoomType::Start, RoomType::Goal };

	Room* start = SelectRoomRandomly(excludes);
	start->SetRoomType(RoomType::Start);

	// �S�[�������߂�
	Room* goal = SelectRoomRandomly(excludes);
	while(true)
	{
		const Location diff = start->GetLocation() - goal->GetLocation();

		// �S�[�����}���n�b�^��������2�ȏ㗣�ꂽ�ꏊ�Ɍ��߂�
		if ((std::abs(diff.X) + std::abs(diff.Y)) >= 2)
		{
			goal->SetRoomType(RoomType::Goal);
			break;
		}

		// �X�^�[�g�n�_���I�΂�Ă��܂����ꍇ�͑I�ђ���
		goal = SelectRoomRandomly(excludes);
	}

	// �X�^�[�g�ƃS�[�����q�����Ă��邩���ׂ�
	Seen.clear();
	const bool isConnected = CanReachAt(*start, *goal);
	if(!isConnected)
	{
		// �q�����Ă��Ȃ��ꍇ�̓_���W�������Đ�������
		GenerateDungeon();
	}
}

/*
 * ���������_���W������W���o�͂ɏo�͂��܂�
 */
void Dungeon::Print()
{
	std::cout << Width << "x" << Height << "�̃_���W��������" << std::endl;

	// �����p�̓��L���b�V��������������
	Roads.reserve(Height);
	const int roadWidth = (Width * 2) - 1;
	for (int i = 0; i < Height; i++)
	{
		const std::vector<bool> road(roadWidth, false);
		Roads.emplace_back(road);
	}
	
	/*
	 * ���L���b�V���ɓ���ǉ�����
	 * ex) ����5�̏ꍇ�Avector<bool>[i] (i=0,1,...,8)�̓��e��
	 * �O�@�Q�@�S�@�U�@�W
	 * ���P���R���T���V��
	 */
	for (int line = 0; line < Height; line++)
	{
		for (int i = 0; i < Width; i++)
		{
			if (Rooms.at(line).at(i).IsConnected(Direction::Up))
			{
				Roads.at(line).at(i * 2) = true;
			}

			if (Rooms.at(line).at(i).IsConnected(Direction::Down))
			{
				Roads.at(line + 1).at(i * 2) = true;
			}

			if (Rooms.at(line).at(i).IsConnected(Direction::Left))
			{
				Roads.at(line).at((i * 2) - 1) = true;
			}

			if (Rooms.at(line).at(i).IsConnected(Direction::Right))
			{
				Roads.at(line).at((i * 2) + 1) = true;
			}
		}
	}

	// �_���W��������������
	for (int i = 0; i < Height; i++)
	{
		for (int j = 0; j < Width; j++)
		{
			std::cout << (Roads.at(i).at(j * 2) ? "�b" : "�@") << "�@";
		}
		std::cout << std::endl;
		for (int j = 0; j < Width; j++)
		{
			std::cout << RoomTypeToStr(Rooms.at(i).at(j).GetRoomType());
			const int pos = (j * 2) + 1;
			if (pos < roadWidth)
			{
				std::cout << (Roads.at(i).at(pos) ? "�[" : "�@");
			}
		}
		std::cout << std::endl;
	}
}

/*
 * �[���D��T���ɂ�蓞�B�\�����`�F�b�N���܂�
 * @return from����to�܂œ��B�\�Ȉړ����@������ꍇtrue, ����ȊO��false
 */
bool Dungeon::CanReachAt(const Room& from, const Room& to)
{
	// from��to�̍��W����v������A���B�\
	if(from.GetLocation() == to.GetLocation())
	{
		return true;
	}

	bool result = false;
	for(const Room* neighbor : from.GetNeighbors())
	{
		if (std::find(Seen.begin(), Seen.end(), neighbor->GetLocation()) != Seen.end())
		{
			continue;
		}

		// ���Ɍ����n�_�̓L���b�V�����āA2�x���Ȃ��悤��
		Seen.emplace_back(neighbor->GetLocation());

		// neighbor�ɗאڂ��������ɑ΂���
		result = CanReachAt(*neighbor, to);
		if(result == true)
		{
			break;
		}
	}
	return result;
}

/*
 * �����_����2������I�����A�������q���܂��B
 */
void Dungeon::ConnectRoomsByRandomDirection()
{
	std::random_device rd;
	std::mt19937 eng(rd());
	const std::uniform_int_distribution<int> dist(0, 3);

	for(int i = 0; i < Rooms.size(); i++)
	{
		for(int j = 0; j < Rooms.at(i).size(); j++)
		{
			const int firstDir = dist(eng);
			auto dir = static_cast<Direction>(firstDir);
			Room* room = &Rooms.at(i).at(j);
			Room* neighbor = GetNextRoom(i, j, dir);

			if(neighbor == nullptr)
			{
				dir = Room::ReverseDirection(dir);
				neighbor = GetNextRoom(i, j, dir);
			}
			room->SetNeighbor(dir, neighbor);
			neighbor->SetNeighbor(Room::ReverseDirection(dir), room);

			int secondDir = dist(eng);
			while (firstDir == secondDir)
			{
				secondDir = dist(eng);
			}

			auto dir2 = static_cast<Direction>(secondDir);
			neighbor = GetNextRoom(i, j, dir2);
			if (neighbor == nullptr)
			{
				dir2 = Room::ReverseDirection(dir2);
				neighbor = GetNextRoom(i, j, dir2);
			}
			room->SetNeighbor(dir2, neighbor);
			neighbor->SetNeighbor(Room::ReverseDirection(dir2), room);
		}
	}
}

/*
 * �w�肵�������ɗׂ荇���Ă��镔�����擾���܂�
 * @param line TODO �ύX�\��
 * @param row TODO �ύX�\��
 * @param direction ����
 * @return �ׂ荇���Ă��镔���B������Ȃ��ꍇ��nullptr
 */
Room* Dungeon::GetNextRoom(const int line, const int row, const Direction direction)
{
	switch (direction)
	{
	case Direction::Up:
		if (line - 1 >= 0)
		{
			return &Rooms.at(line - 1).at(row);
		}
		break;
	case Direction::Down:
		if (line + 1 <= Rooms.size() - 1)
		{
			return &Rooms.at(line + 1).at(row);
		}
		break;
	case Direction::Left:
		if (row - 1 >= 0)
		{
			return &Rooms.at(line).at(row - 1);
		}
		break;
	case Direction::Right:
		if (row + 1 <= Rooms.at(line).size() - 1)
		{
			return &Rooms.at(line).at(row + 1);
		}
		break;
	}
	return nullptr;
}

/*
 * RoomType���r�W���A���C�Y�p�̋L���ɕϊ����܂��B
 * @param type �ϊ�����RoomType
 * @return �L�����i�[���ꂽstd::string
 */
std::string Dungeon::RoomTypeToStr(const RoomType type)
{
	switch (type)
	{
	case RoomType::Normal:
		return "��";
	case RoomType::Goal:
		return "��";
	case RoomType::Start:
		return "�~";
	default:
		// unreachable
		return "UN";
	}
}

/*
 * �����������_���ɑI�����܂��B
 * @param excludes �T�����珜�O����RoomType���i�[����std::set<RoomType>
 * @return �I�΂ꂽ�����̃|�C���^
 */
Room* Dungeon::SelectRoomRandomly(std::set<RoomType>& excludes)
{
	// �����������_���Ɏ擾���邽�߂̕ϐ��̐錾
	std::random_device rd;
	std::mt19937 eng(rd());
	const std::uniform_int_distribution<int> randLine(0, Rooms.size() - 1);
	const std::uniform_int_distribution<int> randRow(0, Rooms.at(0).size() - 1);

	Room* room = nullptr;
	// �������m�肷��܂Ń��[�v
	while(true)
	{
		// �����_����1�����I��
		room = &Rooms.at(randLine(eng)).at(randRow(eng));

		if(excludes.find(room->GetRoomType()) == excludes.end())
		{
			// excludes�ŏ��O�w�肵��RoomType�Ɉ�v���Ȃ�������m�肷��
			break;
		}
	}

	return room;
}