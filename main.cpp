#include <4dm.h>
#include <openssl/evp.h>
using namespace fdm;


void updateGame( StateMultiplayer* mp, std::string uuidstr, std::string uuidlbl, std::string uuid_hidden ) {
	
	mp->uuidInput.text = uuidstr;  // apply uuid to game logic
	
	mp->uuidLabel.text = uuidlbl;
	mp->uuidInput.passwordVisualText = uuid_hidden;
}
void updateUUID() {
	StateMultiplayer* mp = StateMultiplayer::instanceObj;
	
	static std::string trueUUID = mp->uuidInput.text;
	static std::string oldAddress = "ä";
	static std::string uuidstr = "";
	static std::string uuidlbl = "";
	static std::string uuid_hidden = "";
	
	if (oldAddress == mp->serverAddressInput.text)
		return updateGame(mp,uuidstr,uuidlbl,uuid_hidden);
	oldAddress = mp->serverAddressInput.text;
	
	unsigned char hashBytes[EVP_MAX_MD_SIZE];
	
	EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
	EVP_DigestInit(mdctx, EVP_sha256());  // set hashing function to sha256
	EVP_DigestUpdate(mdctx, trueUUID.c_str(), trueUUID.length());  // add uuid to hash
	EVP_DigestUpdate(mdctx, mp->serverAddressInput.text.c_str(), mp->serverAddressInput.text.length());  // add server address to hash
	EVP_DigestFinal(mdctx, hashBytes, NULL);
	EVP_MD_CTX_free(mdctx);
	
	std::stringstream rawuuid;
	for (int i = 0; i < 0x10; i++)
	    rawuuid << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hashBytes[i]);
	
	uuid uuid = uuid(rawuuid.str());
	uuidstr = uuid.to_string(uuid);  // add in dashes
	uuidlbl = "Obfuscated UUID:";
	int teaseLength = 8;
	
	if (mp->serverAddressInput.text == "") {
		uuidstr = trueUUID;
		uuidlbl = "!!! UNobfuscated UUID: !!!";
		teaseLength = 4;
	}
	
	uuid_hidden = uuidstr;
	std::replace_if(uuid_hidden.begin() + teaseLength, uuid_hidden.end(), [](char c) {return c != '-';}, '*');
	
	return updateGame(mp,uuidstr,uuidlbl,uuid_hidden);
}

$hook(void, StateMultiplayer, joinButtonCallback) {
	original(self);
	updateUUID();
}

$hook(void, StateMultiplayer, keyInput, StateManager& s, int key, int scancode, int action, int mods ) {
	original(self,s,key,scancode,action,mods);
	updateUUID();
}

$hook(void, StateMultiplayer, init, StateManager& s, uint32_t codepoint) {
	original(self,s,codepoint);
	updateUUID();
}

$hook(void, StateMultiplayer, update, StateManager& s, double dt) {
	original(self,s,dt);
	updateUUID();
}
