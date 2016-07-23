#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>
#include <tchar.h>
#include "SerialClass.h"	// Library described above
#include <string>
#include <csignal>
#include <conio.h>
using namespace std;
fstream ECGout, PPGout, dataOut;
int isRun = 0;
unsigned long int totalBytesWritten = 0;

void closeAll(){
	dataOut.close();
	ECGout.close();
	PPGout.close();
}
void signalHandler(int signum)
{	
	isRun = 0;
	signal(SIGINT, signalHandler);
}
int signedConvert(unsigned char high, unsigned char low){
	//converts the 2 bytes of a signed 10bit number with the sign in the first bit
	int temp = 0;//used for conversion do not change 
	temp = temp | (high & 0x03);
	temp = temp << 8;
	temp = temp | low;
	if (temp>511){ temp = temp - 1024; }
	return temp;

}

int unsignedConvert(unsigned char high, unsigned low){
	//converts the 2 bytes of a signed 10bit number with the sign in the first bit
	int temp = 0;//used for conversion do not change 
	temp = temp | (high & 0x03);
	temp = temp << 8;
	temp = temp | low;
	temp = temp & 0x03FF;
	return temp;
}
void assembleData(){
	int efficiency = 0;
	unsigned char buffer[6];
	char temp = 'q';
	unsigned long int  counter = 0;
	dataOut.close();
	dataOut.clear();
	cout << "Preparing to format the incoming data to desired output ( ";
	cout << totalBytesWritten << " Bytes) \n\n";
	dataOut.open("dataOut.txt", ios::in|ios::binary);
	dataOut.seekg(0);
	
	while (temp!='\n'){
		dataOut.read(&temp, 1);//readonly 1 byte 
	}
	//now we are aligned to the first '\n' character
	//now we read in 6 bytes at a time and check th elast byte to be \n
	while (!dataOut.eof()){
		counter++;
  		dataOut.read((char*)buffer, 6);//read 6 bytes at a time hoping that the 6th will always be \n
		if (buffer[5] == '\n'){
			efficiency++;
			//valid chunk of data use it
			int ECGval = signedConvert(buffer[0], buffer[1]);
			int PPGval = unsignedConvert(buffer[3], buffer[4]);
			char ECGvalBuf[8] = "";
			char PPGvalBuf[8] = "";
			_itoa_s(ECGval, ECGvalBuf, 10);
			_itoa_s(PPGval, PPGvalBuf, 10);
			ECGout << ECGvalBuf << '\n';
			PPGout << PPGvalBuf << '\n';
		}
		else{
			temp = 'q';
			//re-Sync
			while (temp != '\n'){
				dataOut.read(&temp, 1);//readonly 1 byte 
			}
		}

	}
	cout << "\n\nall Done with Efficiency " << ((float)efficiency / counter)*100<<"\n\n";
	system("pause");
}
// application reads from the specified serial port and reports the collected data
int main(int argc, _TCHAR* argv[])
{
	printf("Welcome to the ECG_PPG-DataLogger app!\n\n");
	char* FileName = "ECG.txt";
	signal(SIGINT, signalHandler);
	ECGout.open("ECG.txt",ios::out);
	PPGout.open("PPG.txt",  ios::out);
	dataOut.open("dataOut.txt",  ios::out|ios::binary);
	cout << "Enter the COM port from which the data is to saved  ";
	string input,name="\\\\.\\";
	cin >> input;
	name += input;
	//Serial* SP = new Serial("\\\\.\\COM7"); // adjust as needed
	char portname[20];
	strcpy_s(portname,name.c_str());
	portname[name.length()] = '\0';
	cout << portname <<" Starting ... ";
	Serial* SP = new Serial(portname);    // adjust as needed

	if (SP->IsConnected())
		printf("We're connected\n");

	char incomingData[201] = "";			// don't forget to pre-allocate memory
	//printf("%s\n",incomingData);
	int dataLength = 200;
	int readResult = 0;
	
	while (SP->IsConnected()){
		if ((SP->IsConnected())){
			while (isRun){
				readResult = SP->ReadData(incomingData, dataLength);
				if (readResult != 0){
					totalBytesWritten += readResult;
					incomingData[readResult] = 0;
					for (int i = 0; i < readResult; i++){
						dataOut.write(&(incomingData[i]), 1);//wrtie 1 byte only
					}
				//	printf("\t\tBytes read:  %i\n", readResult);
					}
			}
		}
		if (isRun == 0){
			cout << "\npress 'S' to start  'P' to PAUSE and 'E' to exit\n\n >>";
			char input = 'I';
			while (input != 'S' && input != 'P' && input != 'E'){
				input = getchar();
			}
			switch (input){
			case 'S':
				isRun = 1;
				cout << "Press Ctrl+C to pause the Acquisition ";
				break;

			case 'P':
				isRun = 0;
				break;
			case 'E':
				assembleData();
				closeAll();
				exit(0);
				break;


			}
		}
	}
	system("pause");
	return 0;
}