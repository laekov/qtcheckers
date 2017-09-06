#include <QtDebug>
#include <cmath>
#include "board.hh"

template <class T>
inline T abs(const T& x) {
	return x < 0 ? -x : x;
}
template <class T>
inline T max(const T& a, const T& b) {
	return a > b ? a : b;
}

int Board::get(int x, int y) {
	if (x >= 0 && x < 10 && y >= 0 && y < 10) {
		return this->a[x][y];
	} else {
		return -1;
	}
}

bool Board::movable(int ox, int oy, int tx, int ty, int frac, int ctnjmp) {
	if (!Board::inRange(ox, oy)) {
		return 0;
	}
	if (!Board::inRange(tx, ty)) {
		return 0;
	}
	if (tx - ox != ty - oy && tx - ox != oy - ty) {
		return 0;
	}
	if (this->a[tx][ty] != Board::Empty) {
		return 0;
	}
	if (tx == ox && ty == oy) {
		return 0;
	}
	if (isObs(this->a[ox][oy]) || (this->a[ox][oy] & frac) == 0) {
		return 0;
	}
	if (Board::isSoldier(this->a[ox][oy])) {
		int d((frac == 1) ? -1 : 1);
		// qDebug("%d %d %d\n", frac, tx - ox, d);
		if (tx - ox == d && !ctnjmp) {
			return 1;
		} else if (abs(tx - ox) == 2) {
			int mx((tx + ox)>> 1), my((ty + oy)>> 1);
			// qDebug("Checking %d,%d and %d,%d", ox, oy, tx, ty);
			if (isEnemy(this->a[ox][oy], this->a[mx][my])) {
				return 1;
			}
		}
	} else {
		return 0;
	}
	return 0;
}

static const int movx[4] = { 1, 1, -1, -1 };
static const int movy[4] = { 1, -1, 1, -1 };

int Board::lenDFS(int ox, int oy, int tx, int ty, int frac) {
	int res(0);
	// qDebug("Checking %d,%d and %d,%d", ox, oy, tx, ty);
	if (this->movable(ox, oy, tx, ty, frac, 1)) {
		this->a[tx][ty] = this->a[ox][oy];
		this->a[ox][oy] = Board::Empty;
		int mx((tx + ox)>> 1), my((ty + oy)>> 1);
		this->a[mx][my] |= Board::Obs;
		for (int i = 0; i < 4; ++ i) {
			res = max(res, this->lenDFS(tx, ty, tx + movx[i] * 2, ty + movy[i] * 2, frac) + 1);
		}
		this->a[mx][my] &= ~Board::Obs;
		this->a[ox][oy] = this->a[tx][ty];
		this->a[tx][ty] = Board::Empty;
	}
	return res;
}

bool Board::accessible(int ox, int oy, int tx, int ty, int frac, int ctnjmp) {
	if (this->movable(ox, oy, tx, ty, frac, ctnjmp)) {
		if (Board::isSoldier(this->a[ox][oy])) {
		   if (abs(ox - tx) == 2) {
			   int ccl(this->lenDFS(ox, oy, tx, ty, frac)), kl(ccl);
			   for (int i = 0; i < 4; ++ i) {
				   kl = max(kl, this->lenDFS(ox, oy, ox + movx[i] * 2, oy + movy[i] * 2, frac));
			   }
			   return kl == ccl;
		   } else {
			   return 1;
		   }
		}
	}
	return 0;
}

void Board::sync(const char* d) {
	if (d[0] == 'B') {
		for (int i = 1; i <= 100; ++ i) {
			this->a[(i - 1) / 10][(i - 1) % 10] = d[i] - 48;
		}
	}
}

std::string Board::toString() {
	std::string s("B");
	for (int i = 0; i < 10; ++ i) {
		for (int j = 0; j < 10; ++ j) {
			s += (char)this->a[i][j] + 48;
		}
	}
	s += '\n';
	return s;
}

int Board::move(int ox, int oy, int px, int py, int* lkx, int* lky) {
	if (!this->accessible(ox, oy, px, py, this->a[ox][oy] & 3)) {
		return 0;
	}
	this->a[px][py] = this->a[ox][oy];
	this->a[ox][oy] = Board::Empty;
	int res(1);
	if (Board::isSoldier(this->a[px][py])) {
		if (abs(px - ox) == 2) {
			int mx((px + ox)>> 1), my((py + oy)>> 1);
			this->a[mx][my] |= Board::Obs;
			for (int i = 0; i < 4; ++ i) {
				if (this->movable(px, py, px + movx[i] * 2, py + movy[i] * 2, this->a[px][py] & 3, 1)) {
					res |= 2;
					break;
				}
			}
			if (!(res & 2)) {
				for (int i = 0; i < 10; ++ i) {
					for (int j = 0; j < 10; ++ j) {
						if (Board::isObs(this->a[i][j])) {
							this->a[i][j] = Board::Empty;
						}
					}
				}
			} 
		}
	}
	if (res & 2) {
		*lkx = px;
		*lky = py;
	} else {
		*lkx = -1;
		*lky = -1;
	}
	return res;
}

