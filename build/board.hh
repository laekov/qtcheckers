#ifndef BOARD_HH
#define BOARD_HH
#include <cstring>
#include <string>
class Board {
	private:
		int a[10][10];
		int lenDFS(int, int, int, int, int);
		int maxJmp(int);
		int maxJmp(int, int);
	public:
		static const int Empty = 0;
		static const int BSoldier = 1;
		static const int BKing = 5;
		static const int WSoldier = 2;
		static const int WKing = 6;
		static const int King = 4;
		static const int Obs = 8;
		inline static bool isSoldier(int x) {
			return x == 1 || x == 2;
		}
		inline static bool isKing(int x) {
			return !isObs(x) && (x & 4);
		}
		inline static bool isEnemy(int x, int y) {
			return !isObs(x) && !isObs(y) && x && y && (x & y & 3) == 0;
		}
		inline static bool isObs(int x) {
			return x & 8;
		}
		inline static bool isBlack(int x) {
			return x & 1;
		}
		inline static bool isWhite(int x) {
			return x & 2;
		}
		inline static int setObs(int x) {
			return x | Obs;
		}
		inline static int inRange(int x, int y) {
			return x >= 0 && x < 10 && y >= 0 && y < 10;
		}
		inline void setKing(int x, int y) {
			if (Board::inRange(x, y) && Board::isSoldier(this->a[x][y])) {
				this->a[x][y] |= Board::King;
			}
		}
		Board() {
			memset(this->a, 0, sizeof(this->a));
			for (int i = 0; i < 4; ++ i) {
				for (int j = 0; j < 5; ++ j) {
					this->a[i][((i ^ 1) & 1) + j * 2] = WSoldier;
					this->a[9 - i][(i & 1) + j * 2] = BSoldier;
				}
			}
		}
		int get(int, int);
		bool movable(int, int, int, int, int, int = 0);
		bool accessible(int, int, int, int, int, int = 0);
		int move(int, int, int, int, int*, int*);
		void sync(const char*);
		std::string toString();
		int win();
};
#endif

