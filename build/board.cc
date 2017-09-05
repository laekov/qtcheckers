#include "board.hh"

int Board::get(int x, int y) {
	if (x >= 0 && x < 10 && y >= 0 && y < 10) {
		return this->a[x][y];
	} else {
		return -1;
	}
}
