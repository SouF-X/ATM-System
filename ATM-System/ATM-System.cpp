// ATM System

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>

using namespace std;

struct sClient
{
	string AccountNumber;
	string Name;
	string Phone;
	string PinCode;
	double Balance;
	bool MarkForDelete = false;
};


const string ClientsFileName = "Clients.txt";

enum enATmMainMenueOptions {eQuickWithdraw = 1, eNormalWithdraw = 2, eDeposit = 3, eCheckBalance = 4, eLogout = 5};

void ShowATmMainMenue();
void Login();
void ShowQuickWithdrawScreen();
void ShowNormalWithdrawScreen();
void ShowDepositScreen();

sClient CurrentClient;


// Start Clients Common Functions // 

vector <string> Split(string S1, string Delim)
{
	vector <string> vString;
	short pos = 0;
	string sWord;

	while ((pos = S1.find(Delim)) != std::string::npos)
	{
		sWord = S1.substr(0, pos);
		if (sWord != "")
		{
			vString.push_back(sWord);
		}

		S1.erase(0, pos + Delim.length());
	}

	if (S1 != "")
	{
		vString.push_back(S1);
	}

	return vString;
}

sClient ConvertLineDataToRecord(string Line, string Seperator = "#//#")
{
	sClient ClientData;
	vector <string> vString = Split(Line, Seperator);


	ClientData.AccountNumber = vString[0];
	ClientData.PinCode = vString[1];
	ClientData.Name = vString[2];
	ClientData.Phone = vString[3];
	ClientData.Balance = stod(vString[4]);

	return ClientData;
}

vector <sClient> LoadClientDataFromFile(string FileName)
{
	vector <sClient> vClients;

	fstream MyFile;
	MyFile.open(FileName, ios::in);

	if (MyFile.is_open())
	{
		string Line;
		sClient Client;

		while (getline(MyFile, Line))
		{
			Client = ConvertLineDataToRecord(Line);
			vClients.push_back(Client);
		}

		MyFile.close();

	}

	return vClients;
}

string ConvertRecordToLine(sClient Client, string Seperator = "#//#")
{
	string stClientRecord = "";

	stClientRecord += Client.AccountNumber + Seperator;
	stClientRecord += Client.PinCode + Seperator;
	stClientRecord += Client.Name + Seperator;
	stClientRecord += Client.Phone + Seperator;
	stClientRecord += to_string(Client.Balance) + Seperator;

	return stClientRecord;
}

bool FindClientByAccountNumber(string AccountNumber, vector <sClient> vClients, sClient& Client)
{
	for (sClient C : vClients)
	{
		if (C.AccountNumber == AccountNumber)
		{
			Client = C;
			return true;
		}
	}

	return false;
}

vector <sClient> SaveClientDataToFile(string FileName, vector <sClient> vClients)
{
	fstream MyFile;
	MyFile.open(FileName, ios::out);

	string DataLine;

	if (MyFile.is_open())
	{
		for (sClient C : vClients)
		{
			if (C.MarkForDelete == false)
			{
				DataLine = ConvertRecordToLine(C);
				MyFile << DataLine << endl;
			}
		}

		MyFile.close();
	}

	return vClients;
}

bool DepositBalanceToClientByAccountNumber(string AccountNumber, double Amount, vector <sClient>& vClients)
{
	char Answer = 'n';
	cout << "\n\nAre you sure you want to perform this transaction? y/n ? ";
	cin >> Answer;

	if (Answer == 'y' || Answer == 'Y')
	{
		for (sClient& C : vClients)
		{
			if (C.AccountNumber == AccountNumber)
			{
				C.Balance += Amount;
				SaveClientDataToFile(ClientsFileName, vClients);
				cout << "\n\nDone Successfully. New balance is: " << C.Balance;
				return true;
			}
		}

		return false;
	}
}

// End Clients Common Functions //

// -------------------------------------------------------------------------------------------------------------------

// Start Check Balance

void ShowCheckBalanceScreen()
{
	cout << "===========================================\n";
	cout << "\t\tCheck Balance Screen\n";
	cout << "===========================================\n";
	cout << "Your Balance is " << CurrentClient.Balance;
}

// End Check Balance

// -------------------------------------------------------------------------------------------------------------------
 
// Start Quick Withdraw

enum enQuickWithdrawOptions {
	Twenty = 1, Fifty = 2, OneHundred = 3,
	TwoHundred = 4, FourHundred = 5, SixHundred = 6,
	EightHundred = 7, OneThousand = 8, Exit = 9
};

short getQuickWithDrawAmount(short QuickWithDrawOption)
{
	switch (QuickWithDrawOption)
	{
	case 1:
		return 20;
	case 2:
		return 50;
	case 3:
		return 100;
	case 4:
		return 200;
	case 5:
		return 400;
	case 6:
		return 600;
	case 7:
		return 800;
	case 8:
		return 1000;
	default:
		return 0;
	}
}

short ReadQuickWithdrawOption()
{
	short Choice = 0;

	while (Choice < 1 || Choice > 9)
	{
		cout << "\nChoose what to do from [1 to 9] ? ";
		cin >> Choice;
	}

	return Choice;
}

void PerformQuickWithdrawMenueOption(short QuickWithDrawOption)
{
	if (QuickWithDrawOption == 9)
		return;

	short WithDrawBalance = getQuickWithDrawAmount(QuickWithDrawOption);

	if (WithDrawBalance > CurrentClient.Balance)
	{
		cout << "\nThe amount exceeds your balance, make another choice.\n";
		cout << "Press Anykey to continue...";
		system("pause>0");
		ShowQuickWithdrawScreen();
		return;
	}

	vector <sClient> vClients = LoadClientDataFromFile(ClientsFileName);
	DepositBalanceToClientByAccountNumber(CurrentClient.AccountNumber, WithDrawBalance * -1, vClients);
	CurrentClient.Balance -= WithDrawBalance;

}

void ShowQuickWithdrawScreen()
{
	system("cls");
	cout << "===========================================\n";
	cout << "\t\tQuick Withdraw\n";
	cout << "===========================================\n";
	cout << "\t[1] 20" << "\t\t[2] 50";
	cout << "\n\t[3] 100" << "\t\t[4] 200";
	cout << "\n\t[5] 400" << "\t\t[6] 600";
	cout << "\n\t[7] 800" << "\t\t[8] 1000";
	cout << "\n\t[9] Exit";
	cout << "\n===========================================\n";
	cout << "Your balance is " << CurrentClient.Balance;

	PerformQuickWithdrawMenueOption(ReadQuickWithdrawOption());
}

// End Quick Withdraw

// -------------------------------------------------------------------------------------------------------------------

// Start Normal Withdraw

int ReadWithDrawAmount()
{
	int AmountNumber = 0;

	do
	{
		cout << "Enter an amount multiple of 5's ? ";
		cin >> AmountNumber;

	} while (AmountNumber % 5 != 0);

	return AmountNumber;
}

void PerformNormalWithdrawOption()
{
	int WithDrawBalance = ReadWithDrawAmount();

	if (WithDrawBalance > CurrentClient.Balance)
	{
		cout << "\nThe amount exceeds your balance, make another choice.\n";
		cout << "Press Anykey to continue...";
		system("pause>0");
		ShowNormalWithdrawScreen();
		return;
	}

	vector <sClient> vClients = LoadClientDataFromFile(ClientsFileName);
	DepositBalanceToClientByAccountNumber(CurrentClient.AccountNumber, WithDrawBalance * -1, vClients);
	CurrentClient.Balance -= WithDrawBalance;
}

void ShowNormalWithdrawScreen()
{
	system("cls");
	cout << "===========================================\n";
	cout << "\t\tNormal Withdraw Screen\n";
	cout << "===========================================\n";
	
	PerformNormalWithdrawOption();
}

// End Normal Withdraw

// -------------------------------------------------------------------------------------------------------------------

// Start Deposit

double ReadDepositAmount()
{
	double Amount = 0;

	do
	{
		cout << "Enter a positive Deposit Amount? ";
		cin >> Amount;

	} while (Amount <= 0);

	return Amount;
}

void PerformDepositOption()
{
	int DepositAmount = ReadDepositAmount();

	vector <sClient> vClients = LoadClientDataFromFile(ClientsFileName);
	DepositBalanceToClientByAccountNumber(CurrentClient.AccountNumber, DepositAmount, vClients);
	CurrentClient.Balance += DepositAmount;
}

void ShowDepositScreen()
{
	cout << "===========================================\n";
	cout << "\t\tDeposit Screen\n";
	cout << "===========================================\n";

	PerformDepositOption();
}

// End Depostit

// -------------------------------------------------------------------------------------------------------------------

// Start ATM Menue

void GoBackToATmMainMenue()
{
	cout << "\n\nPress any key to go bak to Main Menue...";
 	system("pause>0");
	ShowATmMainMenue();
}

short ReadATmMainMenueOption()
{
	short Choice = 0;

	do
	{
		cout << "Choose what do you want to do? [1 to 5] ? ";
		cin >> Choice;
	} while (Choice > 5);

	return Choice;
}

void PerformATmMainMenueOption(enATmMainMenueOptions ATmMenueOptions)
{
	switch (ATmMenueOptions)
	{
	case enATmMainMenueOptions::eQuickWithdraw:
	{
		system("cls");
		ShowQuickWithdrawScreen();
		GoBackToATmMainMenue();
		break;
	}
	case enATmMainMenueOptions::eNormalWithdraw:
	{
		system("cls");
		ShowNormalWithdrawScreen();
		GoBackToATmMainMenue();
		break;
	}
	case enATmMainMenueOptions::eDeposit:
	{
		system("cls");
		ShowDepositScreen();
		GoBackToATmMainMenue();
		break;
	}
	case enATmMainMenueOptions::eCheckBalance:
	{
		system("cls");
		ShowCheckBalanceScreen();
		GoBackToATmMainMenue();
		break;
	}
	case enATmMainMenueOptions::eLogout:
	{
		system("cls");
		Login();
	}
	}
}

void ShowATmMainMenue()
{
	system("cls");
	cout << "===========================================\n";
	cout << "\t\tATM Main Menue Screen\n";
	cout << "===========================================\n";
	cout << "\t[1] Quick Withdraw.\n";
	cout << "\t[2] Normal Withdraw.\n";
	cout << "\t[3] Deposit.\n";
	cout << "\t[4] Check Balance.\n";
	cout << "\t[5] Logout.\n";
	cout << "===========================================\n";

	PerformATmMainMenueOption((enATmMainMenueOptions)ReadATmMainMenueOption());

}

// End ATM Menue

// -------------------------------------------------------------------------------------------------------------------

// Start Login

bool FindClientByAccountNumberAndPinCode(string AccountNumber, string PinCode, sClient& CurrentClient)
{
	vector <sClient> vClients = LoadClientDataFromFile(ClientsFileName);

	for (sClient& C : vClients)
	{
		if (C.AccountNumber == AccountNumber && C.PinCode == PinCode)
		{
			CurrentClient = C;
			return true;
		}
	}

	return false;
}

void Login()
{
	bool LoginFaild = false;

	string AccountNumber, PinCode;

	do
	{
		system("cls");

		cout << "\n---------------------------------\n";
		cout << "\tLogin Screen";
		cout << "\n---------------------------------\n";

		if (LoginFaild)
		{
			cout << "Invalid AccountNumber/PinCode!\n";
		}

		cout << "Enter AccountNumber? ";
		cin >> AccountNumber;

		cout << "Enter PinCode? ";
		cin >> PinCode;

		LoginFaild = !FindClientByAccountNumberAndPinCode(AccountNumber, PinCode, CurrentClient);

	} while (LoginFaild);

	ShowATmMainMenue();
}

// End Login

// -------------------------------------------------------------------------------------------------------------------

int main()
{
	Login();

	return 0;
}