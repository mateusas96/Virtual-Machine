#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <bitset>

using namespace std;

const char steps[] = { "decryptor.bin" };
const char data_file[] = { "q1_encr.txt" };

struct instructions
{
	char code;
	char cop1;
};

class VM {
private:
	unsigned char *regs;
	char *prog_mem;
	int eof_Flag;
	int iReg1, iReg2;
	instructions *pointer;
	bool jump;
	bool vm_stop;
public:
	int reg_count{ 16 };
	int prog_mem_size{ 256 };

	VM()
	{
		regs = new unsigned char[reg_count];
		prog_mem = new char[prog_mem_size];
	}

	void reg_number()
	{
		pointer++;
		iReg1 = pointer->cop1 & 0x0F;
		iReg2 = pointer->cop1 & 0xF0 >> 4;
	}
	void decoder(const char code, ifstream &data_file)
	{
		switch (code)
		{
		case 0x04: MOVC(); break;
		case 0x05: LSL(); break;
		case 0x07: JMP(); break;
		case 0x0A: JFE(); break;
		case 0x0B: RET(); break;
		case 0x0D: SUB(); break;
		case 0x0E: XOR(); break;
		case 0x0F: OR(); break;
		case 0x10: IN(data_file); break;
		case 0x11: OUT(); break;
		default: break;
		}
	}

	void decryptor(const char steps[])
	{
		ifstream steps_file(steps, ios_base::binary);

		if (!steps_file)
		{
			cout << "Nepavyko atidaryti failo" << endl;
		}
		else
		{
			int i = 0;
			while (steps_file.get(*(prog_mem + i)) && i++ < prog_mem_size);
		}
		steps_file.close();
	}
	void start(const char steps[], const char data_file[])
	{
		decryptor(steps);

		pointer = reinterpret_cast<instructions*>(prog_mem);
		iReg1 = pointer->cop1 & 0x0F;
		iReg2 = pointer->cop1 & 0xF0 >> 4;

		ifstream myfile(data_file);
		if (!myfile)
		{
			cout << "Nepavyko atidaryti duomenu failo" << endl;
		}
		else
		{
			while (vm_stop == false)
			{
				decoder(pointer->code, myfile);
				if (jump == false)
				{
					reg_number();
				}
				jump = false;
			}
		}
	}
	void IN(ifstream &data_file)
	{
		char temp{};

		if (data_file.eof())
		{
			eof_Flag = 1;
		}
		else
		{
			data_file.get(temp);
			*(regs + iReg1) = static_cast<unsigned char>(temp);
			eof_Flag = 0;
		}
	}
	void OUT()
	{
		if (eof_Flag == false)
		{
			bitset<8> cop1Bits(pointer->cop1);
			bitset<8> divider(0b00001111);

			auto xReg = bitset<4>((cop1Bits & divider).to_ulong());

			cout << regs[xReg.to_ulong()];
		}
	}
	void MOVC()
	{
		regs[0] = pointer->cop1;
	}
	void LSL()
	{
		bitset<8> cop1Bits(pointer->cop1);
		bitset<8> divider(0b00001111);

		auto xReg = bitset<4>((cop1Bits & divider).to_ulong());
		regs[xReg.to_ulong()] <<= 1;
	}
	void JMP()
	{
		pointer = (instructions*)((char*)pointer + pointer->cop1);
		iReg1 = pointer->cop1 & 0x0F;
		iReg2 = pointer->cop1 & 0xF0 >> 4;
		jump = true;
	}
	void JFE()
	{
		if (eof_Flag == true)
		{
			JMP();
		}
	}
	void RET()
	{
		vm_stop = true;
	}
	void SUB()
	{
		bitset<8> cop1Bits(pointer->cop1);
		bitset<8> divider(0b00001111);

		auto xReg = bitset<4>((cop1Bits & divider).to_ulong());
		auto yReg = bitset<4>(((cop1Bits >> 4) & divider).to_ulong());

		regs[xReg.to_ulong()] -= regs[yReg.to_ulong()];
	}
	void XOR()
	{
		bitset<8> cop1Bits(pointer->cop1);
		bitset<8> divider(0b00001111);

		auto xReg = bitset<4>((cop1Bits & divider).to_ulong());
		auto yReg = bitset<4>(((cop1Bits >> 4) & divider).to_ulong());

		regs[xReg.to_ulong()] ^= regs[yReg.to_ulong()];
	}
	void OR()
	{
		bitset<8> cop1Bits(pointer->cop1);
		bitset<8> divider(0b00001111);

		auto xReg = bitset<4>((cop1Bits & divider).to_ulong());
		auto yReg = bitset<4>(((cop1Bits >> 4) & divider).to_ulong());

		regs[xReg.to_ulong()] |= regs[yReg.to_ulong()];
	}
};

int main()
{
	VM vm;
	vm.start(steps, data_file);
	cout << "\n";
	return 0;
}