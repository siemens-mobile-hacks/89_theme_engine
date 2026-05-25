.section .text.SoftKeys_DrawString_Jump_Part1
.thumb
ldr r2, [sp, #0x68]

.section .text.SoftKeys_DrawString_Jump_Part2
.thumb
blx SoftKeys_DrawString_Trampoline

.section .text.SoftKeys_DrawMiddleIcon_Jump
.thumb
bl SoftKeys_DrawMiddleIcon_Hook
