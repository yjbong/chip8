#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <map>
#include <utility>

#define WIDTH 64
#define HEIGHT 32

using namespace std;

unsigned char ram[4096]; // Chip-8 RAM
unsigned char V[16]; // V 레지스터
unsigned short I; // I 레지스터
unsigned short PC; // Program Counter
unsigned char SP; // Stack Pointer
unsigned short STACK[16]; 
unsigned char DT; // Delay Timer;
unsigned char ST; // Sound Timer;

unsigned char display[WIDTH][HEIGHT];

// Original Chip-8 keypad
// 1 2 3 C
// 4 5 6 D
// 7 8 9 E
// A 0 B F

// keypad mapped to keyboard
// 1 2 3 4
// Q W E R
// A S D F
// Z X C V

int keymap[16];
map <int, int> invKeymap;

void init(HDC hdc) {

	// Init display
	for (int x = 0; x < WIDTH; x++)
		for (int y = 0; y < HEIGHT; y++)
			SetPixel(hdc, x, y, RGB(0, 0, 0));

	// Character sprite
	ram[0 * 5 + 0] = 0xF0, ram[0 * 5 + 1] = 0x90, ram[0 * 5 + 2] = 0x90, ram[0 * 5 + 3] = 0x90, ram[0 * 5 + 4] = 0xF0; // 0
	ram[1 * 5 + 0] = 0x20, ram[1 * 5 + 1] = 0x60, ram[1 * 5 + 2] = 0x20, ram[1 * 5 + 3] = 0x20, ram[1 * 5 + 4] = 0x70; // 1
	ram[2 * 5 + 0] = 0xF0, ram[2 * 5 + 1] = 0x10, ram[2 * 5 + 2] = 0xF0, ram[2 * 5 + 3] = 0x80, ram[2 * 5 + 4] = 0xF0; // 2
	ram[3 * 5 + 0] = 0xF0, ram[3 * 5 + 1] = 0x10, ram[3 * 5 + 2] = 0xF0, ram[3 * 5 + 3] = 0x10, ram[3 * 5 + 4] = 0xF0; // 3
	ram[4 * 5 + 0] = 0x90, ram[4 * 5 + 1] = 0x90, ram[4 * 5 + 2] = 0xF0, ram[4 * 5 + 3] = 0x10, ram[4 * 5 + 4] = 0x10; // 4
	ram[5 * 5 + 0] = 0xF0, ram[5 * 5 + 1] = 0x80, ram[5 * 5 + 2] = 0xF0, ram[5 * 5 + 3] = 0x10, ram[5 * 5 + 4] = 0xF0; // 5
	ram[6 * 5 + 0] = 0xF0, ram[6 * 5 + 1] = 0x80, ram[6 * 5 + 2] = 0xF0, ram[6 * 5 + 3] = 0x90, ram[6 * 5 + 4] = 0xF0; // 6
	ram[7 * 5 + 0] = 0xF0, ram[7 * 5 + 1] = 0x10, ram[7 * 5 + 2] = 0x20, ram[7 * 5 + 3] = 0x40, ram[7 * 5 + 4] = 0x40; // 7
	ram[8 * 5 + 0] = 0xF0, ram[8 * 5 + 1] = 0x90, ram[8 * 5 + 2] = 0xF0, ram[8 * 5 + 3] = 0x90, ram[8 * 5 + 4] = 0xF0; // 8
	ram[9 * 5 + 0] = 0xF0, ram[9 * 5 + 1] = 0x90, ram[9 * 5 + 2] = 0xF0, ram[9 * 5 + 3] = 0x10, ram[9 * 5 + 4] = 0xF0; // 9
	ram[10 * 5 + 0] = 0xF0, ram[10 * 5 + 1] = 0x90, ram[10 * 5 + 2] = 0xF0, ram[10 * 5 + 3] = 0x90, ram[10 * 5 + 4] = 0x90; // A
	ram[11 * 5 + 0] = 0xE0, ram[11 * 5 + 1] = 0x90, ram[11 * 5 + 2] = 0xE0, ram[11 * 5 + 3] = 0x90, ram[11 * 5 + 4] = 0xE0; // B
	ram[12 * 5 + 0] = 0xF0, ram[12 * 5 + 1] = 0x80, ram[12 * 5 + 2] = 0x80, ram[12 * 5 + 3] = 0x80, ram[12 * 5 + 4] = 0xF0; // C
	ram[13 * 5 + 0] = 0xE0, ram[13 * 5 + 1] = 0x90, ram[13 * 5 + 2] = 0x90, ram[13 * 5 + 3] = 0x90, ram[13 * 5 + 4] = 0xE0; // D
	ram[14 * 5 + 0] = 0xF0, ram[14 * 5 + 1] = 0x80, ram[14 * 5 + 2] = 0xF0, ram[14 * 5 + 3] = 0x80, ram[14 * 5 + 4] = 0xF0; // E
	ram[15 * 5 + 0] = 0xF0, ram[15 * 5 + 1] = 0x80, ram[15 * 5 + 2] = 0xF0, ram[15 * 5 + 3] = 0x80, ram[15 * 5 + 4] = 0x80; // F

	// Keymap
	invKeymap.clear(); 	

	keymap[0x0] = 'X'; invKeymap.insert(make_pair<int, int>('X', 0x0));
	keymap[0x1] = '1'; invKeymap.insert(make_pair<int, int>('1', 0x1));
	keymap[0x2] = '2'; invKeymap.insert(make_pair<int, int>('2', 0x2));
	keymap[0x3] = '3'; invKeymap.insert(make_pair<int, int>('3', 0x3));

	keymap[0x4] = 'Q'; invKeymap.insert(make_pair<int, int>('Q', 0x4));
	keymap[0x5] = 'W'; invKeymap.insert(make_pair<int, int>('W', 0x5));
	keymap[0x6] = 'E'; invKeymap.insert(make_pair<int, int>('E', 0x6));
	keymap[0x7] = 'A'; invKeymap.insert(make_pair<int, int>('A', 0x7));

	keymap[0x8] = 'S'; invKeymap.insert(make_pair<int, int>('S', 0x8));
	keymap[0x9] = 'D'; invKeymap.insert(make_pair<int, int>('D', 0x9));
	keymap[0xA] = 'Z'; invKeymap.insert(make_pair<int, int>('Z', 0xA));
	keymap[0xB] = 'C'; invKeymap.insert(make_pair<int, int>('C', 0xB));
	
	keymap[0xC] = '4'; invKeymap.insert(make_pair<int, int>('4', 0xC));
	keymap[0xD] = 'R'; invKeymap.insert(make_pair<int, int>('R', 0xD));
	keymap[0xE] = 'F'; invKeymap.insert(make_pair<int, int>('F', 0xE));
	keymap[0xF] = 'V'; invKeymap.insert(make_pair<int, int>('V', 0xF));

	SP = -1;
	PC = 0x200;
}

// Get nibbles (nibble digit place: 3210, 3 is the most significant nibble.)
// getNibbles(opcode, m, n) = m번째 nibble을 최상위로 하여 n개의 nibble을 읽어옴
unsigned short getNibble(unsigned short opcode, int m, int n) {

	int t = m - n + 1;
	assert(m >= 0 && m <= 3);
	assert(n >= 1 && t >= 0);

	int mask = (1 << (n * 4)) - 1;
	return (opcode & (mask << (t * 4))) >> (t * 4);
}

void load(const char* filename) {
	FILE *fp = fopen(filename, "rb");
	// 파일 사이즈 확인
	fseek(fp, 0, SEEK_END);
	long filesize = ftell(fp);
	printf("filesize = %ld\n", filesize);
	fseek(fp, 0, SEEK_SET);

	// 이진 파일을 1바이트씩 읽어서 ram에 저장
	int idx = 0x200;
	unsigned char ch;
	for (int i = 0; i < filesize; i++) {
		ch = fgetc(fp);
		ram[idx++] = ch;
	}
}

void draw(HDC hdc) {

	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			//SetPixel(hdc, x, y, RGB(127, x, y));
			if (display[x][y]) SetPixel(hdc, x, y, RGB(255, 255, 255));
			else SetPixel(hdc, x, y, RGB(0, 0, 0));
		}
	}

	//getchar();
}

// PC가 특정값으로 쓰여졌다면 false, 그렇지 않으면 true를 리턴하게 한다.
bool decode(unsigned short opcode, HDC hdc) {

	bool ret = true;

	// 00E0 - CLS
	if (opcode == 0x00E0) {
		// Clear the display
		for (int i = 0; i < HEIGHT; i++)
			for (int j = 0; j < WIDTH; j++)
				display[i][j] = 0;
	}
	// 00EE - RET
	if (opcode == 0x00EE) {
		// 스택에 값이 있을 때 SP 값의 범위는 0 이상 16 미만이다.
		if (SP < 16) {
			PC = STACK[SP];
			SP--;
		}
	}
	// 1nnn - JP addr
	if (getNibble(opcode, 3, 1) == 0x1) {
		unsigned short nnn = getNibble(opcode, 2, 3);
		// Jump to location nnn
		PC = nnn;
		ret = false;
	}
	// 2nnn - CALL addr
	if (getNibble(opcode, 3, 1) == 0x2) {
		unsigned short nnn = getNibble(opcode, 2, 3);
		// Call subroutine at nnn
		assert(SP < 16 || SP==0xff);
		STACK[++SP] = PC;
		PC = nnn;
		ret = false;
	}
	// 3xkk - SE Vx, byte
	if (getNibble(opcode, 3, 1) == 0x3) {
		unsigned short kk = getNibble(opcode, 1, 2);
		// The interpreter compares register Vk to kk, and if they are equal, increments the program counter by 2.
		unsigned short x = getNibble(opcode, 2, 1);
		if (V[x] == kk) PC += 2;
	}
	// 4xkk - SNE Vx, byte
	if (getNibble(opcode, 3, 1) == 0x4) {
		unsigned short kk = getNibble(opcode, 1, 2);
		// The interpreter compares register Vk to kk, and if they are not equal, increments the program counter by 2.
		unsigned short x = getNibble(opcode, 2, 1);
		if (V[x] != kk) PC += 2;
	}
	// 5xy0 - SE Vx, Vy
	if (getNibble(opcode, 3, 1) == 0x5 && getNibble(opcode, 0, 1) == 0x0) {
		// The interpreter compares register Vx to Vy, and if they are equal, increments the program counter by 2.
		unsigned short x = getNibble(opcode, 2, 1);
		unsigned short y = getNibble(opcode, 1, 1);
		if (V[x] == V[y]) PC += 2;
	}
	// 6xkk - LD Vx, byte
	if (getNibble(opcode, 3, 1) == 0x6) {
		unsigned short kk = getNibble(opcode, 1, 2);
		// The interpreter puts the value kk into register Vx.
		unsigned short x = getNibble(opcode, 2, 1);
		assert(x != 0xf);
		V[x] = kk;
	}
	// 7xkk - ADD Vx, byte
	if (getNibble(opcode, 3, 1) == 0x7) {
		unsigned short kk = getNibble(opcode, 1, 2);
		// The interpreter puts the value kk into register Vx.
		unsigned short x = getNibble(opcode, 2, 1);
		assert(x != 0xF);
		V[x] += kk;
	}
	if (getNibble(opcode, 3, 1) == 0x8) {
		// 8xy0 - LD Vx, Vy
		if (getNibble(opcode, 0, 1) == 0x0) {
			// Stores the value of register Vy in register Vx.
			unsigned short x = getNibble(opcode, 2, 1);
			unsigned short y = getNibble(opcode, 1, 1);
			V[x] = V[y];
		}
		// 8xy1 - OR Vx, Vy
		if (getNibble(opcode, 0, 1) == 0x1) {
			// Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
			unsigned short x = getNibble(opcode, 2, 1);
			unsigned short y = getNibble(opcode, 1, 1);
			V[x] |= V[y];
		}
		// 8xy2 - AND Vx, Vy
		if (getNibble(opcode, 0, 1) == 0x2) {
			// Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
			unsigned short x = getNibble(opcode, 2, 1);
			unsigned short y = getNibble(opcode, 1, 1);
			V[x] &= V[y];
		}
		// 8xy3 - XOR Vx, Vy
		if (getNibble(opcode, 0, 1) == 0x3) {
			// Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
			unsigned short x = getNibble(opcode, 2, 1);
			unsigned short y = getNibble(opcode, 1, 1);
			V[x] ^= V[y];
		}
		// 8xy4 - ADD Vx, Vy
		if (getNibble(opcode, 0, 1) == 0x4) {
			// The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0.
			// Only the lowest 8 bits of the result are kept, and stored in Vx.
			unsigned short x = getNibble(opcode, 2, 1);
			unsigned short y = getNibble(opcode, 1, 1);
			unsigned long result = (unsigned long)V[x] + V[y];
			V[0xF] = result > 0xff ? 1 : 0;
			V[x] = result & 0xff;
		}
		// 8xy5 - SUB Vx, Vy
		if (getNibble(opcode, 0, 1) == 0x5) {
			// If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
			unsigned short x = getNibble(opcode, 2, 1);
			unsigned short y = getNibble(opcode, 1, 1);
			V[0xF] = V[x] > V[y] ? 1 : 0;
			V[x] -= V[y];
		}
		// 8xy6 - SHR Vx {, Vy}
		if (getNibble(opcode, 0, 1) == 0x6) {
			// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
			unsigned short x = getNibble(opcode, 2, 1);
			unsigned short y = getNibble(opcode, 1, 1);
			V[0xF] = V[x] & 0x1;
			V[x] >>= 1;
		}
		// 8xy7 - SUBN Vx, Vy
		if (getNibble(opcode, 0, 1) == 0x7) {
			// If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
			unsigned short x = getNibble(opcode, 2, 1);
			unsigned short y = getNibble(opcode, 1, 1);
			V[0xf] = V[y] > V[x] ? 1 : 0;
			V[x] = V[y] - V[x];
		}
		// 8xyE - SHL Vx {, Vy}
		if (getNibble(opcode, 0, 1) == 0xE) {
			// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is multiplied by 2.
			unsigned short x = getNibble(opcode, 2, 1);
			unsigned short y = getNibble(opcode, 1, 1);
			V[0xF] = V[x] & 0x80 ? 1 : 0;
			V[x] <<= 1;
		}
	}
	// 9xy0 - SNE Vx, Vy
	if (getNibble(opcode, 3, 1) == 0x9 && getNibble(opcode, 0, 1) == 0x0) {
		// The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
		unsigned short x = getNibble(opcode, 2, 1);
		unsigned short y = getNibble(opcode, 1, 1);
		if (V[x] != V[y]) PC += 2;
	}
	// Annn - LD I, addr
	if (getNibble(opcode, 3, 1) == 0xA) {
		// The value of register I is set to nnn.
		unsigned short nnn = getNibble(opcode, 2, 3);
		I = nnn;
	}
	// Bnnn - JP V0, addr
	if (getNibble(opcode, 3, 1) == 0xB) {
		// The program counter is set to nnn plus the value of V0.
		unsigned short nnn = getNibble(opcode, 2, 3);
		PC = nnn + V[0];
		ret = false;
	}
	// Cxkk - RND Vx, byte
	if (getNibble(opcode, 3, 1) == 0xC) {

		unsigned short kk = getNibble(opcode, 1, 2);
		// The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk.
		// The result are stored in Vx.
		unsigned short x = getNibble(opcode, 2, 1);
		assert(x != 0xF);
		V[x] == ((rand()) & 0xff) & kk;
	}
	// Dxyn - DRW Vx, Vy, nibble
	if (getNibble(opcode, 3, 1) == 0xD) {
		unsigned short x = getNibble(opcode, 2, 1);
		unsigned short y = getNibble(opcode, 1, 1);
		unsigned short n = getNibble(opcode, 0, 1);

		// The interpreter reads n bytes from memory, starting at the address stored in I.
		// These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
		// Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
		// If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen.
		V[0xF] = 0;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < 8; j++) {
				unsigned char bit = (ram[I + i] & (1 << j)) >> j;
				unsigned char oldPixel = display[(V[x] + j) % WIDTH][(V[y] + i) % HEIGHT];
				unsigned char newPixel = oldPixel ^ bit;
				if (oldPixel && (!newPixel)) V[0xF] = 1;
				display[(V[x] + j) % WIDTH][(V[y] + i) % HEIGHT] = newPixel;

				if (oldPixel != newPixel) {
					if (newPixel) SetPixel(hdc, (V[x] + j) % WIDTH, (V[y] + i) % HEIGHT, RGB(255, 255, 255));
					else SetPixel(hdc, (V[x] + j) % WIDTH, (V[y] + i) % HEIGHT, RGB(0, 0, 0));
				}
			}
		}
	}
	if (getNibble(opcode, 3, 1) == 0xE) {
		// Ex9E - SKP Vx
		if (getNibble(opcode, 1, 2) == 0x9E) {
			unsigned short x = getNibble(opcode, 2, 1);
			assert(V[x] <= 0xF);
			// Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
			if (GetAsyncKeyState(keymap[V[x]]) & 0x8000) PC += 2;
		}
		// ExA1 - SKNP Vx
		if (getNibble(opcode, 1, 2) == 0x9E) {
			unsigned short x = getNibble(opcode, 2, 1);
			assert(V[x] <= 0xF);
			// Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
			if (!(GetAsyncKeyState(keymap[V[x]]) & 0x8000)) PC += 2;
		}
	}
	if (getNibble(opcode, 3, 1) == 0xF) {
		// Fx07 - LD Vx, DT
		if (getNibble(opcode, 1, 2) == 0x07) {
			unsigned short x = getNibble(opcode, 2, 1);
			assert(x != 0xF);
			// The value of DT is placed into Vx.
			V[x] = DT;
		}
		// Fx0A - LD Vx, K
		if (getNibble(opcode, 1, 2) == 0x0A) {
			unsigned short x = getNibble(opcode, 2, 1);
			assert(x != 0xF);
			// All execution stops until a key is pressed, then the value of that key is stored in Vx.
			while (1) {
				unsigned char ch = getchar();
				map<int, int>::iterator it;
				it = invKeymap.find(ch);
				if (it != invKeymap.end()) {
					V[x] = it->second;
					break;
				}
			}
		}
		// Fx15 - LD DT, Vx
		if (getNibble(opcode, 1, 2) == 0x15) {
			unsigned short x = getNibble(opcode, 2, 1);
			// DT is set equal to the value of Vx.
			DT = V[x];
		}
		// Fx18 - LD ST, Vx
		if (getNibble(opcode, 1, 2) == 0x18) {
			unsigned short x = getNibble(opcode, 2, 1);
			// ST is set equal to the value of Vx.
			ST = V[x];
		}
		// Fx1E - ADD I, Vx
		if (getNibble(opcode, 1, 2) == 0x1E) {
			unsigned short x = getNibble(opcode, 2, 1);
			// The values of I and Vx are added, and the results are stored in I.
			I += V[x];
		}
		// Fx29 - LD F, Vx
		if (getNibble(opcode, 1, 2) == 0x29) {
			unsigned short x = getNibble(opcode, 2, 1);
			assert(V[x] <= 0xF);
			// The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx.
			I = V[x] * 5;
		}
		// Fx33 - LD B, Vx
		if (getNibble(opcode, 1, 2) == 0x33) {
			unsigned short x = getNibble(opcode, 2, 1);
			// The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I,
			// the tens digit at location I+1, and the ones digit at location I+2.
			unsigned char val = V[x];
			ram[I + 2] = val % 10; val /= 10;
			ram[I + 1] = val % 10; val /= 10;
			ram[I] = val;
		}
		// Fx55 - LD [I], Vx
		if (getNibble(opcode, 1, 2) == 0x55) {
			unsigned short x = getNibble(opcode, 2, 1);
			// The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
			for (int i = 0; i <= x; i++) ram[I + i] = V[i];
		}
		// Fx65 - LD Vx, [I]
		if (getNibble(opcode, 1, 2) == 0x65) {
			unsigned short x = getNibble(opcode, 2, 1);
			assert(x != 0xF);
			// The interpreter reads values from memory starting at location I into registers V0 through Vx.
			for (int i = 0; i <= x; i++) V[i] = ram[I + i];
		}
	}

	return ret;
}

int main(int argc, char *argv[]) {

	HWND hwnd = GetConsoleWindow();
	if (hwnd != NULL) { SetWindowPos(hwnd, 0, 0, 0, 1000, 300, SWP_SHOWWINDOW | SWP_NOMOVE); }
	//HDC hdc = GetDC(GetConsoleWindow());
	HDC hdc = GetDC(hwnd);

	init(hdc);
	load(argv[1]);
	//load("BLINKY");

	while (1) {
		if (DT > 0) DT--;
		if (ST > 0) ST--;
		unsigned short opcode = (ram[PC] << 8) | ram[PC + 1];
		//printf("\t\topcode[0x%03x] = %04x\n", PC, opcode);
		if (decode(opcode, hdc)) PC += 2;
		//draw(hdc);
		//Sleep(1000);
	}
	return 0;
}