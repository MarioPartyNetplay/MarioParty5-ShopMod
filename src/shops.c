#include "include/types.h"

#define FALSE 0
#define TRUE 1

typedef struct CapsuleTbl {
/* 0x00 */ s32 unk_00;
/* 0x04 */ char unk_04[0x10];
/* 0x14 */ s32 price;
/* 0x18 */ char unk_18[1];
/* 0x19 */ s8 unk_19;
/* 0x1A */ char unk_1A[2];
} CapsuleTbl;
//801CA21C
extern CapsuleTbl capsuleTbl[];

s32 GWPlayerCoinGet(s32);
s16 print8(s16 x, s16 y, float scale, char *str, ...);
void HuPrcVSleep(void);
void HuPrcEnd(void);
static TestFunc(void);

extern s32 fontcolor;
extern u16 HuPadBtnDown[4];
#define PAD_BUTTON_LEFT 0x0001
#define PAD_BUTTON_RIGHT 0x0002
#define PAD_BUTTON_DOWN 0x0004
#define PAD_BUTTON_UP 0x0008
#define PAD_TRIGGER_Z 0x0010
#define PAD_TRIGGER_R 0x0020
#define PAD_TRIGGER_L 0x0040
#define PAD_BUTTON_A 0x0100
#define PAD_BUTTON_B 0x0200
#define PAD_BUTTON_X 0x0400
#define PAD_BUTTON_Y 0x0800
#define PAD_BUTTON_MENU 0x1000
#define PAD_BUTTON_START 0x1000

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define _ASM_STRING(str_var, str_value, counter_val) \
    asm( \
        "bl  _end" TOSTRING(counter_val) "\n" \
        ".asciz \"" str_value "\"\n" \
        ".balign 4\n" \
        "_end" TOSTRING(counter_val) ":\n" \
        "mflr %0\n" \
        : "=r" (str_var) \
    );
    
#define ASM_STRING(str_var, str_value) _ASM_STRING(str_var, str_value, __COUNTER__)


#define _ASM_DATA(data_var, data_array, counter_val) \
    asm( \
        "bl  _end" TOSTRING(counter_val) "\n" \
        ".byte " data_array "\n" \
        ".balign 4\n" \
        "_end" TOSTRING(counter_val) ":\n" \
        "mflr %0\n" \
        : "=r" (data_var) \
    );

#define ASM_DATA(data_var, data_array) _ASM_DATA(data_var, data_array, __COUNTER__)

//this version *can* give you items you cant afford
#define GET_CAPSULE_NO(capsuleNo, cost) \
    do { \
        (capsuleNo) = MBCapsuleNoNextGet(); \
        (cost) = MBCapsuleCostGet(capsuleNo); \
    } while(0)

//this item ONLY lets you get items you can afford
#define GET_CAPSULE_NO_CAN_AFFORD(capsuleNo, cost) \
    do { \
        (capsuleNo) = MBCapsuleNoNextGet(); \
        (cost) = MBCapsuleCostGet(capsuleNo); \
    } while((cost) > coin)

#define USE_CAPSULE(capsuleNo, cost) \
    if((capsuleNo) == 34) { \
        MBCapsuleKupaGetExec(); \
    } \
    if((capsuleNo) == 36) { \
        MB3MiracleGetExec(); \
    } \
    MBPlayerCapsuleNoAdd(playerNo, (capsuleNo)); \
    MasuCoinMain(playerNo, -(cost))

typedef struct ItemChoices {
    u8 item;
    u8 cost;
} ItemChoices;

//800C0950
void CapMachineExec2(void) {
    char* buffer;
    u8* costs;
    s32 i, j;
    s32 choice;
    ItemChoices itemChoices[10]; //10 items max
    //volatile so that regardless of optimization level, it will always recheck the value (needed for the choice message box at the end)
    //currently though, we compile with -O0 so no volatile needed
    s32 amountOfShopItems = 5; 
    char* itemChoiceFormat;
    char* shopPrologueText;
    char* singleString;
    char* exitString;
    s32 len;

    // ASM_DATA(costs,
    // "\
    // 20, /* 0 - Mushroom */\
    // 20, /* 1 - Super Mushroom */\
    // 20, /* 2 - Cursed Mushroom */\
    // 20, /* 3 - Warp Pipe */\
    // 20, /* 4 - Klepto */\
    // 20, /* 5 - Bubble */\
    // 20, /* 6 - Wiggler */\
    // 20, /* 7 - Unused */\
    // 20, /* 8 - Unused */\
    // 20, /* 9 - Unused */\
    // 20, /* 10 - Hammer Bro */\
    // 20, /* 11 - Coin Block */\
    // 20, /* 12 - Spiny */\
    // 20, /* 13 - Paratroopa */\
    // 20, /* 14 - Bullet Bill */\
    // 20, /* 15 - Goomba */\
    // 20, /* 16 - Bob-omb */\
    // 20, /* 17 - Koopa Bank */\
    // 20, /* 18 - Unused */\
    // 20, /* 19 - Unused */\
    // 20, /* 20 - Kamek */\
    // 20, /* 21 - Mr. Blizzard */\
    // 20, /* 22 - Piranha Plant */\
    // 20, /* 23 - Magikoopa */\
    // 20, /* 24 - Ukiki */\
    // 20, /* 25 - Lakitu */\
    // 20, /* 26 - Unused */\
    // 20, /* 27 - Unused */\
    // 20, /* 28 - Unused */\
    // 20, /* 29 - Unused */\
    // 20, /* 30 - Tweester */\
    // 20, /* 31 - Duel */\
    // 20, /* 32 - Chain Chomp */\
    // 20, /* 33 - Bone */\
    // 20, /* 34 - Bowser */\
    // 20, /* 35 - Chance */\
    // 20, /* 36 - Miracle */\
    // 20, /* 37 - Donkey Kong */\
    // 20, /* 38 - Versus */\
    // 20, /* 39 - Unused */\
    // 20, /* 40 - Camera Debug */\
    // 20  /* 41 - Movement Debug */\
    // ");

    //ASM_STRING(itemChoiceFormat, "\x0F  %%s\x10\x0E\x13%%d\n");

    ASM_STRING(itemChoiceFormat, "      \x0F%%s\x10\x0E\x13>%%d\n");
    ASM_STRING(shopPrologueText, "    Which\x10""capsule\x10would\x10you\n    like\x10to\x10purchase");
    ASM_STRING(singleString, "%%s");
    ASM_STRING(exitString, "      \x0F""Exit");

    //if last turn, display message and exit
    //maybe add a check for if it's a team mode
    if (GwSystem.turnNo == GwSystem.turnCnt) {
        MBWinCreate(0, 0x00240001, -1);
        MBTopWinWait();
        MBTopWinKill();
        HuPrcEnd();
        return;
    }

    s32 playerNo = GwSystem.turnPlayerNo;
    s32 coin = GWPlayerCoinGet(playerNo);

    //actually type this correctly later
    u32* prc = HuPrcCurrentGet();
    u32* prc2 = prc[0x4B];
    u32 player = *prc2;
    MBPlayerMotIdleSet(player); //make player idle

    if(GwPlayer[playerNo].comF) {
        s32 capsuleNo1, cost1;
        GET_CAPSULE_NO_CAN_AFFORD(capsuleNo1, cost1);
        USE_CAPSULE(capsuleNo1, cost1);
        HuPrcEnd();
        return;
    }

    //gets at least 1 item the player can afford
    GET_CAPSULE_NO_CAN_AFFORD(itemChoices[0].item, itemChoices[0].cost);

    for (i = 1; i < amountOfShopItems; i++) {
        s32 curItem, curItemCost;
        
        rerollItem:
        GET_CAPSULE_NO(curItem, curItemCost);

        //check to make sure we dont have duplicates
        for (j = 0; j < i; j++) {
            if (itemChoices[j].item == curItem) {
                goto rerollItem;
            }
        }
        //item wasn't duplicate, if item costs too much make it a mushroom
        if (curItemCost > coin) {
            itemChoices[i].item = 0; //mushroom
            itemChoices[i].cost = capsuleTbl[0].price;
        } else {
            itemChoices[i].item = curItem;
            itemChoices[i].cost = curItemCost;
        }
    }

    //75 characters per line should be enough
    buffer = HuMemDirectMalloc(1, amountOfShopItems * 75); //string of size 300

    //build item list
    sprintf(&buffer[0], singleString, shopPrologueText);
    len = strlen(buffer);
    //for some reason, doing \xC3 directly in the string does weird stuff with sprintf
    buffer[len] = '\xC3';
    buffer[len+1] = '\n';
    buffer[len+2] = '\0';
    //sprintf(&buffer[0], itemChoiceFormat, HuWinMesPtrGet(MBCapsuleMesGet(itemChoices[0].item)), itemChoices[0].cost);
    for (i = 0; i < amountOfShopItems; i++) {
        len = strlen(buffer);
        sprintf(&buffer[len], itemChoiceFormat, HuWinMesPtrGet(MBCapsuleMesGet(itemChoices[i].item)), itemChoices[i].cost);
    }

    len = strlen(buffer);
    sprintf(&buffer[len], singleString, exitString);

    if (amountOfShopItems > 4) {
        MBWinCreateChoice(1, buffer, 0x14, 0);
    } else {
        MBWinCreateChoice(3, buffer, 0x14, 0);
    }

    
    MBTopWinAttrReset(0x10);
    MBTopWinWait();
    choice = MBWinLastChoiceGet();
    if (choice != -1 && choice < amountOfShopItems) {
        USE_CAPSULE(itemChoices[choice].item, itemChoices[choice].cost);
    }
    MBTopWinKill();
    HuMemDirectFree(buffer);
    HuPrcEnd();
}
