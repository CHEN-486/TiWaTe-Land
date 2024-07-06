#pragma once
#include <vector>
#include <graphics.h>
using namespace std;
//游戏开始，运行，得分
bool running = false;
bool is_game_started = false;
int score = 0;

const unsigned int  window_wight = 1280;
const unsigned int  window_high = 720;

const unsigned int button_wight = 192;
const unsigned int button_hight = 75;


#pragma comment (lib,"MSIMG32.LIB")
#pragma comment(lib, "Winmm.lib")
//角色透明化背景
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

//角色桢序列
class atlas
{
public:
	atlas(LPCTSTR path, int num)
	{
		TCHAR anime_path[256];
		for (int i = 0; i < num; ++i)
		{
			_stprintf_s(anime_path, path, i);
			IMAGE* fram = new IMAGE;
			loadimage(fram, anime_path);
			frame_list.push_back(fram);
		}
	}
	atlas() = default;
	~atlas()
	{
		for (int i = 0; i < frame_list.size(); ++i)
			delete frame_list[i];
	}
public:
	vector<IMAGE*> frame_list;
};//享元设计
atlas* ATSplayer_left;
atlas* ATSplayer_right;
atlas* ATSenemy_left;
atlas* ATSenemy_right;

//角色动画加载
class anime
{
public:
	anime(atlas* atlas, int interval)
	{
		anime_interval = interval;
		anim_atlas = atlas;
	}
	anime() = default;
	~anime() = default;


	void play(int time, int x, int y)
	{
		timer += time;
		if (timer > anime_interval)
		{
			count_anime = (count_anime + 1) % anim_atlas->frame_list.size();
			timer = 0;
		}
		putimage_alpha(x, y, anim_atlas->frame_list[count_anime]);
	}
private:
	int timer;
	int count_anime = 0;
	int anime_interval;
	atlas* anim_atlas;
};

//玩家类的创建
class player
{
public:
	POINT player_pos = { 500,500 };
	const int player_wight = 80;
	const int player_high = 80;
public:
	player()
	{
		loadimage(&Pshadow, _T("img/shadow_player.png"));
		player_left = new anime(ATSplayer_left, 45);
		player_right = new anime(ATSplayer_right, 45);
	}
	~player()
	{
		delete player_left;
		delete player_right;
	}


	void draw_player(int time)
	{
		int pos_shadow_x = player_pos.x + (player_wight - player_shadow) / 2;
		int pos_shadow_y = player_pos.y + player_high - 8;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &Pshadow);
		static bool facing_left = true;
		int dir_x = ismove_right - ismove_left;
		if (dir_x > 0)
			facing_left = false;
		else if (dir_x < 0)
			facing_left = true;
		if (facing_left)
			player_left->play(time, player_pos.x, player_pos.y);
		else
			player_right->play(time, player_pos.x, player_pos.y);
	}

	void processevent(ExMessage msg)
	{
		if (msg.message == WM_KEYDOWN)
		{
			switch (msg.vkcode)
			{
			case  VK_UP:
				ismove_up = true;
				break;
			case VK_LEFT:
				ismove_left = true;
				break;
			case VK_RIGHT:
				ismove_right = true;
				break;
			case VK_DOWN:
				ismove_down = true;
				break;
			default:
				break;
			}
		}
		if (msg.message == WM_KEYUP)
		{
			switch (msg.vkcode)
			{
			case  VK_UP:
				ismove_up = false;
				break;
			case VK_LEFT:
				ismove_left = false;
				break;
			case VK_RIGHT:
				ismove_right = false;
				break;
			case VK_DOWN:
				ismove_down = false;
				break;
			default:
				break;
			}
		}
	}

	void move()
	{
		int dir_x = ismove_right - ismove_left;
		int  dir_y = ismove_down - ismove_up;
		double one_len = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (one_len != 0)
		{
			double other_x = dir_x / one_len;
			double other_y = dir_y / one_len;
			player_pos.x += (int)(other_x * player_speed);
			player_pos.y += (int)(other_y * player_speed);
		}

		if (player_pos.x < 0)player_pos.x = 0;
		if (player_pos.y < 0)player_pos.y = 0;
		if (player_pos.x > window_wight - player_wight)player_pos.x = window_wight - player_wight;
		if (player_pos.y > window_high - player_high)player_pos.y = window_high - player_high;
	}


private:
	const int player_shadow = 32;
	const int player_speed = 5;
private:
	anime* player_left;
	anime* player_right;
	IMAGE Pshadow;
	bool ismove_right = false;
	bool ismove_left = false;
	bool ismove_up = false;
	bool ismove_down = false;
};

class bullet
{
public:
	POINT bullet_pos;
public:
	bullet() = default;
	~bullet() = default;
	void draw()
	{
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));
		fillcircle(bullet_pos.x, bullet_pos.y, radius);
	}
private:
	int radius = 10;
};

class enemy
{
public:
	enemy()
	{
		loadimage(&Eshadow, _T("img/shadow_enemy.png"));
		enemy_left = new anime(ATSenemy_left, 45);
		enemy_right = new anime(ATSenemy_right, 45);
		born();
	}
	~enemy()
	{
		delete enemy_left;
		delete enemy_right;
	}



	void draw_enemy(int time)
	{
		int pos_shadow_x = enemy_pos.x + (enemy_wight - enemy_shadow) / 2;
		int pos_shadow_y = enemy_pos.y + enemy_high - 35;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &Eshadow);
		if (facing_left)
			enemy_left->play(time, enemy_pos.x, enemy_pos.y);
		else
			enemy_right->play(time, enemy_pos.x, enemy_pos.y);
	}
	void move(const player& play)
	{
		int dir_x = play.player_pos.x - enemy_pos.x;
		int  dir_y = play.player_pos.y - enemy_pos.y;
		double one_len = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (one_len != 0)
		{
			enemy_pos.x += (int)(enemy_speed * dir_x / one_len);
			enemy_pos.y += (int)(enemy_speed * dir_y / one_len);
		}
		if (dir_x > 0)
			facing_left = false;
		else if (dir_x < 0)
			facing_left = true;
	}
	void born()
	{
		int edge = rand() % 4;
		switch (edge)
		{
		case 0:
			enemy_pos.x = rand() % window_wight;
			enemy_pos.y = -enemy_high;
			break;
		case 1:
			enemy_pos.x = rand() % window_wight;
			enemy_pos.y = window_high;
			break;
		case 2:
			enemy_pos.x = -enemy_wight;
			enemy_pos.y = rand() % window_high;
			break;
		case 3:
			enemy_pos.x = window_wight;
			enemy_pos.y = rand() % window_high;
			break;
		default:
			break;
		}

	}
	bool check_player(const player& player)
	{
		POINT check_posation = { player.player_pos.x + player.player_wight / 2,player.player_pos.y + player.player_high / 2 };
		if (check_posation.y > enemy_pos.y && check_posation.y<enemy_pos.y + enemy_high
			&& check_posation.x>enemy_pos.x && check_posation.x < enemy_pos.x + enemy_wight)
			return true;
		else
			return false;

	}
	bool check_bullet(const bullet& bullet)
	{
		if (bullet.bullet_pos.x > enemy_pos.x && bullet.bullet_pos.x<enemy_pos.x + enemy_wight
			&& bullet.bullet_pos.y>enemy_pos.y && bullet.bullet_pos.y < enemy_pos.y + enemy_high)
			return true;
		else
			return false;
	}

	void hurt()
	{
		alive = false;
	}
	bool check_alive()
	{
		return alive;
	}
private:
	const int enemy_wight = 80;
	const int enemy_high = 80;
	const int enemy_shadow = 48;
	const int enemy_speed = 2;
private:
	anime* enemy_left;
	anime* enemy_right;
	IMAGE Eshadow;
	POINT enemy_pos;
	bool facing_left = true;
	bool alive = true;
};

class Button
{

public:
	Button(RECT rect, LPCTSTR path)
	{
		region = rect;
		for (int i = 0; i < num; ++i)
		{
			TCHAR button_path[256];
			for (int i = 0; i < num; ++i)
			{
				_stprintf_s(button_path, path, i);
				IMAGE* fram = new IMAGE;
				loadimage(fram, button_path);
				button_list.push_back(fram);
			}
		}

	}
	~Button() = default;
	void Draw()
	{
		putimage(region.left, region.top, button_list[statement]);
	}
	void  ProcessEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			if (statement == state::original && CheckCursorHit(msg.x, msg.y))
				statement = state::hover;
			else if (statement == state::hover && !CheckCursorHit(msg.x, msg.y))
				statement = state::original;
			break;
		case WM_LBUTTONDOWN:
			if (CheckCursorHit(msg.x, msg.y))
				statement = state::down;
			break;
		case WM_LBUTTONUP:
			if (statement == state::down)
				OnClick();
			break;
		default:
			break;
		}
	}
protected:
	virtual  void OnClick() = 0;
private:
	bool CheckCursorHit(int x, int y)
	{
		return x >= region.left && x <= region.right && y >= region.top && y <= region.bottom;
	}
private:
	enum state
	{
		original,
		hover,
		down
	};
private:
	const unsigned int num = 3;
	RECT region;
	vector<IMAGE*> button_list;
	state statement = state::original;
};
class StartButton :public Button
{
public:
	StartButton(RECT rect, LPCTSTR path)
		:Button(rect, path)
	{}
	~StartButton() = default;
protected:
	void OnClick()
	{
		mciSendString(_T("open mus/fight.wav alias fight"), NULL, 0, NULL);
		mciSendString(_T("play fight from 0"), NULL, 0, NULL);
		is_game_started = true;
		mciSendString(_T("stop menu"), NULL, 0, NULL);
		mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	}
};
class EndButton :public Button
{
public:
	EndButton(RECT rect, LPCTSTR path)
		:Button(rect, path)
	{}
	~EndButton() = default;
protected:
	void OnClick()
	{
		running = false;
	}
};
