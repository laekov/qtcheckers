#ifndef BOARD_HH
#define BOARD_HH
#include <cstring>
class Board {
	private:
		int a[10][10];
	public:
		static const int Empty = 0;
		static const int BSoldier = 1;
		static const int BKing = 5;
		static const int WSoldier = 2;
		static const int WKing = 6;
		static const int Obs = 8;
		inline static bool isKing(int x) {
			return x & 3;
		}
		inline static bool isEnemy(int x, int y) {
			return (x & y & 3) == 0;
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
		Board() {
			memset(this->a, 0, sizeof(this->a));
			for (int i = 0; i < 4; ++ i) {
				for (int j = 0; j < 5; ++ j) {
					this->a[i][(i & 1) + j * 2] = WSoldier;
					this->a[9 - i][((i ^ 1) & 1) + j * 2] = BSoldier;
				}
			}
		}
		int get(int x, int y);
};
#endif
