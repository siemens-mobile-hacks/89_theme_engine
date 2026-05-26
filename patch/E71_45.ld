SECTIONS {
    .Common 0xA1384600 : {
        KEEP(*(*.text.TransformText))
    }
    .SoftKeys_DrawString_Jump_Part1 0xA0A2AC98 : {
        KEEP(*(*.text.SoftKeys_DrawString_Jump_Part1))
    }
    .SoftKeys_DrawString_Jump_Part2 0xA0A2ACA8 : {
        KEEP(*(*.text.SoftKeys_DrawString_Jump_Part2))
    }
    .SoftKeys_DrawString_Trampoline 0xA0A23994 : {
        KEEP(*(.text.SoftKeys_DrawString_Trampoline))
    }
    .SoftKeys_DrawMiddleIcon_Jump 0xA0A2ACEE : {
        KEEP(*(.text.SoftKeys_DrawMiddleIcon_Jump))
    }
    .SoftKeys_DrawMiddleIcon_Hook 0xA0A36744 : {
        KEEP(*(.text.SoftKeys_DrawMiddleIcon_Hook))
    }
    .SoftKeys 0xA1384700 : {
        KEEP(*(.text.SoftKeys_DrawString_Hook))
    }
    .DisablePleaseWaitAnimation 0xA094EDE6 : {
        KEEP(*(.text.DisablePleaseWaitAnimation))
    }
}
