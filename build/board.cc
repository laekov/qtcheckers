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

bool Board::movable(int ox, int oy, int tx, int ty, int fac, int ctnjmp) {
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
	if (isObs(this->a[ox][oy]) || (this->a[ox][oy] & fac) == 0) {
		return 0;
	}
	if (Board::isSoldier(this->a[ox][oy])) {
		int d((fac == 1) ? -1 : 1);
		// qDebug("%d %d %d\n", fac, tx - ox, d);
		if (tx - ox == d && !ctnjmp) {
			return 1;
		} else if (abs(tx - ox) == 2) {
			int mx((tx + ox)>> 1), my((ty + oy)>> 1);
			// qDebug("Checking %d,%d and %d,%d", ox, oy, tx, ty);
			if (isEnemy(this->a[ox][oy], this->a[mx][my])) {
				return 1;
			}
		}
	} else if (Board::isKing(this->a[ox][oy])) {
		int dx(abs(tx - ox) / (tx - ox)), dy(abs(ty - oy) / (ty - oy));
		int cenm(0);
		for (int cx = ox + dx, cy = oy + dy; cx != tx; cx += dx, cy += dy) {
			if (Board::isObs(this->a[cx][cy]) || (this->a[cx][cy] & fac & 3)) {
				return 0;
			}
			if (Board::isEnemy(this->a[cx][cy], fac)) {
				++ cenm;
			}
		}
		return (cenm == 0 && !ctnjmp) || cenm == 1;
	} else {
		return 0;
	}
	return 0;
}

static const int movx[4] = { 1, 1, -1, -1 };
static const int movy[4] = { 1, -1, 1, -1 };

int Board::lenDFS(int ox, int oy, int tx, int ty, int fac) {
	int res(0);
	// qDebug("Checking %d,%d and %d,%d", ox, oy, tx, ty);
	if (this->movable(ox, oy, tx, ty, fac, 1)) {
		res = 1;
		this->a[tx][ty] = this->a[ox][oy];
		this->a[ox][oy] = Board::Empty;
		for (int i = 0; i < 4; ++ i) {
			if (Board::isSoldier(this->a[ox][oy])) {
				int mx((tx + ox)>> 1), my((ty + oy)>> 1);
				this->a[mx][my] |= Board::Obs;
				res = max(res, this->lenDFS(tx, ty, tx + movx[i] * 2, ty + movy[i] * 2, fac) + 1);
				this->a[mx][my] &= ~Board::Obs;
			} else if (Board::isKing(this->a[ox][oy])) {
				int cenn(0);
				int dx(abs(tx - ox) / (tx - ox)), dy(abs(ty - oy) / (ty - oy));
				for (int cx = ox + dx, cy = oy + dy; cx != tx; cx += dx, cy += dy) {
					if (Board::isEnemy(this->a[cx][cy], fac)) {
						this->a[cx][cy] |= Board::Obs;
					}
				}
				for (int cx = tx + movx[i], cy = ty + movy[i]; Board::inRange(cx, cy); cx += movx[i], cy += movy[i]) {
					if (cenn) {
						res = max(res, this->lenDFS(tx, ty, cx, cy, fac) + 1);
					}
					if (isEnemy(this->a[tx][ty], this->a[cx][cy])) {
						if (cenn) {
							break;
						}
						cenn = abs(cx - tx);
					} else if (isObs(this->a[cx][cy])) {
						break;
					}
				}
				for (int cx = ox + dx, cy = oy + dy; cx != tx; cx += dx, cy += dy) {
					if (Board::isEnemy(this->a[cx][cy], fac)) {
						this->a[cx][cy] &= ~Board::Obs;
					}
				}
			}
		}
		this->a[ox][oy] = this->a[tx][ty];
		this->a[tx][ty] = Board::Empty;
	}
	return res;
}

int Board::maxJmp(int ox, int oy) {
	int fac(this->a[ox][oy] & 3);
	int res(0);
	if (Board::isSoldier(this->a[ox][oy])) {
		for (int i = 0; i < 4; ++ i) {
			res = max(res, this->lenDFS(ox, oy, ox + movx[i] * 2, oy + movy[i] * 2, fac));
		}
	} else if (Board::isKing(this->a[ox][oy])) {
		for (int i = 0; i < 4; ++ i) {
			int cenn(0);
			for (int cx = ox + movx[i], cy = oy + movy[i]; Board::inRange(cx, cy); cx += movx[i], cy += movy[i]) {
				if (cenn) {
					res = max(res, this->lenDFS(ox, oy, cx, cy, fac));
				}
				if (isEnemy(this->a[ox][oy], this->a[cx][cy])) {
					if (cenn) {
						break;
					}
					cenn = 1;
				} else if (isObs(this->a[cx][cy])) {
					break;
				}
			}
		}
	}
	return res;

}
int Board::maxJmp(int fac) {
	int s(0);
	for (int i = 0; i < 10; ++ i) {
		for (int j = 0; j < 10; ++ j) {
			if (!Board::isObs(this->a[i][j]) && (this->a[i][j] & fac)) {
				s = max(s, this->maxJmp(i, j));
			}
		}
	}
	return s;
}

bool Board::accessible(int ox, int oy, int tx, int ty, int fac, int ctnjmp) {
	if (this->movable(ox, oy, tx, ty, fac, ctnjmp)) {
		int kl(this->maxJmp(this->a[ox][oy] & 3));
		if (Board::isSoldier(this->a[ox][oy])) {
			if (abs(ox - tx) == 2) {
				int ccl(this->lenDFS(ox, oy, tx, ty, fac));
				return kl == ccl;
			} else {
				return kl == 0;
			}
		} else if (Board::isKing(this->a[ox][oy])) {
			int dx(abs(tx - ox) / (tx - ox)), dy(abs(ty - oy) / (ty - oy));
			int cenm(0);
			for (int cx = ox + dx, cy = oy + dy; cx != tx; cx += dx, cy += dy) {
				if (Board::isEnemy(this->a[cx][cy], fac)) {
					++ cenm;
				}
			}
			if (cenm == 0) {
				return kl == 0;
			} else {
				return this->lenDFS(ox, oy, tx, ty, fac) == kl;
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
	int res(1), ne(1);
	this->a[px][py] = this->a[ox][oy];
	this->a[ox][oy] = Board::Empty;
	if (Board::isSoldier(this->a[px][py])) {
		if (abs(px - ox) == 2) {
			int mx((px + ox)>> 1), my((py + oy)>> 1);
			this->a[mx][my] |= Board::Obs;
			ne = 0;
		}
	} else if (Board::isKing(this->a[px][py])) {
		int dx(abs(px - ox) / (px - ox)), dy(abs(py - oy) / (py - oy));
		for (int cx = ox + dx, cy = oy + dy; cx != px; cx += dx, cy += dy) {
			if (isEnemy(this->a[px][py], this->a[cx][cy])) {
				ne = 0;
				this->a[cx][cy] |= Board::Obs;
			}
		}
	}
	if (this->maxJmp(px, py) && !ne) {
		*lkx = px;
		*lky = py;
		res |= 2;
	} else {
		for (int i = 0; i < 10; ++ i) {
			for (int j = 0; j < 10; ++ j) {
				if (Board::isObs(this->a[i][j])) {
					this->a[i][j] = Board::Empty;
				}
			}
		}
		int bkx(isWhite(this->a[px][py]) ? 9 : 0);
		if (px == bkx) {
			this->a[px][py] |= Board::King;
		}
		*lkx = -1;
		*lky = -1;
	}
	return res;
}

int Board::win() {
	int wcnt(0), wmv(0), bcnt(0), bmv(0);
	for (int i = 0; i < 10; ++ i) {
		for (int j = 0; j < 10; ++ j) {
			if (this->a[i][j] && !Board::isObs(this->a[i][j])) {
				int *cnt, *mv;
				if (Board::isWhite(this->a[i][j])) {
					cnt = &wcnt, mv = &wmv;
				} else {
					cnt = &bcnt, mv = &bmv;
				}
				++ *cnt;
				if (Board::isKing(this->a[i][j])) {
					for (int mi = 0; mi < 4; ++ mi) {
						for (int cx = i, cy = j; Board::inRange(cx, cy) && !*mv; cx += movx[mi], cy += movy[mi]) {
							if (this->movable(i, j, cx, cy, this->a[i][j] & 3)) {
								*mv = 1;
							}
						}
					}
				} else if (Board::isSoldier(this->a[i][j])) {
					for (int mi = 0; mi < 4; ++ mi) {
						for (int k = 1; k <= 2; ++ k) {
							if (this->movable(i, j, i + k * movx[mi], j + k * movy[mi], this->a[i][j] & 3)) {
								*mv = 1;
							}
						}
					}
				}
			}
		}
	}
	if (!bcnt || !bmv) {
		return Board::WSoldier;
	}
	if (!wcnt || !wmv) {
		return Board::BSoldier;
	}
	return 0;
}

