#r26 holds message ID
#lwz r0, 0x0098 (r31) gets pointer to message
#iterate over message until 0x1E
#then, write \x10 until 0xA is encountered
#Remove Item Price In String

srwi r3, r26, 16
cmpwi r3, 0x0038 #capsule message group ID
bne+ endOfString
lwz r3, 0x0098 (r31) #restore from hook

loopString:
lbz r4, 0x0000 (r3)
cmpwi r4, 0
beq- endOfString
cmpwi r4, 0x1E
beq- check2
#wasn't 0x1E, adv str
addi r3, r3, 1
b loopString

check2:
lbz r4, 0x0001 (r3)
cmpwi r4, 0x5
beq+ removeItemPriceFromString
addi r3, r3, 1
b loopString

removeItemPriceFromString:
li r4, 0x10
itemPriceLoop:
lbz r5, 0x0000 (r3)
cmpwi r5, 0x0A #is is \n
beq- endOfString
#else, write 0x10 (space) to string
stb r4, 0x0000 (r3)
addi r3, r3, 1
b itemPriceLoop


endOfString:
lwz r0, 0x0098 (r31) #restore from hook