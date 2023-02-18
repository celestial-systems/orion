ENTRY(ori_main)


SECTIONS
{
	kernel_offset = 0xffffffff80000000;

	. = kernel_offset + 0x200000;

	.text ALIGN(4k) :
	{
		KEEP(*(.text*))
	}

	.rodata ALIGN(4k) :
	{
		KEEP(*(.rodata*))
	}

	.data ALIGN(4k) :
	{
		KEEP(*(.data*))
	}

	.bss ALIGN(4k) :
	{
		KEEP(*(COMMON))
		KEEP(*(.bss*))
	}
}