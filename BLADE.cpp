#include <SDKDDKVer.h>
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include <algorithm>
#include <ctime>
#include <iostream>
using namespace std;

#define N_CARD	20
#define WITH_COMPUTER	true

enum Card:char
{
	NoneCard=0, One=1, Two=2, Three=3, Four=4, Five=5, Six=6, Seven=7, Bolt=8, Mirror=9
};

enum Player:char
{
	PlayerA = -1, PlayerB = 1, NonePlayer = 0
};


class Blade
{
public:
	Blade(int n)
		:nCard(n), PlayerA(), PlayerB(), PlayerA_Ext(), PlayerB_Ext()
	{
		ReNew();
	}

	void ReNew()
	{
		PlayerA.clear();
		PlayerB.clear();
		PlayerA_Ext.clear();
		PlayerB_Ext.clear();

		for (int i = 0; i < nCard; i++)
		{
			PlayerA.push_back((Card)(rand() % 9 + 1));
			PlayerB.push_back((Card)(rand() % 9 + 1));
		}
		sort(PlayerA.begin(), PlayerA.end());
		sort(PlayerB.begin(), PlayerB.end());

		PlayerA_Ext.push_back((Card)(rand() % 6 + 2));
		PlayerB_Ext.push_back((Card)(rand() % 6 + 2));

		while (PlayerA_Ext[0] == PlayerB_Ext[0])
			PlayerA_Ext[0] = ((Card)(rand() % 6 + 2));

		sumA = PlayerA_Ext[0];
		sumB = PlayerB_Ext[0];

		if (PlayerA_Ext[0] > PlayerB_Ext[0])
			Turn = Player::PlayerB;
		else
			Turn = Player::PlayerA;
	}

	Player Throw(Player player,unsigned int cardindex)
	{
		if (player == Player::PlayerA)
		{
			wchar_t sum[10];
			wsprintfW(sum, L"%d\n", PlayerA[cardindex]);
			OutputDebugStringW(sum);
		}
		if (player == Player::PlayerA)
		{
			if (Turn == Player::PlayerB)
				throw std::logic_error("It's not PlayerA's turn.");
			if (cardindex >= PlayerA.size())
				throw std::invalid_argument("Invlaid index.");

			if (PlayerA[cardindex] == Bolt)
			{
				if (PlayerB_Ext[PlayerB_Ext.size() - 1] < 0)
					Player::NonePlayer;
				PlayerB_Ext[PlayerB_Ext.size()-1] = (Card)-(char)PlayerB_Ext[PlayerB_Ext.size()-1];
			}
			else if (PlayerA[cardindex] == Mirror)
			{
				Card tmp = PlayerA_Ext[PlayerA_Ext.size()-1];
				PlayerA_Ext[PlayerA_Ext.size()-1] = PlayerB_Ext[PlayerB_Ext.size()-1];
				PlayerB_Ext[PlayerB_Ext.size()-1] = tmp;
			}
			else if (PlayerA[cardindex] == One)
			{
				if (PlayerA_Ext[PlayerA_Ext.size()-1] < 0)
					PlayerA_Ext[PlayerA_Ext.size()-1] = (Card)-(char)PlayerA_Ext[PlayerA_Ext.size()-1];
				PlayerA_Ext.push_back(PlayerA[cardindex]);
			}
			else
			{
				PlayerA_Ext.push_back(PlayerA[cardindex]);
			}

			PlayerA.erase(PlayerA.begin() + cardindex);
		}
		else if (player == Player::PlayerB)
		{
			if (Turn == Player::PlayerA)
				throw std::logic_error("It's not PlayerA's turn.");
			if (cardindex >= PlayerB.size())
				throw std::invalid_argument("Invlaid index.");

			if (PlayerB[cardindex] == Bolt)
			{
				if (PlayerA_Ext[PlayerA_Ext.size() - 1] < 0)
					return Player::NonePlayer;
				PlayerA_Ext[PlayerA_Ext.size()-1] = (Card)-(char)PlayerA_Ext[PlayerA_Ext.size()-1];
			}
			else if (PlayerB[cardindex] == Mirror)
			{
				Card tmp = PlayerA_Ext[PlayerA_Ext.size()-1];
				PlayerA_Ext[PlayerA_Ext.size()-1] = PlayerB_Ext[PlayerB_Ext.size()-1];
				PlayerB_Ext[PlayerB_Ext.size()-1] = tmp;
			}
			else if (PlayerB[cardindex] == One)
			{
				if (PlayerB_Ext[PlayerB_Ext.size()-1] < 0)
					PlayerB_Ext[PlayerB_Ext.size()-1] = (Card)-(char)PlayerB_Ext[PlayerB_Ext.size()-1];
				PlayerB_Ext.push_back(PlayerB[cardindex]);
			}
			else
			{
				PlayerB_Ext.push_back(PlayerB[cardindex]);
			}

			PlayerB.erase(PlayerB.begin() + cardindex);
		}
		else
			throw std::invalid_argument("Invalid player.");

		Turn = (Player)-(char)Turn;

		sumA = 0, sumB = 0;

		for each (Card var in PlayerA_Ext)
		{
			if (var > 0)
				sumA += var;
		}
		for each (Card var in PlayerB_Ext)
		{
			if (var > 0)
				sumB += var;
		}

		if (sumA == sumB)
		{
			PlayerA_Ext.clear();
			PlayerB_Ext.clear();

			PlayerA_Ext.push_back((Card)(rand() % 6 + 2));
			PlayerB_Ext.push_back((Card)(rand() % 6 + 2));

			while (PlayerA_Ext[0] == PlayerB_Ext[0])
				PlayerA_Ext[0] = ((Card)(rand() % 6 + 2));

			if (PlayerA_Ext[0] > PlayerB_Ext[0])
				Turn = Player::PlayerB;
			else
				Turn = Player::PlayerA;

			sumA = PlayerA_Ext[0];
			sumB = PlayerB_Ext[0];

			{
				wchar_t sum[10];
				wsprintfW(sum, L"ReNew\n");
				OutputDebugStringW(sum);
			}
		}
		else if (sumA < sumB)
		{
			if (Turn == Player::PlayerB)
				return Player::PlayerB;
		}
		else
		{
			if (Turn == Player::PlayerA)
				return Player::PlayerA;
		}

		if (Turn == Player::PlayerA && PlayerA.size() == 0)
			return Player::PlayerB;
		if (Turn == Player::PlayerB && PlayerB.size() == 0)
			return Player::PlayerA;

		return Player::NonePlayer;
	}

	Player RunAI()
	{
		if (Turn == Player::PlayerB)
			throw std::logic_error("Invaild Player");

		int dif = sumB - sumA;
		auto one = find(PlayerA.begin(), PlayerA.end(), One);
		auto mirror = find(PlayerA.begin(), PlayerA.end(), Mirror);
		auto bolt = find(PlayerA.begin(), PlayerA.end(), Bolt);

		if (PlayerA_Ext[PlayerA_Ext.size() - 1]<-3 && PlayerA_Ext[PlayerA_Ext.size() - 1]<=-dif && one != PlayerA.end())
			return Throw(Player::PlayerA, one - PlayerA.begin());

		if (PlayerB_Ext[PlayerB_Ext.size() - 1] > 5 && PlayerB_Ext[PlayerB_Ext.size() - 1] >= dif && bolt != PlayerA.end())
			return Throw(Player::PlayerA, bolt - PlayerA.begin());

		if (PlayerB_Ext[PlayerB_Ext.size() - 1] - PlayerA_Ext[PlayerA_Ext.size() - 1] > 3 && 2*(PlayerB_Ext[PlayerB_Ext.size() - 1] - PlayerA_Ext[PlayerA_Ext.size() - 1]) >= dif && mirror != PlayerA.end())
			return Throw(Player::PlayerA, mirror - PlayerA.begin());

		auto difcard = find(PlayerA.begin(), PlayerA.end(), dif);

		for (unsigned int i = 0; i < PlayerA.size() && PlayerA[i] <= 7; i++)
		{
			if (PlayerA[i] > dif)
			{
				if (PlayerA[i] > dif + 2)
				{
					if (difcard == PlayerA.end())
						return Throw(Player::PlayerA, i);
					else
						return Throw(Player::PlayerA, difcard - PlayerA.begin());
				}
			}
		}

		if (difcard == PlayerA.end())
			return Throw(Player::PlayerA, 0);
		else
			return Throw(Player::PlayerA, difcard - PlayerA.begin());
	}

	Card RunAI2()
	{
		if (Turn == Player::PlayerB)
			throw std::logic_error("Invaild Player");

		int dif = sumB - sumA;
		auto one = find(PlayerA.begin(), PlayerA.end(), One);
		auto mirror = find(PlayerA.begin(), PlayerA.end(), Mirror);
		auto bolt = find(PlayerA.begin(), PlayerA.end(), Bolt);

		if (PlayerA_Ext[PlayerA_Ext.size() - 1]<-3 && PlayerA_Ext[PlayerA_Ext.size() - 1] <= -dif && one != PlayerA.end())
			return One;

		if (PlayerB_Ext[PlayerB_Ext.size() - 1] > 5 && PlayerB_Ext[PlayerB_Ext.size() - 1] >= dif && bolt != PlayerA.end())
			return Bolt;

		if (PlayerB_Ext[PlayerB_Ext.size() - 1] - PlayerA_Ext[PlayerA_Ext.size() - 1] > 3 && 2 * (PlayerB_Ext[PlayerB_Ext.size() - 1] - PlayerA_Ext[PlayerA_Ext.size() - 1]) >= dif && mirror != PlayerA.end())
			return Mirror;

		auto difcard = find(PlayerA.begin(), PlayerA.end(), dif);

		for (unsigned int i = 0; i < PlayerA.size() && PlayerA[i] <= 7; i++)
		{
			if (PlayerA[i] > dif)
			{
				if (PlayerA[i] > dif + 2)
				{
					if (difcard == PlayerA.end())
						return PlayerA[i];
					else
						return *difcard;
				}
			}
		}

		if (difcard == PlayerA.end())
			return PlayerA[0];
		else
			return *difcard;
	}

	inline Player GetTurn() const{ return Turn; }
	inline const vector<Card> & GetPlayerA() const{ return PlayerA; }
	inline const vector<Card> & GetPlayerB() const{ return PlayerB; }
	inline const vector<Card> & GetPlayerA_Ext() const{ return PlayerA_Ext; }
	inline const vector<Card> & GetPlayerB_Ext() const{ return PlayerB_Ext; }
	inline int GetSumA() const{ return sumA; }
	inline int GetSumB() const{ return sumB; }
	inline int GetCard() const{ return nCard; }

private:
	const int nCard;
	vector<Card> PlayerA, PlayerB, PlayerA_Ext, PlayerB_Ext;
	int sumA, sumB;
	Player Turn;
};

void Print(Blade & b, bool DispA)
{
	cout << "Tom :";
	for each (auto var in b.GetPlayerA())
	{
		if (DispA)
		{
			if (var == Bolt)
				cout << "b ";
			else if (var == Mirror)
				cout << "m ";
			else
				cout << var<< ' ';
		}
		else
		{
			cout << "X ";
		}
	}
	cout << "/ ";
	for each (auto var in b.GetPlayerA_Ext())
	{
		if (var == Bolt)
			cout << "b ";
		else if (var == Mirror)
			cout << "m ";
		else
			cout << var<<' ';
	}
	cout << '/' <<endl;

	cout << "John:";
	for each (auto var in b.GetPlayerB())
	{
		if (var == Bolt)
			cout << "b ";
		else if (var == Mirror)
			cout << "m ";
		else
			cout << var<<' ';
	}
	cout << "/ ";
	for each (auto var in b.GetPlayerB_Ext())
	{
		if (var == Bolt)
			cout << "b ";
		else if (var == Mirror)
			cout << "m ";
		else
			cout << var<<' ';
	}
	cout << '/' << endl;
}

inline void EnterNext(){
	system("cls");
	//cout << "-------------------------" << endl << endl;
}

int main(int argc, char** argv)
{
	Blade b(N_CARD);
	bool playwithcomputer;
	char ch;

ReInput:
	cout << "Do you want to play with computer(Y/N)?" << endl;
	if (!(cin >> ch))
		abort();
	fflush(stdin);
	if (ch == 'Y' || ch == 'y')
		playwithcomputer = true;
	else if (ch == 'N' || ch == 'n')
		playwithcomputer = false;
	else
		goto ReInput;

	EnterNext();

	b.ReNew();
	cout << "Welcome to BALDE!\n";
	cout << "Tom  gets" << b.GetPlayerA_Ext()[0] << endl;
	cout << "John gets" << b.GetPlayerB_Ext()[0] << endl;
	cout << (b.GetPlayerA_Ext()[0] > b.GetPlayerB_Ext()[0] ? "Tom " : "John") << " goes first.";
	
	Sleep(3000);
	EnterNext();
	

	for (int i = 1;; i++)
	{
		cout << "Round" << i << '\t' << "Tom " << b.GetSumA() << '\t' << "John " << b.GetSumB() << endl;
		Print(b, !playwithcomputer);
		if (b.GetTurn() == PlayerA)
		{
		RePrint:
			cout << "Tom plays: ";
			Card var;
			if (playwithcomputer)
			{
				var = b.RunAI2();
				if (var == Bolt)
					cout << "b" << endl;
				else if (var == Mirror)
					cout << "m" << endl;
				else
					cout << var << endl;
			}
			else
			{
				char ch;
				if (!(cin >> ch))
					abort();
				fflush(stdin);
				if (ch == 'b')
					var = Bolt;
				else if (ch == 'm')
					var = Mirror;
				else if (ch >= '1' && ch <= '7')
					var = (Card)(ch - '0');
				else
					goto RePrint;
			}
			Player p = b.Throw(PlayerA, find(b.GetPlayerA().begin(), b.GetPlayerA().end(), var) - b.GetPlayerA().begin());

			if (p == PlayerA)
				cout << "Tom win.";
			else if (p == PlayerB)
				cout << "John win.";
			else
			{
				Sleep(3000);
				EnterNext();
				continue;
			}

			Sleep(3000);
			EnterNext();
			goto ReInput;
		}
		else
		{
		RePrint2:
			cout << "John plays: ";
			Card var;
			{
				char ch;
				if (!(cin >> ch))
					abort();
				fflush(stdin);
				if (ch == 'b')
					var = Bolt;
				else if (ch == 'm')
					var = Mirror;
				else if (ch >= '1' && ch <= '7')
					var = (Card)(ch - '0');
				else
					goto RePrint2;
			}
			Player p = b.Throw(PlayerB, find(b.GetPlayerB().begin(), b.GetPlayerB().end(), var) - b.GetPlayerB().begin());

			if (p == PlayerA)
				cout << "Tom win.";
			else if (p == PlayerB)
				cout << "John win.";
			else
			{
				Sleep(3000);
				EnterNext();
				continue;
			}

			Sleep(3000);
			EnterNext();
			goto ReInput;
		}
	}
}

HINSTANCE hInst;
HWND hWnd;
HFONT fSmall;
HFONT fBig;
HBRUSH bWrite, bTitle, bCard, bCardDisabled;
int nCARD;
int CardWeidth;
Blade* BladeGame;

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	srand((unsigned int)time(NULL));

	nCARD = N_CARD;
	CardWeidth = 60;
	fSmall = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, FF_DONTCARE, _TEXT("微软雅黑"));
	fBig = CreateFont(28, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, FF_DONTCARE, _TEXT("微软雅黑"));
	bWrite = CreateSolidBrush(RGB(255, 255, 255));
	bTitle = CreateSolidBrush(RGB(255, 201, 14));
	bCard = CreateSolidBrush(RGB(0, 162, 232));
	bCardDisabled = CreateSolidBrush(RGB(79, 128, 153));

	BladeGame = new Blade(nCARD);

	while (WITH_COMPUTER && BladeGame->GetTurn() == PlayerA)
	{
		Player p = BladeGame->RunAI();

		if (p == PlayerA)
		{
			MessageBox(NULL, WITH_COMPUTER ? _TEXT("You lose!") : _TEXT("A win!"), _TEXT("Blade"), MB_OK);
			BladeGame->ReNew();
		}
		if (p == PlayerB)
		{
			MessageBox(NULL, WITH_COMPUTER ? _TEXT("You win!") : _TEXT("B win!"), _TEXT("Blade"), MB_OK);
			BladeGame->ReNew();
		}
	}

	MSG msg;
	MyRegisterClass(hInstance);
	InitInstance(hInstance, nCmdShow);

	while (GetMessage(&msg, NULL, 0, 0))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= _TEXT("MyBladeGameClass");
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

void SetClientSize(HWND hWnd, int width, int height)
{
	RECT  rectProgram, rectClient;
	GetWindowRect(hWnd, &rectProgram);
	GetClientRect(hWnd, &rectClient);
	int nWidth = rectProgram.right - rectProgram.left - (rectClient.right - rectClient.left);
	int nHeiht = rectProgram.bottom - rectProgram.top - (rectClient.bottom - rectClient.top);
	nWidth += width;
	nHeiht += height;
	rectProgram.right = nWidth;
	rectProgram.bottom = nHeiht;
	int showToScreenx = GetSystemMetrics(SM_CXSCREEN) / 2 - nWidth / 2;
	int showToScreeny = GetSystemMetrics(SM_CYSCREEN) / 2 - nHeiht / 2;
	MoveWindow(hWnd, showToScreenx, showToScreeny, rectProgram.right, rectProgram.bottom, false);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance;
   hWnd = CreateWindow(_TEXT("MyBladeGameClass"), _TEXT("Blade Game"), WS_OVERLAPPEDWINDOW&~WS_THICKFRAME,
	   CW_USEDEFAULT, 0, (CardWeidth * nCARD + 80), 600, NULL, NULL, hInstance, NULL);

   SetClientSize(hWnd, (CardWeidth * nCARD + 80), 600);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//上方100为开始游戏按钮
//每张牌宽度为70，间隔10，高度100
//标题高度100，间隔30
//文字宽度150

void TextPrint(HDC hdc, int i, int j, Card card)
{
	int x, y;
	LPTSTR str = _TEXT("");
	int strl = 0;

	if (card < 0)
		card = (Card)-(char)card;

	if (card == Bolt || card == Mirror)
	{
		y = 168 + i * 110;
		x = 15 + j * CardWeidth;
		SelectObject(hdc, fSmall);
	}
	else
	{
		y = 165 + i * 110;
		x = 30 + j * CardWeidth;
		SelectObject(hdc, fBig);
	}

	switch (card)
	{
	case One:
		str = _TEXT("1");
		strl = 1;
		break;
	case Two:
		str = _TEXT("2");
		strl = 1;
		break;
	case Three:
		str = _TEXT("3");
		strl = 1;
		break;
	case Four:
		str = _TEXT("4");
		strl = 1;
		break;
	case Five:
		str = _TEXT("5");
		strl = 1;
		break;
	case Six:
		str = _TEXT("6");
		strl = 1;
		break;
	case Seven:
		str = _TEXT("7");
		strl = 1;
		break;

	case Bolt:
		str = _TEXT("BOLT");
		strl = 4;
		break;
	case Mirror:
		str = _TEXT("MIRR");
		strl = 6;
		break;
	}
	
	TextOut(hdc, x, y, str, strl);
}

void OnPaint(HDC hdc)
{
	HDC cdc = CreateCompatibleDC(hdc);
	HBITMAP hbmp = CreateCompatibleBitmap(hdc, CardWeidth * nCARD + 80, 600);
	SelectObject(cdc, hbmp);
	RECT r;

	r.left = 0;
	r.right = CardWeidth * nCARD + 80;
	r.top = 0;
	r.bottom = 600;
	FillRect(cdc, &r, bWrite);
	r.bottom = 100;
	FillRect(cdc, &r, bTitle);

	SetBkMode(cdc, TRANSPARENT);
	SetTextColor(cdc, RGB(0, 0, 0));
	SelectObject(cdc, fBig);
	TextOut(cdc, r.right/2 - 75, 40, _TEXT("重新开始"), 4);

	r.top = 130;
	r.bottom = 230;
	for (unsigned int i = 0; i < BladeGame->GetPlayerA().size(); i++)
	{
		r.left = i * CardWeidth + 10;
		r.right = (i + 1) * CardWeidth;
		FillRect(cdc, &r, bCard);
		if (!WITH_COMPUTER)
			TextPrint(cdc, 0, i, BladeGame->GetPlayerA()[i]);
	}

	r.top = 240;
	r.bottom = 340;
	for (unsigned int i = 0; i < BladeGame->GetPlayerA_Ext().size(); i++)
	{
		r.left = i * CardWeidth + 10;
		r.right = (i + 1) * CardWeidth;

		if (BladeGame->GetPlayerA_Ext()[i] > 0)
			FillRect(cdc, &r, bCard);
		else
			FillRect(cdc, &r, bCardDisabled);

		TextPrint(cdc, 1, i, BladeGame->GetPlayerA_Ext()[i]);
	}

	r.top = 350;
	r.bottom = 450;
	for (unsigned int i = 0; i < BladeGame->GetPlayerB_Ext().size(); i++)
	{
		r.left = i * CardWeidth + 10;
		r.right = (i + 1) * CardWeidth;
		
		if (BladeGame->GetPlayerB_Ext()[i] > 0)
			FillRect(cdc, &r, bCard);
		else
			FillRect(cdc, &r, bCardDisabled);

		TextPrint(cdc, 2, i, BladeGame->GetPlayerB_Ext()[i]);
	}

	r.top = 460;
	r.bottom = 560;
	for (unsigned int i = 0; i < BladeGame->GetPlayerB().size(); i++)
	{
		r.left = i * CardWeidth + 10;
		r.right = (i + 1) * CardWeidth;
		FillRect(cdc, &r, bCard);

		TextPrint(cdc, 3, i, BladeGame->GetPlayerB()[i]);
	}

	if (BladeGame->GetTurn() == PlayerA)
	{
		r.top = 130;
		r.bottom = 340;
	}
	else
	{
		r.top = 350;
		r.bottom = 560;
	}

	r.left = CardWeidth * nCARD + 10;
	r.right = CardWeidth * nCARD + 30;

	FillRect(cdc, &r, bTitle);

	SelectObject(cdc, fBig);
	wchar_t sum[10];
	_itow_s(BladeGame->GetSumA(), sum, 10);
	TextOut(cdc, CardWeidth * nCARD + 40, 275, sum, lstrlenW(sum));

	_itow_s(BladeGame->GetSumB(), sum, 10);
	TextOut(cdc, CardWeidth * nCARD + 40, 385, sum, lstrlenW(sum));

	BitBlt(hdc, 0, 0, CardWeidth * nCARD + 80, 600, cdc, 0, 0, SRCCOPY);
	DeleteDC(cdc);
	DeleteObject(hbmp);
}

void OnClick(int x, int y)
{
	if (y <= 100)
	{
		BladeGame->ReNew();

		while (WITH_COMPUTER && BladeGame->GetTurn() == PlayerA)
		{
			Player p = BladeGame->RunAI();

			if (p == PlayerA)
			{
				MessageBox(NULL, _TEXT("You lose!"), _TEXT("Blade"), MB_OK);
				BladeGame->ReNew();
			}
			if (p == PlayerB)
			{
				MessageBox(NULL, _TEXT("You win!"), _TEXT("Blade"), MB_OK);
				BladeGame->ReNew();
			}
		}

		InvalidateRect(hWnd, NULL, FALSE);
		return;
	}
	
	if (y < 130)
		return;
	if (x < 10)
		return;

	int modx = (x - 10) % (CardWeidth);
	int mody = (y - 130) % 110;

	if (modx > CardWeidth-10 || mody > 100)
		return;

	x = (x - 10) / (CardWeidth);
	y = (y - 130) / 110;

	if (y == 1 || y == 2)
		return;

	if ((!WITH_COMPUTER) && y == 0)
	{
		if (BladeGame->GetTurn() == PlayerA && (unsigned int)x < BladeGame->GetPlayerA().size())
		{
			Player p = BladeGame->Throw(PlayerA, x);
			if (p == PlayerA)
			{
				MessageBox(NULL, _TEXT("A win"), _TEXT("Blade"), MB_OK);
				BladeGame->ReNew();
			}
			if (p == PlayerB)
			{
				MessageBox(NULL, _TEXT("B win"), _TEXT("Blade"), MB_OK);
				BladeGame->ReNew();
			}
				
			InvalidateRect(hWnd, NULL, FALSE);
			return;
		}
	}

	if (y == 3)
	{
		if (BladeGame->GetTurn() == PlayerB && (unsigned int)x < BladeGame->GetPlayerB().size())
		{
			Player p = BladeGame->Throw(PlayerB, x);

			if (p == PlayerA)
			{
				MessageBox(NULL, _TEXT("You lose!"), _TEXT("Blade"), MB_OK);
				BladeGame->ReNew();
			}
			if (p == PlayerB)
			{
				MessageBox(NULL, _TEXT("You win!"), _TEXT("Blade"), MB_OK);
 				BladeGame->ReNew();
			}

			while (WITH_COMPUTER && BladeGame->GetTurn() == PlayerA)
			{
				Player p = BladeGame->RunAI();

				if (p == PlayerA)
				{
					MessageBox(NULL, _TEXT("You lose!"), _TEXT("Blade"), MB_OK);
					BladeGame->ReNew();
				}
				if (p == PlayerB)
				{
					MessageBox(NULL, _TEXT("You win!"), _TEXT("Blade"), MB_OK);
					BladeGame->ReNew();
				}
			}

			InvalidateRect(hWnd, NULL, FALSE);

			return;
		}
	}
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		OnClick(LOWORD(lParam), HIWORD(lParam));
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}