.global tss_flush

tss_flush:
    mov $0x2b, %ax
    ltr %ax
    ret