#include "Dungeon.h"
#include <iostream>
#include <random>

Dungeon::Dungeon(const int width, const int height) : Width(width), Height(height)
{
	Rooms.reserve(Height);
}

/*
 * ダンジョンを生成します
 */
void Dungeon::GenerateDungeon()
{
	// 部屋を入れるvectorの初期化
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

	// 部屋をつなげる
	ConnectRoomsByRandomDirection();

	// スタートを決める
	std::set<RoomType> excludes { RoomType::Start, RoomType::Goal };

	Room* start = SelectRoomRandomly(excludes);
	start->SetRoomType(RoomType::Start);

	// ゴールを決める
	Room* goal = SelectRoomRandomly(excludes);
	while(true)
	{
		const Location diff = start->GetLocation() - goal->GetLocation();

		// ゴールをマンハッタン距離で2以上離れた場所に決める
		if ((std::abs(diff.X) + std::abs(diff.Y)) >= 2)
		{
			goal->SetRoomType(RoomType::Goal);
			break;
		}

		// スタート地点が選ばれてしまった場合は選び直す
		goal = SelectRoomRandomly(excludes);
	}

	// スタートとゴールが繋がっているか調べる
	Seen.clear();
	const bool isConnected = CanReachAt(*start, *goal);
	if(!isConnected)
	{
		// 繋がっていない場合はダンジョンを再生成する
		GenerateDungeon();
	}
}

/*
 * 生成したダンジョンを標準出力に出力します
 */
void Dungeon::Print()
{
	std::cout << Width << "x" << Height << "のダンジョン生成" << std::endl;

	// 可視化用の道キャッシュを初期化する
	Roads.reserve(Height);
	const int roadWidth = (Width * 2) - 1;
	for (int i = 0; i < Height; i++)
	{
		const std::vector<bool> road(roadWidth, false);
		Roads.emplace_back(road);
	}
	
	/*
	 * 道キャッシュに道を追加する
	 * ex) 幅が5の場合、vector<bool>[i] (i=0,1,...,8)の内容は
	 * ０　２　４　６　８
	 * ■１■３■５■７■
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

	// ダンジョンを可視化する
	for (int i = 0; i < Height; i++)
	{
		for (int j = 0; j < Width; j++)
		{
			std::cout << (Roads.at(i).at(j * 2) ? "｜" : "　") << "　";
		}
		std::cout << std::endl;
		for (int j = 0; j < Width; j++)
		{
			std::cout << RoomTypeToStr(Rooms.at(i).at(j).GetRoomType());
			const int pos = (j * 2) + 1;
			if (pos < roadWidth)
			{
				std::cout << (Roads.at(i).at(pos) ? "ー" : "　");
			}
		}
		std::cout << std::endl;
	}
}

/*
 * 深さ優先探索により到達可能性をチェックします
 * @return fromからtoまで到達可能な移動方法がある場合true, それ以外はfalse
 */
bool Dungeon::CanReachAt(const Room& from, const Room& to)
{
	// fromとtoの座標が一致したら、到達可能
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

		// 既に見た地点はキャッシュして、2度見ないように
		Seen.emplace_back(neighbor->GetLocation());

		// neighborに隣接した部屋に対して
		result = CanReachAt(*neighbor, to);
		if(result == true)
		{
			break;
		}
	}
	return result;
}

/*
 * ランダムな2方向を選択し、部屋を繋げます。
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
 * 指定した方向に隣り合っている部屋を取得します
 * @param line TODO 変更予定
 * @param row TODO 変更予定
 * @param direction 方向
 * @return 隣り合っている部屋。見つからない場合はnullptr
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
 * RoomTypeをビジュアライズ用の記号に変換します。
 * @param type 変換するRoomType
 * @return 記号が格納されたstd::string
 */
std::string Dungeon::RoomTypeToStr(const RoomType type)
{
	switch (type)
	{
	case RoomType::Normal:
		return "■";
	case RoomType::Goal:
		return "□";
	case RoomType::Start:
		return "×";
	default:
		// unreachable
		return "UN";
	}
}

/*
 * 部屋をランダムに選択します。
 * @param excludes 探索から除外するRoomTypeを格納したstd::set<RoomType>
 * @return 選ばれた部屋のポインタ
 */
Room* Dungeon::SelectRoomRandomly(std::set<RoomType>& excludes)
{
	// 整数をランダムに取得するための変数の宣言
	std::random_device rd;
	std::mt19937 eng(rd());
	const std::uniform_int_distribution<int> randLine(0, Rooms.size() - 1);
	const std::uniform_int_distribution<int> randRow(0, Rooms.at(0).size() - 1);

	Room* room = nullptr;
	// 部屋が確定するまでループ
	while(true)
	{
		// ランダムに1部屋選ぶ
		room = &Rooms.at(randLine(eng)).at(randRow(eng));

		if(excludes.find(room->GetRoomType()) == excludes.end())
		{
			// excludesで除外指定したRoomTypeに一致しなかったら確定する
			break;
		}
	}

	return room;
}