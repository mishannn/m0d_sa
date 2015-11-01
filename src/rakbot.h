struct stRakBotSettings
{
	bool bIsActive;
	bool iIsConnected;
	char szNickName[32];
	unsigned short usID;
	unsigned int uiChallenge;
	class RakClientInterface *pRakClient;
};

class CRakBot
{
public:
	static void Thread(void *);
	static void Connect(char *szNickName, char *szPassword);
	static void Disconnect(unsigned short botID);
private:
	static void UpdateNetwork(unsigned short botID);
};

extern unsigned short botCount;
extern struct stRakBotSettings botSettings[SAMP_MAX_PLAYERS];