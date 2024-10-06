
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
extern f32 lbl_8028A3E8;
extern f32 lbl_8028A420;
extern f32 lbl_8028A424;

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

typedef struct Capsules {
    u8 cost;
    u8 weight;
} Capsules;

//800C0950
void CapMachineExec2(void) {
    char* buffer;
    Capsules* costs;
    s32 i, j;
    s32 choice;
    ItemChoices itemChoices[10]; //10 items max
    //volatile so that regardless of optimization level, it will always recheck the value (needed for the choice message box at the end)
    //currently though, we compile with -O0 so no volatile needed
    s32 amountOfShopItems = 3; 
    char* itemChoiceFormat;
    char* itemChoiceFormat2;
    char* shopPrologueText;
    char* singleString;
    char* exitString;
    s32 len;
    s32 skipAnimation = TRUE;

    ASM_DATA(costs,
    "\
    5, 10, /* 0 - Mushroom */\
    15, 10, /* 1 - Super Mushroom */\
    10, 10, /* 2 - Cursed Mushroom */\
    10, 10, /* 3 - Warp Pipe */\
    10, 10, /* 4 - Klepto */\
    10, 10, /* 5 - Bubble */\
    30, 10, /* 6 - Wiggler */\
    10, 10, /* 7 - Unused */\
    10, 10, /* 8 - Unused */\
    10, 10, /* 9 - Unused */\
    10, 10, /* 10 - Hammer Bro */\
    5, 10, /* 11 - Coin Block */\
    10, 10, /* 12 - Spiny */\
    15, 10, /* 13 - Paratroopa */\
    15, 10, /* 14 - Bullet Bill */\
    20, 10, /* 15 - Goomba */\
    10, 10, /* 16 - Bob-omb */\
    15, 10, /* 17 - Koopa Bank */\
    10, 10, /* 18 - Unused */\
    10, 10, /* 19 - Unused */\
    10, 10, /* 20 - Kamek */\
    10, 10, /* 21 - Mr. Blizzard */\
    15, 10, /* 22 - Piranha Plant */\
    10, 10, /* 23 - Magikoopa */\
    20, 10, /* 24 - Ukiki */\
    10, 10, /* 25 - Lakitu */\
    10, 10, /* 26 - Unused */\
    10, 10, /* 27 - Unused */\
    10, 10, /* 28 - Unused */\
    10, 10, /* 29 - Unused */\
    15, 10, /* 30 - Tweester */\
    20, 10, /* 31 - Duel */\
    25, 10, /* 32 - Chain Chomp */\
    15, 10, /* 33 - Bone */\
    20, 10, /* 34 - Bowser */\
    30, 10, /* 35 - Chance */\
    30, 10, /* 36 - Miracle */\
    20, 10, /* 37 - Donkey Kong */\
    20, 10, /* 38 - Versus */\
    10, 10, /* 39 - Unused */\
    10, 10, /* 40 - Camera Debug */\
    10, 10, /* 41 - Movement Debug */\
    ");

    //ASM_STRING(itemChoiceFormat, "\x0F  %%s\x10\x0E\x13%%d\n");

    ASM_STRING(shopPrologueText, "    Which\x10""capsule\x10would\x10you\n    like\x10to\x10purchase");
    ASM_STRING(itemChoiceFormat, "      \x0F%%s");
    ASM_STRING(itemChoiceFormat2, "\x10\x0E\x13>%%d\n");
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

    for (i = 0; i < 42; i++) {
        capsuleTbl[i].price = costs[i].cost;
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

    //get random items for shop
    for (i = 1; i < amountOfShopItems; i++) {
        s32 curItem, curItemCost, isDuplicate;

        //roll for items until it's not a duplicate
        rerollItem:
        GET_CAPSULE_NO(curItem, curItemCost);

        //check to make sure we dont have duplicates
        for (j = 0; j < i; j++) {
            if (itemChoices[j].item == curItem) {
                goto rerollItem;
            }
        }

        if (curItemCost > coin) {
            itemChoices[i].item = 0; //mushroom
            itemChoices[i].cost = capsuleTbl[0].price;
        } else {
            itemChoices[i].item = curItem;
            itemChoices[i].cost = curItemCost;
        }
    }

    //75 characters per line should be enough
    buffer = HuMemDirectMalloc(1, amountOfShopItems * 75);

    //build item list
    sprintf(&buffer[0], singleString, shopPrologueText);
    len = strlen(buffer);
    //for some reason, doing \xC3 directly in the string does weird stuff with sprintf
    buffer[len] = '\xC3';
    buffer[len+1] = '\n';
    buffer[len+2] = '\0';
    //sprintf(&buffer[0], itemChoiceFormat, HuWinMesPtrGet(MBCapsuleMesGet(itemChoices[0].item)), itemChoices[0].cost);
    for (i = 0; i < amountOfShopItems; i++) {
        s32 prevlen = strlen(buffer);
        sprintf(&buffer[prevlen], itemChoiceFormat, HuWinMesPtrGet(MBCapsuleMesGet(itemChoices[i].item)));
        s32 curlen = strlen(buffer);
        s32 curCapsuleNameLen = curlen - prevlen;

        // buffer[curlen] = '\x0C';
        s32 tabCounts;
        //7 for 0x20 0x20 0x20 0x20 0x20 0x20 0xF
        //if item is coin block or bob-omb, hardcode tab count to 4
        if (itemChoices[i].item == 11 || itemChoices[i].item == 16) {
            tabCounts = 4;
            //if kamek or klepto
        } else if (itemChoices[i].item == 20 || itemChoices[i].item == 4 || itemChoices[i].item == 25) {
            tabCounts = 6;
        } else if (curCapsuleNameLen > 13 + 7) {
            // buffer[curlen] = '\x0C';
            tabCounts = 1;
        } else if (curCapsuleNameLen > 11 + 7) {
            // buffer[curlen] = '\x0C';
            // buffer[curlen+1] = '\x0C';
            tabCounts = 2;
        } else if (curCapsuleNameLen > 9 + 7) {
            // buffer[curlen] = '\x0C';
            // buffer[curlen+1] = '\x0C';
            // buffer[curlen+2] = '\x0C';
            tabCounts = 3;
        } else if (curCapsuleNameLen > 7 + 7) {
            // buffer[curlen] = '\x0C';
            // buffer[curlen+1] = '\x0C';
            // buffer[curlen+2] = '\x0C';
            // buffer[curlen+3] = '\x0C';
            tabCounts = 4;
        } else if (curCapsuleNameLen > 5 + 7) {
            // buffer[curlen] = '\x0C';
            // buffer[curlen+1] = '\x0C';
            // buffer[curlen+2] = '\x0C';
            // buffer[curlen+3] = '\x0C';
            // buffer[curlen+4] = '\x0C';
            tabCounts = 5;
        } else {
            // buffer[curlen] = '\x0C';
            // buffer[curlen+1] = '\x0C';
            // buffer[curlen+2] = '\x0C';
            // buffer[curlen+3] = '\x0C';
            // buffer[curlen+4] = '\x0C';
            // buffer[curlen+5] = '\x0C';
            tabCounts = 6;
        }

        for (j = 0; j < tabCounts; j++) {
            buffer[curlen + j] = '\x0C';
        }

        // if (curCapsuleNameLen >= 22) {
        //     buffer[curlen] = '\x0C';
        // } else if (curCapsuleNameLen < 22) {
        //     buffer[curlen] = '\x0C';
        //     buffer[curlen+1] = '\x0C';
        //     buffer[curlen+2] = '\x0C';
        // } else if (curCapsuleNameLen < 18) {
        //     buffer[curlen] = '\x0C';
        //     buffer[curlen+1] = '\x0C';
        //     buffer[curlen+2] = '\x0C';
        //     buffer[curlen+3] = '\x0C';
        // } else if (curCapsuleNameLen < 16) {
        //     buffer[curlen] = '\x0C';
        //     buffer[curlen+1] = '\x0C';
        //     buffer[curlen+2] = '\x0C';
        //     buffer[curlen+3] = '\x0C';
        //     buffer[curlen+4] = '\x0C';
        // } else if (curCapsuleNameLen < 12) {
        //     buffer[curlen] = '\x0C';
        //     buffer[curlen+1] = '\x0C';
        //     buffer[curlen+2] = '\x0C';
        //     buffer[curlen+3] = '\x0C';
        //     buffer[curlen+4] = '\x0C';
        //     buffer[curlen+5] = '\x0C';
        // }

        // for (; temp < 25; temp++) {
        //     buffer[prevlen + temp] = '\x1A';
        // }

        len = strlen(buffer);
        sprintf(&buffer[len], itemChoiceFormat2, itemChoices[i].cost);

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
        // f32 sp8[2];
        // s32 temp_r28;
        // s32 var_r29;
        // if((itemChoices[choice].item) == 34) {
        //     MBCapsuleKupaGetExec();
        // } else if((itemChoices[choice].item) == 36) {
        //     MB3MiracleGetExec();
        // }
        // MBPlayerCapsuleNoAdd(playerNo, (itemChoices[choice].item));
        // MBPlayerPosGet(playerNo, sp8);
        // sp8[1] += lbl_8028A420; //250.0f
        // temp_r28 = MBCoinDispCreate(sp8, -itemChoices[choice].cost, 0);
        // MBCameraMotionWait();
        // MBPlayerMotionNoShiftSet(playerNo, 0xC, lbl_8028A3E8, lbl_8028A424, 0);
        // MBCoinAddNoDispExec(playerNo, -itemChoices[choice].cost);
        // //wait for animation to finish
        // var_r29 = 0;
        // while (MBCoinDispCheck(temp_r28) == 0 || var_r29 == 0) {
        //     if ((MBPlayerMotionEndCheck(playerNo) != 0) && (var_r29 == 0)) {
        //         MBPlayerMotIdleSet(playerNo);
        //         var_r29 = 1;
        //     }
        //     HuPrcVSleep();
        // }
    }
    MBTopWinKill();
    HuMemDirectFree(buffer);
    HuPrcEnd();
}
