/*
Main code taken from www.hoozi.com
Main code written by Niyaz PK at niyazlife@gmail.com
*/

#ifndef _AES_ENCRYPT_H_
#define _AES_ENCRYPT_H_

class ZEncryptAES
{
private:
	int Nr; // The number of rounds in AES Cipher. It is simply initiated to zero. The actual value is recieved in the program.
	int Nk; // The number of 32 bit words in the key. It is simply initiated to zero. The actual value is recieved in the program.
	unsigned char *in, *out, state[4][4];
	unsigned char RoundKey[240]; // The array that stores the round keys.
	unsigned char Key[32]; // The Key input to the AES Program

    //universal stuff
    inline int getSBoxInvert(int num);
    inline int getSBoxValue(int num);
    inline void KeyExpansion();
    inline void AddRoundKey(int round);

    //decrypt stuff
    inline void InvSubBytes();
    inline void InvShiftRows();
    inline void InvCipher();
    inline void InvMixColumns();
    
    //encrypt stuff
    inline void SubBytes();
    inline void ShiftRows();
    inline void MixColumns();
    inline void Cipher();
    
public:
	ZEncryptAES()
	{
		Nr=0;
		Nk=0;
	}

    //public stuff
    int Init_Key(unsigned char *key, int size);
    void AES_Encrypt(char * input, int in_size, char *output);
    void AES_Decrypt(char * input, int in_size, char *output);
};

#endif
