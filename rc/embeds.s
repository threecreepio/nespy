.global icon_bin
.align 4
icon_bin:
    .incbin "rc/NESpy.png"
icon_bin_end:
.global icon_bin_size
.align 4
icon_bin_size:
    .int icon_bin_end - icon_bin
