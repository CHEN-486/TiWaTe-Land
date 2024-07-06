#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>
#include <string>
#include <vector>
#include "anime.h"
using namespace std;

//生成敌人
void born_enemy(vector<enemy*>& enemy_list)
{
	static int counter = 0;
	const int interval = 100;
	if ((++counter) % interval == 0)
	{
		enemy_list.push_back(new enemy);
		counter = 0;
	}
}
void born_bullet(vector<bullet*>& bullet_list)
{
	for (int i = 0; i < bullet_list.size(); ++i)
		bullet_list[i] = new bullet;
	
}

void updatebullet_list(vector<bullet*> &bullet_list,const player &player)
{
	const double jing_speed = 0.0045;
	const double qie_speed = 0.0055;
	double angle = 2 * 3.1415926 / bullet_list.size();
	double jing = 100 + 25 * sin(GetTickCount() * jing_speed);
	for(int i=0;i<bullet_list.size();++i)
	{
		bullet_list[i]->bullet_pos.x = player.player_pos.x + player.player_wight / 2 + (int)(jing * cos(i * angle + GetTickCount() * qie_speed));
		bullet_list[i]->bullet_pos.y = player.player_pos.y + player.player_high / 2 + (int)(jing * sin(i * angle + GetTickCount() * qie_speed));
	}
}

void draw_score(int score)
{
	static TCHAR text[64];
	_stprintf_s(text, _T("当前得分为 %d"), score);
	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 50, 88));
	outtextxy(10, 10, text);
}

int main()
{
	ExMessage msg0;
	IMAGE startground;
	IMAGE start0;
	IMAGE start;
	IMAGE start1;
	IMAGE end0;
	IMAGE end;
	IMAGE end1;
	
	initgraph(window_wight, window_high);
	running = true;
	ATSplayer_left = new atlas(_T("img/player_left_%d.png"), 6);
	ATSplayer_right = new atlas(_T("img/player_right_%d.png"), 6);
	ATSenemy_left = new atlas(_T("img/enemy_left_%d.png"), 6);
	ATSenemy_right = new atlas(_T("img/enemy_right_%d.png"), 6);


	ExMessage msg;
	IMAGE background;
	IMAGE img_menu;
	player player;
	vector<enemy*> enemy_list;
	vector<bullet*> bullet_list(3);

	RECT region_start, region_end;
	region_start.left = (window_wight - button_wight) / 2;
	region_start.right = region_start.left + button_wight;
	region_start.top = 430;
	region_start.bottom = region_start.top + button_hight;

	region_end.left = (window_wight - button_wight) / 2;
	region_end.right = region_end.left + button_wight;
	region_end.top = 550;
	region_end.bottom = region_end.top + button_hight;

	StartButton start_game(region_start, _T("img/ui_start_%d.png"));
	EndButton  end_game(region_end, _T("img/ui_quit_%d.png"));

	born_bullet(bullet_list);

	loadimage(&img_menu, _T("img/menu.png"));
	loadimage(&background, _T("img/background.png"));
	BeginBatchDraw();
	mciSendString(_T("open mus/bgm.mp3 alias menu"), NULL, 0, NULL);
	mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);
	mciSendString(_T("open mus/ganma.mp3 alias die"), NULL, 0, NULL);
	mciSendString(_T("open mus/MEGALOVANIA.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("play menu repeat from 0"), NULL, 0, NULL);
	while (running)
	{
		DWORD start_time = GetTickCount();
		while (peekmessage(&msg))
		{
			if (is_game_started)
				player.processevent(msg);
			else 
			{
				start_game.ProcessEvent(msg);
				end_game.ProcessEvent(msg);
			}
			
		}
		if (is_game_started)
		{
			born_enemy(enemy_list);
			player.move();
			updatebullet_list(bullet_list, player);
			for (enemy* enemy : enemy_list)
				enemy->move(player);
			for (enemy* enemy : enemy_list)
			{
				if (enemy->check_player(player))
				{
					mciSendString(_T("play die from 0"), NULL, 0, NULL);
					TCHAR text[64];
					_stprintf_s(text, _T("最终得分为 %d"), score);
					MessageBox(GetHWnd(), text, _T("游戏结束 "), MB_OK);
					running = false;
					break;
				}
			}
			for (enemy* enemy : enemy_list)
			{
				for (bullet* bullet : bullet_list)
				{
					if (enemy->check_bullet(*bullet))
					{
						enemy->hurt();
						score++;
						mciSendString(_T("play hit from 0"), NULL, 0, NULL);
					}
				}
			}
			for (int i = 0; i < enemy_list.size(); ++i)
			{
				enemy* enemy_dele = enemy_list[i];
				if (!enemy_list[i]->check_alive())
				{
					swap(enemy_list[i], enemy_list.back());
					enemy_list.pop_back();
					delete enemy_dele;
				}
			}
		}
		cleardevice();
		if (is_game_started)
		{
			putimage(0, 0, &background);
			player.draw_player(1000 / 144);
			for (enemy* enemy : enemy_list)
				enemy->draw_enemy(1000 / 144);
			for (bullet* bullet : bullet_list)
				bullet->draw();
			draw_score(score);
		}
		else
		{
			putimage(0, 0, &img_menu);
			start_game.Draw();
			end_game.Draw();
		}
		FlushBatchDraw();
		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if(delta_time<1000/144)
		{
			Sleep(1000 / 144 - delta_time);
		}
	}
	EndBatchDraw();
	return 0;
}