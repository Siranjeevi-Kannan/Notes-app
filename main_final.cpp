#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <Windows.h>

void ClearDelay(void) {
	for (int i = 0; i < 0xFF; i++) {
		GetAsyncKeyState(i);
	}
}
class ConsoleHandle {
protected:
	HANDLE stdconsolehwnd;	
	CONSOLE_SCREEN_BUFFER_INFO bufferinfo;
public:
	ConsoleHandle(void) {
		stdconsolehwnd = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(stdconsolehwnd, &bufferinfo);
	}
	~ConsoleHandle(void) {
		CloseHandle(stdconsolehwnd);
	}
};
class FileHandle {
private:
	static std::vector<std::string> lines;
	static void FileDump(std::string str) {
		std::ofstream file(str + ".txt");
		for (size_t i = 0; i < lines.size(); ++i) {
			file << lines[i] << std::endl;
		}
		file.close();
		lines.clear();
	}
	std::string filename;
public:
	FileHandle(const std::string& str) :filename(str) {}
	void LineDump(void) {
		std::ifstream file(filename + ".txt");
		std::string line;
		while (std::getline(file, line)) {
			lines.push_back(line);
		}
		file.close();
	}
	FileHandle operator++(int) {
		LineDump();
		FileDump(filename + " copy");
		return FileHandle(filename + " copy");
	}
	FileHandle operator+(FileHandle newfile) {
		LineDump();
		newfile.LineDump();
		FileDump(filename + " modified");
		return FileHandle(filename + " modified");
	}
	friend class Menu;
};
std::vector<std::string> FileHandle::lines;
class IOHandler;
void ConsoleFileText(IOHandler& iohwnd, int key);
void ConsoleFileText(IOHandler& iohwnd, int key, int cap);
class IOHandler : public ConsoleHandle {
protected:
	int cap = 0;
	std::string filename;
	std::vector<std::string> secondary_lines;
	std::string primary_line;
	unsigned int line_index = 0;
public:
	void read_routine(std::string file_name) {
		filename = file_name;
		std::ifstream file(filename + ".txt");
		std::string line;
		while (std::getline(file, line)) {
			secondary_lines.push_back(line);
		}
		file.close();
		for (size_t i = 0; i < secondary_lines.size(); ++i) {
			std::cout << secondary_lines[i] << std::endl;
		}
	}
	void open_routine(std::string file_name) {
		filename = file_name;
	}
	std::string edit_routine(int c) {
		ClearDelay();
		line_index = 0;
		primary_line.clear();
		short code = 0;
		int end = 1;
		while (end)
		{
			Sleep(100);
			GetConsoleScreenBufferInfo(stdconsolehwnd, &bufferinfo);
			code = (GetAsyncKeyState(VK_BACK) & 0b000000000000001);
			if (code) {
				if (bufferinfo.dwCursorPosition.X > 0 && line_index > 0) {
					line_index--;
					primary_line.erase(line_index, 1);
					bufferinfo.dwCursorPosition.X--;
					SetConsoleCursorPosition(stdconsolehwnd, bufferinfo.dwCursorPosition);
					std::cout << primary_line.substr(line_index) << ' ';
					SetConsoleCursorPosition(stdconsolehwnd, bufferinfo.dwCursorPosition);
				}
				continue;
			}
			code = (GetAsyncKeyState(VK_RETURN) & 0b000000000000001);
			if (code) {
				if (c == 0) {
					if (line_index == primary_line.length()) {
						secondary_lines.push_back(primary_line);
						primary_line.clear();
						line_index = 0;
						std::cout << std::endl;
					}
					else {
						secondary_lines.push_back(primary_line.substr(0, line_index));
						primary_line = primary_line.substr(line_index);
						SetConsoleCursorPosition(stdconsolehwnd, bufferinfo.dwCursorPosition);
						std::string str(primary_line.length(), ' ');
						std::cout << str;
						SetConsoleCursorPosition(stdconsolehwnd, bufferinfo.dwCursorPosition);
						line_index = 0;
						std::cout << std::endl;
						GetConsoleScreenBufferInfo(stdconsolehwnd, &bufferinfo);
						std::cout << primary_line;
						SetConsoleCursorPosition(stdconsolehwnd, bufferinfo.dwCursorPosition);
					}
				}
				else {
					line_index = 0;
					return primary_line;
				}
				continue;
			}
			code = (GetAsyncKeyState(VK_ESCAPE) & 0b000000000000001);
			if (code) {
				if (c == 0) {
					secondary_lines.push_back(primary_line);
					primary_line.clear();
					line_index = 0;
					system("cls");
					std::ofstream file(filename + ".txt");
					for (size_t i = 0; i < secondary_lines.size(); ++i) {
						file << secondary_lines[i] << std::endl;
					}
					file.close();
					secondary_lines.clear();
					filename.clear();
					end = 0;
				}
				else {
					line_index = 0;
					std::string str = "Invalid";
					return str;
				}
				continue;
			}
			code = (GetAsyncKeyState(VK_LEFT) & 0b000000000000001);
			if (code) {
				if (line_index > 0 && bufferinfo.dwCursorPosition.X > 0) {
					line_index--;
					bufferinfo.dwCursorPosition.X--;
					SetConsoleCursorPosition(stdconsolehwnd, bufferinfo.dwCursorPosition);
				}
				continue;
			}
			code = (GetAsyncKeyState(VK_RIGHT) & 0b000000000000001);
			if (code) {
				if (line_index < primary_line.length()) {
					if (bufferinfo.dwCursorPosition.X < bufferinfo.dwSize.X) {
						line_index++;
						bufferinfo.dwCursorPosition.X++;
						SetConsoleCursorPosition(stdconsolehwnd, bufferinfo.dwCursorPosition);
					}
					else {
						line_index++;
						bufferinfo.dwCursorPosition.X = 0;
						bufferinfo.dwCursorPosition.Y++;
						SetConsoleCursorPosition(stdconsolehwnd, bufferinfo.dwCursorPosition);
					}
				}
				continue;
			}
			code = (GetAsyncKeyState(VK_CAPITAL) & 0b000000000000001);
			if (code) {
				cap = (~cap);
				continue;
			}
			code = (GetAsyncKeyState(VK_SPACE) & 0b000000000000001);
			if (code) {
				ConsoleFileText(*this, VK_SPACE);
				continue;
			}
			code = (GetAsyncKeyState(VK_OEM_COMMA) & 0b000000000000001);
			if (code) {
				ConsoleFileText(*this, VK_OEM_COMMA, ',' - VK_OEM_COMMA);
				continue;
			}
			code = (GetAsyncKeyState(VK_OEM_PERIOD) & 0b000000000000001);
			if (code) {
				ConsoleFileText(*this, VK_OEM_PERIOD, '.' - VK_OEM_PERIOD);
				continue;
			}
			code = (GetAsyncKeyState(VK_OEM_2) & 0b000000000000001);
			if (code) {
				ConsoleFileText(*this, VK_OEM_2, '?' - VK_OEM_2);
				continue;
			}
			code = (GetAsyncKeyState(VK_OEM_7) & 0b000000000000001);
			if (code) {
				ConsoleFileText(*this, VK_OEM_7, '"' - VK_OEM_7);
				continue;
			}
			int z = 0;
			for (int i = '0'; i <= '9'; i++) {
				code = (GetAsyncKeyState(i) & 0b000000000000001);
				if (code) {
					ConsoleFileText(*this, i);
					z = 1;
					break;
				}
			}
			if (cap && z == 0) {
				for (int i = 'A'; i <= 'Z'; i++) {
					code = (GetAsyncKeyState(i) & 0b000000000000001);
					if (code) {
						ConsoleFileText(*this, i, 'a' - 'A');
						z = 1;
						break;
					}
				}
			}
			else {
				for (int i = 'A'; i <= 'Z'; i++) {
					code = (GetAsyncKeyState(i) & 0b000000000000001);
					if (code) {
						ConsoleFileText(*this, i);
						break;
					}
				}
			}
		}
		line_index = 0;
		std::string str = "Invalid";
		return str;
	}
	friend class Menu;
	friend void ConsoleFileText(IOHandler& iohwnd, int key);
	friend void ConsoleFileText(IOHandler& iohwnd, int key, int cap);
};
void ConsoleFileText(IOHandler& iohwnd, int key) {
	if (iohwnd.bufferinfo.dwCursorPosition.X < iohwnd.bufferinfo.dwSize.X) {
		std::string str(1, static_cast<char>(key));
		iohwnd.primary_line.insert(iohwnd.line_index, str);
		std::cout << iohwnd.primary_line.substr(iohwnd.line_index);
		iohwnd.bufferinfo.dwCursorPosition.X++;
		SetConsoleCursorPosition(iohwnd.stdconsolehwnd, iohwnd.bufferinfo.dwCursorPosition);
		iohwnd.line_index++;
	}
	else {
		std::string str(1, static_cast<char>(key));
		iohwnd.primary_line.insert(iohwnd.line_index, str);
		std::cout << iohwnd.primary_line.substr(iohwnd.line_index);
		iohwnd.bufferinfo.dwCursorPosition.X = 0;
		iohwnd.bufferinfo.dwCursorPosition.Y++;
		SetConsoleCursorPosition(iohwnd.stdconsolehwnd, iohwnd.bufferinfo.dwCursorPosition);
		iohwnd.line_index++;
	}
}
void ConsoleFileText(IOHandler& iohwnd, int key, int cap) {
	int shift = key + cap;
	if (iohwnd.bufferinfo.dwCursorPosition.X < iohwnd.bufferinfo.dwSize.X) {
		std::string str(1, static_cast<char>(shift));
		iohwnd.primary_line.insert(iohwnd.line_index, str);
		std::cout << iohwnd.primary_line.substr(iohwnd.line_index);
		iohwnd.bufferinfo.dwCursorPosition.X++;
		SetConsoleCursorPosition(iohwnd.stdconsolehwnd, iohwnd.bufferinfo.dwCursorPosition);
		iohwnd.line_index++;
	}
	else {
		std::string str(1, static_cast<char>(shift));
		iohwnd.primary_line.insert(iohwnd.line_index, str);
		std::cout << iohwnd.primary_line.substr(iohwnd.line_index);
		iohwnd.bufferinfo.dwCursorPosition.X = 0;
		iohwnd.bufferinfo.dwCursorPosition.Y++;
		SetConsoleCursorPosition(iohwnd.stdconsolehwnd, iohwnd.bufferinfo.dwCursorPosition);
		iohwnd.line_index++;
	}
}
class coord {
public:
	unsigned char x;
	unsigned char y;
	coord(void) {}
	coord(const unsigned char x0, const unsigned char y0) :x(x0), y(y0) {}
	inline void operator=(const coord c) {
		this->x = c.x;
		this->y = c.y;
	}
	inline void operator+(const coord c) {
		this->x += c.x;
		this->y += c.y;
	}
	inline void update(char* map, char pix) {
		map[(89 * y) + x] = pix;
	}
};
class Empty {
public:
	virtual ~Empty(void) {}
};
class GameObject : public Empty {
public:
	virtual void update(char* map) = 0;
	virtual ~GameObject(void) {}
};
class Ball;
class Player : public GameObject {
private:
	static coord start_pos;
	coord shift = coord(0, 0);
public:
	coord position = coord(0, 0);
	coord velocity = coord(0, 0);
	Player(char* map) {
		if (start_pos.x == 81) {
			shift = coord('W' - VK_UP, 'S' - VK_DOWN);
		}
		position = start_pos;
		start_pos = coord(81, 11);
		position.update(map, '|');
		coord(position.x, position.y + 1).update(map, '|');
	}
	void update(char* map) {
		int code = 0;
		velocity = coord(0, 0);
		code = (GetAsyncKeyState('W' - shift.x) & 0b000000000000001);
		if (code && position.y > 3) {
			velocity.y = -1;
			coord(position.x, position.y + 1).update(map, ' ');
			coord(position.x, position.y - 1).update(map, '|');
		}
		else {
			code = (GetAsyncKeyState('S' - shift.y) & 0b000000000000001);
			if (code && position.y < 19) {
				velocity.y = 1;
				coord(position.x, position.y).update(map, ' ');
				coord(position.x, position.y + 2).update(map, '|');
			}
		}
		position + velocity;
	}
};
coord Player::start_pos = coord(6, 11);
class Ball : public GameObject {
private:
	Player* p1 = nullptr;
	Player* p2 = nullptr;
public:
	coord position = coord(44, 11);
	coord velocity = coord(0, 0);
	Ball(void) {}
	Ball(Player* p10, Player* p20) : p1(p10), p2(p20) {}
	Ball(char* map, Player* p10, Player* p20) : p1(p10), p2(p20) {
		Rand(map);
	}
	void Rand(char* map) {
		std::mt19937 gen(std::time(0));
		std::uniform_int_distribution<int> dist(1, 6);
		int r = dist(gen);
		if (r == 1) {
			velocity = coord(1, 0);
		}
		else if (r == 2) {
			velocity = coord(1, -1);
		}
		else if (r == 3) {
			velocity = coord(-1, -1);
		}
		else if (r == 4) {
			velocity = coord(-1, 0);
		}
		else if (r == 5) {
			velocity = coord(-1, 1);
		}
		else if (r == 6) {
			velocity = coord(1, 1);
		}
		position.update(map, '*');
	}
	void update(char* map) {
		position.update(map, ' ');
		position + velocity;
		if (position.y == 2) {
			position + coord(velocity.x, 2);
			velocity.y *= (-1);
		}
		else if (position.y == 20) {
			position + coord(velocity.x, -2);
			velocity.y *= (-1);
		}
		if (position.x == 4 || position.x == 83) {
			velocity = coord(0, 0);
		}
		if (position.x == 6 && (std::abs(position.y - p1->position.y) < 2)) {
			if (p1->velocity.y) {
				velocity.y = p1->velocity.y;
			}
			position + coord(2, velocity.y);
			velocity.x *= (-1);
		}
		else if (position.x == 81 && (std::abs(position.y - p2->position.y) < 2)) {
			if (p2->velocity.y) {
				velocity.y = p2->velocity.y;
			}
			position + coord(-2, velocity.y);
			velocity.x *= (-1);
		}
		position.update(map, '*');
	}
};
class pixcoord {
public:
	coord c;
	char pix;
	pixcoord(unsigned char x, unsigned char y, char p) : c(coord(x, y)), pix(p) {}
	void update(char* map, pixcoord* p, coord start, int n) {
		for (int i = 0; i < n; i++) {
			coord c = start;
			c + p[i].c;
			c.update(map, p[i].pix);
		}
	}
};
class Explosion : public Ball {
private:
	Ball* ball;
	pixcoord t = pixcoord(0, 0, 0);
	pixcoord s0[1] = { pixcoord(0,0,'*') };
	pixcoord s1[2] = { pixcoord(0,0,'('),pixcoord(1,0,')') };
	pixcoord s2[4] = { pixcoord(0,0,'/'),pixcoord(1,0,'\\'),pixcoord(0,1,'\\'),pixcoord(1,1,'/') };
	pixcoord s3[6] = { pixcoord(0,0,'/'),pixcoord(1,-1,'_'),pixcoord(2,0,'\\'),pixcoord(0,1,'\\'),pixcoord(2,1,'/'),pixcoord(1,1,'_') };
	pixcoord s4[10] = { pixcoord(0,0,'/'),pixcoord(1,-1,'_'),pixcoord(2,-1,'_'),pixcoord(3,0,'\\'),pixcoord(-1,1,'|'),pixcoord(0,2,'\\'),pixcoord(1,2,'_'),pixcoord(2,2,'_'),pixcoord(3,2,'\\'),pixcoord(4,1,'|') };
	pixcoord s5[16] = { pixcoord(0,0,'/'),pixcoord(1,-1,'_'),pixcoord(2,-1,'_'),pixcoord(3,-1,'_'),pixcoord(4,-1,'_'),pixcoord(5,0,'\\'),pixcoord(-1,1,'|'),pixcoord(-1,2,'|'),pixcoord(6,1,'|'),pixcoord(6,2,'|'),pixcoord(0,3,'\\'),pixcoord(1,3,'_'),pixcoord(2,3,'_'),pixcoord(3,3,'_'),pixcoord(4,3,'_'),pixcoord(5,3,'/') };
public:
	int stage = 0;
	Explosion(void) {}
	Explosion(Player* p10, Player* p20, Ball* ball0) : Ball(p10, p20), ball(ball0) {}
	void update(char* map) {
		coord st = ball->position;
		if (stage == 0) {
			t.update(map, s0, st, 1);
		}
		else if (stage == 1) {
			t.update(map, s1, st, 2);
		}
		else if (stage == 2) {
			st + coord(-1, -1);
			t.update(map, s2, st, 4);
		}
		else if (stage == 3) {
			st + coord(-1, -1);
			t.update(map, s3, st, 6);
		}
		else if (stage == 4) {
			st + coord(-2, -2);
			t.update(map, s4, st, 10);
		}
		else if (stage == 5) {
			st + coord(-3, -3);
			t.update(map, s5, st, 16);
		}
		stage++;
	}
};
class GameState {
private:
	const char* strmap = "  __ ______________________________________________________________________________ __  \n /  \\                            \\_____0____:____0____/                            /  \\ \n |  |==============================================================================|  | \n |  |                                                                              |  | \n |  |                                                                              |  | \n |  |                                                                              |  | \n |  |                                                                              |  | \n |  |                                                                              |  | \n |  |                                                                              |  | \n/ \\ |                                                                              | / \\\n| | |                                                                              | | |\n| | |                                                                              | | |\n| | |                                                                              | | |\n| | |                                                                              | | |\n\\ / |                                                                              | \\ /\n |  |                                                                              |  | \n |  |                                                                              |  | \n |  |                                                                              |  | \n |  |                                                                              |  | \n |  |                                                                              |  | \n |  |                                                                              |  | \n |  |==============================================================================|  | \n \\__/______________________________________________________________________________\\__/ \n                                 \\____________________/                                  ";
	int shift = 0;
	Empty** base;
public:
	char* map = nullptr;
	GameState(void) {
		map = (char*)malloc((sizeof(char) * 89 * 24) + 1);
		strcpy(map, strmap);
	}
	void Setbase(Empty** b) {
		base = b;
	}
	int Game_routine(HANDLE stdconsolehwnd) {
		int ret = 0;
		int end = 1;
		int code = 0;
		ClearDelay();
		SetConsoleCursorPosition(stdconsolehwnd, { 0,4 });
		while (end) {
			Sleep(100 + shift);
			GameObject* gameptr = dynamic_cast<GameObject*>(base[0]);
			gameptr->update(map);
			gameptr = dynamic_cast<GameObject*>(base[1]);
			gameptr->update(map);
			gameptr = dynamic_cast<GameObject*>(base[2]);
			gameptr->update(map);
			printf("%s", map);
			SetConsoleCursorPosition(stdconsolehwnd, { 0,4 });
			Ball* ballptr = dynamic_cast<Ball*>(gameptr);
			if (ballptr->velocity.y != 0) {
				shift = 41;
			}
			if (ballptr->velocity.x == 0 && ballptr->velocity.y == 0) {
				gameptr = dynamic_cast<GameObject*>(base[3]);
				if (ballptr->position.x == 4) {
					ballptr->position.x = 5;
					ret = 2;
				}
				else {
					ballptr->position.x = 82;
					ret = 1;
				}
				if (ballptr->position.y < 4) {
					ballptr->position.y = 4;
				}
				else if (ballptr->position.y > 19) {
					ballptr->position.y = 19;
				}
				for (int i = 0; i < 6; i++) {
					Sleep(300);
					gameptr->update(map);
					printf("%s", map);
					SetConsoleCursorPosition(stdconsolehwnd, { 0,4 });
				}
				strcpy(map, strmap);
				gameptr = dynamic_cast<GameObject*>(base[0]);
				Player* pptr = dynamic_cast<Player*>(gameptr);
				pptr->position = coord(6, 11);
				pptr->velocity = coord(0, 0);
				coord(6, 11).update(map, '|');
				coord(6, 12).update(map, '|');
				gameptr = dynamic_cast<GameObject*>(base[1]);
				pptr = dynamic_cast<Player*>(gameptr);
				pptr->position = coord(81, 11);
				pptr->velocity = coord(0, 0);
				coord(81, 11).update(map, '|');
				coord(81, 12).update(map, '|');
				ballptr->position = coord(44, 11);
				ballptr->velocity = coord(0, 0);
				ballptr->Rand(map);
				gameptr = dynamic_cast<GameObject*>(base[3]);
				Explosion* exp = dynamic_cast<Explosion*>(gameptr);
				exp->stage = 0;
				end = 0;
			}
			code = (GetAsyncKeyState(VK_ESCAPE) & 0b000000000000001);
			if (code) {
				end = 0;
			}
		}
		return ret;
	}
	~GameState(void) {
		free(map);
	}
};
class Menu {
private:
	std::string menu_main = "\tWelcome to Notes app!\n\n\tMAIN MENU\n1. Open a text file.\n2. Delete a text file.\n3. Create a new text file.\n4. Create a copy of a text file\n5. Merge two text files\n6. Change background.\n7. Change text color.\n8. Play a game\n9. Exit.\n";
	std::string menu_colors = "1. Black\n2. Blue\n3. Green\n4. Aqua\n5. Red\n6. Purple\n7. Yellow\n8. White\n9. Gray\n10. Light Blue\n11. Light Green\n12. Light Aqua\n13. Light Red\n14. Light Purple\n15. Light Yellow\n16. Bright White\n";
	std::string menu_nav = "Use arrow keys to navigate and press enter to confirm your choice.";
	std::string menu_filename = "Enter file name: ";
	std::string menu_esc = "Press 'Esc' to save and exit.\n";
	std::string menu_back = "Press 'Enter' to go back to Main menu.";
	int current_page = 0;
public:
	void choice_routine(IOHandler& iohwnd, GameState& gamestate) {
		ClearDelay();
		int end = 1;
		std::cout << menu_main << std::endl << std::endl;
		std::cout << menu_nav;
		SetConsoleCursorPosition(iohwnd.stdconsolehwnd, { 0,3 });

		while (end) {
			GetConsoleScreenBufferInfo(iohwnd.stdconsolehwnd, &iohwnd.bufferinfo);
			int y = iohwnd.bufferinfo.dwCursorPosition.Y;
			Sleep(100);
			short code = 0;
			code = (GetAsyncKeyState(VK_RETURN) & 0b000000000000001);
			if (code) {
				if (current_page == 0) {
					if (y == 3) {
						system("cls");
						std::cout << menu_filename;
						std::string str = iohwnd.edit_routine(1);
						if (str != "Invalid") {
							system("cls");
							std::cout << menu_esc << std::endl;
							iohwnd.read_routine(str);
							iohwnd.edit_routine(0);
							std::cout << "File \"" << str << "\" has been saved.\n\n";
						}
						else {
							system("cls");
						}
						std::cout << menu_back;
						current_page = 3;
					}
					else if (y == 4) {
						system("cls");
						std::cout << menu_filename;
						std::string str = iohwnd.edit_routine(1);
						if (str != "Invalid") {
							std::remove((str + ".txt").c_str());
						}
						system("cls");
						std::cout << "File \"" << str << "\" has been deleted.\n\n";
						std::cout << menu_back;
						current_page = 3;
					}
					else if (y == 5) {
						system("cls");
						std::cout << menu_filename;
						std::string str = iohwnd.edit_routine(1);
						if (str != "Invalid") {
							system("cls");
							std::cout << menu_esc << std::endl;
							iohwnd.open_routine(str);
							iohwnd.edit_routine(0);
							std::cout << "File \"" << str << "\" has been saved.\n\n";
						}
						else {
							system("cls");
						}
						std::cout << menu_back;
						current_page = 3;
					}
					else if (y == 6) {
						system("cls");
						std::cout << menu_filename;
						std::string str = iohwnd.edit_routine(1);
						if (str != "Invalid") {
							FileHandle file(str);
							FileHandle copy = file++;
							system("cls");
							std::cout << menu_esc << std::endl;
							iohwnd.read_routine(copy.filename);
							iohwnd.edit_routine(0);
							std::cout << "File \"" << copy.filename << "\" has been saved.\n\n";
						}
						else {
							system("cls");
						}
						std::cout << menu_back;
						current_page = 3;
					}
					else if (y == 7) {
						system("cls");
						std::cout << menu_filename;
						std::string str1 = iohwnd.edit_routine(1);
						if (str1 != "Invalid") {
							std::cout << std::endl << "Enter second file name: ";
							std::string str2 = iohwnd.edit_routine(1);
							if (str2 != "Invalid") {
								system("cls");
								FileHandle file1(str1);
								FileHandle file2(str2);
								FileHandle merged = file1 + file2;
								std::cout << menu_esc << std::endl;
								iohwnd.read_routine(merged.filename);
								iohwnd.edit_routine(0);
								std::cout << "File \"" << merged.filename << "\" has been saved.\n\n";
							}
							else {
								system("cls");
							}
						}
						else {
							system("cls");
						}
						std::cout << menu_back;
						current_page = 3;
					}
					else if (y == 8) {
						system("cls");
						std::cout << menu_colors << std::endl;
						std::cout << menu_nav;
						current_page = 1;
						SetConsoleCursorPosition(iohwnd.stdconsolehwnd, { 0,0 });
					}
					else if (y == 9) {
						system("cls");
						std::cout << menu_colors << std::endl;
						std::cout << menu_nav;
						current_page = 2;
						SetConsoleCursorPosition(iohwnd.stdconsolehwnd, { 0,0 });
					}
					else if (y == 10) {
						system("cls");
						std::cout << "Use 'w' and 's' to control player One and arrow keys to control player Two\n";
						std::cout << "Press 'Enter' to continue..";
						ClearDelay();
						int code1 = 0;
						while (!code1) {
							Sleep(100);
							code1 = (GetAsyncKeyState(VK_RETURN) & 0b000000000000001);
						}
						system("cls");
						std::cout << "Press 'Esc' to go back to Main menu." << std::endl;
						std::cout << "Use 'w' and 's' to control player One and arrow keys to control player Two\n";
						CONSOLE_CURSOR_INFO cursorinfo;
						GetConsoleCursorInfo(iohwnd.stdconsolehwnd, &cursorinfo);
						cursorinfo.bVisible = false; // Set the cursor visibility to false
						SetConsoleCursorInfo(iohwnd.stdconsolehwnd, &cursorinfo);
						int w = gamestate.Game_routine(iohwnd.stdconsolehwnd);
						GetConsoleCursorInfo(iohwnd.stdconsolehwnd, &cursorinfo);
						cursorinfo.bVisible = true;
						SetConsoleCursorInfo(iohwnd.stdconsolehwnd, &cursorinfo);
						Sleep(1000);
						system("cls");
						if (w == 1) {
							std::cout << "Player One has won the game!\n\n";
						}
						else if (w == 2) {
							std::cout << "Player Two has won the game!\n\n";
						}
						ClearDelay();
						std::cout << menu_back;
						current_page = 3;
					}
					else if (y == 11) {
						system("cls");
						end = 0;
					}
				}
				else if (current_page == 1) {
					unsigned char c = iohwnd.bufferinfo.wAttributes;
					c &= 0x0F;
					c |= y << 4;
					SetConsoleTextAttribute(iohwnd.stdconsolehwnd, c);
					system("cls");
					std::cout << menu_main << std::endl << std::endl;
					std::cout << menu_nav;
					SetConsoleCursorPosition(iohwnd.stdconsolehwnd, { 0,3 });
					current_page = 0;
				}
				else if (current_page == 2) {
					unsigned char c = iohwnd.bufferinfo.wAttributes;
					c &= 0xF0;
					c |= y;
					SetConsoleTextAttribute(iohwnd.stdconsolehwnd, c);
					system("cls");
					std::cout << menu_main << std::endl << std::endl;
					std::cout << menu_nav;
					SetConsoleCursorPosition(iohwnd.stdconsolehwnd, { 0,3 });
					current_page = 0;
				}
				else if (current_page == 3) {
					system("cls");
					std::cout << menu_main << std::endl << std::endl;
					std::cout << menu_nav;
					ClearDelay();
					SetConsoleCursorPosition(iohwnd.stdconsolehwnd, { 0,3 });
					current_page = 0;
				}
				continue;
			}
			if (current_page != 3) {
				code = (GetAsyncKeyState(VK_UP) & 0b000000000000001);
				if (code) {
					if (current_page == 0 && y > 3) {
						iohwnd.bufferinfo.dwCursorPosition.Y--;
						SetConsoleCursorPosition(iohwnd.stdconsolehwnd, iohwnd.bufferinfo.dwCursorPosition);
					}
					else if (y > 0 && current_page != 0) {
						iohwnd.bufferinfo.dwCursorPosition.Y--;
						SetConsoleCursorPosition(iohwnd.stdconsolehwnd, iohwnd.bufferinfo.dwCursorPosition);
					}
					continue;
				}
				code = (GetAsyncKeyState(VK_DOWN) & 0b000000000000001);
				if (code) {
					if (current_page == 0 && y < 11) {
						iohwnd.bufferinfo.dwCursorPosition.Y++;
						SetConsoleCursorPosition(iohwnd.stdconsolehwnd, iohwnd.bufferinfo.dwCursorPosition);
					}
					else if (current_page != 0 && y < 15) {
						iohwnd.bufferinfo.dwCursorPosition.Y++;
						SetConsoleCursorPosition(iohwnd.stdconsolehwnd, iohwnd.bufferinfo.dwCursorPosition);
					}
					continue;
				}
			}
		}
	}
};
template <typename T>
class tempc {
private:
	T val;

public:
	tempc(T val1) : val(val1) {}

	T getval() const {
		return val;
	}
};

template <typename T>
T add(tempc<T> obj1, tempc<T> obj2) {
	return obj1.getval() + obj2.getval();
}
int main() {
	int n1, n2, n3;
	(n3 = 15,system("cls"));
	std::string str1, str2, str3;
	str3 = "Password";
	std::cout << "Enter Name: ";
	std::cin >> str1;
	std::cout << "Enter Password: ";
	std::cin >> str2;
	std::cout << "Enter two numbers: ";
	std::cin >> n1 >> n2;
	tempc <std::string> s1(str1);
	tempc <std::string> s2(str2);
	tempc <int> num1(n1);
	tempc <int> num2(n2);
	tempc <std::string> c1 = add(s1, s2);
	tempc <int> c2 = add(num1, num2);
	if (c1.getval() != str3 || c2.getval()!= n3) {
		std::cout << "Incorrect Password";
		return 0;
	}
	system("cls");
	Menu menu;
	IOHandler iohwnd;
	GameState gs;
	Player p1(gs.map), p2(gs.map);
	Ball ball(gs.map, &p1, &p2);
	Explosion exp(&p1, &p2, &ball);
	Empty* base[4] = { &p1,&p2,&ball,&exp };
	gs.Setbase(base);
	menu.choice_routine(iohwnd, gs);
	return 0;
}