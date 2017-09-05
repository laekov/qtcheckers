#include <QtDebug>
#include <cmath>
#include "board.hh"

int Board::get(int x, int y) {
	if (x >= 0 && x < 10 && y >= 0 && y < 10) {
		return this->a[x][y];
	} else {
		return -1;
	}
}

bool Board::accessible(int ox, int oy, int tx, int ty, int frac, int ctnjmp) {
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
		} else if ((int)fabs(tx - ox) == 2) {
			int mx((tx + ox)>> 1), my((ty + oy)>> 1);
			if (isEnemy(this->a[ox][oy], this->a[mx][my])) {
				return 1;
			}
		}
	} else {
		return 0;
	}
	return 0;
}

