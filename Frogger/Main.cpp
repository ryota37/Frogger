# include <Siv3D.hpp>
# include <cstdlib>
# include <ctime>

void DrawGrid(const Grid<int32>& grid)
{
	for (int32 y = 0; y < grid.height(); ++y)
	{
		for (int32 x = 0; x < grid.width(); ++x)
		{
			const RectF rect{ (x * 100), (y * 100), 100 };
			const ColorF color{ (3 - grid[y][x]) / 3.0 };
			rect.stretched(-1).draw(color);
		}
	}
}

void DrawCircle(const Circle& circle)
{
	circle.draw(Palette::Aqua);
}

int random_number(int n)
{
	std::srand(std::time(0));
	int random_number = std::rand() % n;
	return random_number;
}

std::vector<int> random_pair_generator()
{
	int n, m;
	do
	{
		n = random_number(7);
		m = random_number(6);
	} while (!((n % 2 == 0) ^ (m % 2 == 0)));
	return std::vector<int> {n, m};
}

enum class Direction { Up, Down, Left, Right };

namespace Coordinate
{
	constexpr int TOP_GRID_X = 50;
	constexpr int TOP_GRID_Y = 50;
	constexpr int BOTTUM_GRID_X = 750;
	constexpr int BOTTUM_GRID_Y = 550;

	std::vector<int> pair = random_pair_generator();
	int GOAL_X = 50 + 100 * pair[0];
	int GOAL_Y = 50 + 100 * pair[1];
}

bool isWallUp(Circle& circle, Grid<int32>& grid, int grid_x, int grid_y)
{
	if (circle.y == Coordinate::TOP_GRID_Y) return true;
	if (grid[std::max(grid_y - 1, 0)][grid_x] > 0) return true;
	return false;
}

bool isWallLeft(Circle& circle, Grid<int32>& grid, int grid_x, int grid_y)
{
	if (circle.x == Coordinate::TOP_GRID_X) return true;
	if (grid[grid_y][std::max(grid_x - 1, 0)] > 0) return true;
	return false;
}

bool isWallDown(Circle& circle, Grid<int32>& grid, int grid_x, int grid_y)
{
	if (circle.y == Coordinate::BOTTUM_GRID_Y) return true;
	if (grid[std::min(grid_y + 1, 5)][grid_x] > 0) return true;
	return false;
}

bool isWallRight(Circle& circle, Grid<int32>& grid, int grid_x, int grid_y)
{
	if (circle.x == Coordinate::BOTTUM_GRID_X) return true;
	if (grid[grid_y][std::min(grid_x + 1, 7)] > 0) return true;
	return false;
}

bool isWall(Circle& circle, Grid<int32>& grid, Direction dir)
{
	int grid_x = static_cast<int>(circle.x) / 100;
	int grid_y = static_cast<int>(circle.y) / 100;

	switch (dir)
	{
	case Direction::Up: return isWallUp(circle, grid, grid_x, grid_y);
	case Direction::Left: return isWallLeft(circle, grid, grid_x, grid_y);
	case Direction::Down: return isWallDown(circle, grid, grid_x, grid_y);
	case Direction::Right: return isWallRight(circle, grid, grid_x, grid_y);
	}
	return false;
}

void UpdateCircle(Circle& circle, Grid<int32>& grid)
{
	if (KeyUp.down() && !isWall(circle, grid, Direction::Up)) circle.y -= 100;
	if (KeyLeft.down() && !isWall(circle, grid, Direction::Left)) circle.x -= 100;
	if (KeyDown.down() && !isWall(circle, grid, Direction::Down)) circle.y += 100;
	if (KeyRight.down() && !isWall(circle, grid, Direction::Right)) circle.x += 100;
}

bool isAllGridPassed(const Grid<int32>& grid)
{
	for (int32 y = 0; y < grid.height(); ++y)
	{
		for (int32 x = 0; x < grid.width(); ++x)
		{
			if (grid[y][x] == 0) return false;
		}
	}
	return true;
}

void PrintResult(const Circle& circle, const Grid<int32>& grid, const Font& font)
{
	if (circle.x != Coordinate::GOAL_X) return;
	if (circle.y != Coordinate::GOAL_Y) return;
	if (isAllGridPassed(grid)) { font(U"Success!!!").drawAt(Vec2{ 400, 300 }, Palette::Black); }
	else { font(U"Failure!!!").drawAt(Vec2{ 400, 300 }, Palette::Black); }
}

void DrawGoalGrid()
{
	const RectF rect{ Coordinate::GOAL_X - 50, Coordinate::GOAL_Y - 50, 100 };
	rect.draw(Palette::Olive);
}

bool isDirectionKeyPressed()
{
	if (KeyLeft.down()) return true;
	if (KeyUp.down()) return true;
	if (KeyRight.down()) return true;
	if (KeyDown.down()) return true;
	return false;
}

void VisualizeCircleRoute(const Circle& circle, Grid<int32>& grid)
{
	int grid_x = static_cast<int>(circle.x) / 100;
	int grid_y = static_cast<int>(circle.y) / 100;

	for (int32 y = 0; y < grid.height(); ++y)
	{
		for (int32 x = 0; x < grid.width(); ++x)
		{
			const RectF rect{ (x * 100), (y * 100), 100 };
			if (x == grid_x && y == grid_y && grid[y][x] == 0)
			{
				// 円が格子を通った時に要素を0→1にする
				++grid[y][x] += 1;
			}
		}
	}
}

void Main()
{
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });

	// 8x6 の二次元配列を作成し、全ての要素を 0 で初期化する
	Grid<int32> grid(8, 6);
	Circle circle{ Coordinate::TOP_GRID_X,Coordinate::TOP_GRID_Y,25 };
	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };

	while (System::Update())
	{
		DrawGrid(grid);

		UpdateCircle(circle, grid);

		DrawCircle(circle);

		DrawGoalGrid();

		VisualizeCircleRoute(circle, grid);

		PrintResult(circle, grid, font);
	}
}
