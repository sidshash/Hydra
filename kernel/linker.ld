ENTRY(_start)
KERNEL_VMA = 0xFFFFFFFF80000000;
SECTIONS
{
	. = 1M;

  .boot ALIGN(4K) :
  {
    *(.multiboot)
    *(.boot)
    *(.boot.rodata)
  }

  .boot.bss ALIGN(4K) (NOLOAD):
  {
    *(.boot.bss)
  }

  . += KERNEL_VMA;

	.text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VMA)
	{
		*(.text*)
	}

	/* Read-only data. */
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VMA)
	{
		*(.rodata*)
	}

	/* Read-write data (initialized) */
	.data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VMA)
	{
		*(.data*)
	}

	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VMA)
	{
		*(COMMON)
		*(.bss*)
	}

}