# echo "hello_world" > /dev/faulty 

[   70.683741] Unable to handle kernel NULL pointer dereference at virtual address 0000000000000000

[   70.687873] Mem abort info:

[   70.688059]   ESR = 0x96000044

[   70.688358]   EC = 0x25: DABT (current EL), IL = 32 bits

[   70.688572]   SET = 0, FnV = 0

[   70.688736]   EA = 0, S1PTW = 0

[   70.688930]   FSC = 0x04: level 0 translation fault

[   70.689208] Data abort info:

[   70.689381]   ISV = 0, ISS = 0x00000044

[   70.689590]   CM = 0, WnR = 1

[   70.689868] user pgtable: 4k pages, 48-bit VAs, pgdp=000000004347c000

[   70.690294] [0000000000000000] pgd=0000000000000000, p4d=0000000000000000

[   70.695748] Internal error: Oops: 96000044 [#1] PREEMPT SMP

[   70.696273] Modules linked in: scull(O) hello(O) faulty(O) ipv6

[   70.697122] CPU: 0 PID: 230 Comm: sh Tainted: G           O      5.15.18 #1

[   70.697537] Hardware name: linux,dummy-virt (DT)

[   70.697946] pstate: 80000005 (Nzcv daif -PAN -UAO -TCO -DIT -SSBS BTYPE=--)

`**using aarch64-buildroot-linux-uclibc-objdump -DS faulty.ko**,` 

`in faulty_write, zero is moved into **x1** and`

`the **store** of the value into the **x1** register that was previously set with zero,` 

`causes a null pointer exception.`


Disassembly of section .text:


0000000000000000 <faulty_write>:

   0:	d503245f 	bti	c
```diff   
- ##`   4:	d2800001 	mov	x1, #0x0                   	// #0`
```
   8:	d2800000 	mov	x0, #0x0                   	// #0
   
   c:	d503233f 	paciasp
   
  10:	d50323bf 	autiasp
```diff  
- ##`  14:	b900003f 	str	wzr, [x1]`
```
  18:	d65f03c0 	ret
  
  1c:	d503201f 	nop


0000000000000020 <faulty_read>:

  20:	d503233f 	paciasp
  
  24:	a9bd7bfd 	stp	x29, x30, [sp, #-48]!
  
  28:	d5384100 	mrs	x0, sp_el0
  
  
[   70.698428] pc : faulty_write+0x14/0x20 [faulty]

[   70.699738] lr : vfs_write+0xf4/0x2a0

[   70.700640] sp : ffff80000a36bd80

[   70.700933] x29: ffff80000a36bd80 x28: ffff0000025bb700 x27: 0000000000000000

[   70.701660] x26: 0000000000000000 x25: 0000000000000000 x24: 0000000000000000

[   70.702232] x23: 0000000000000000 x22: ffff80000a36bdf0 x21: 0000aaaadae2b3c0

[   70.702510] x20: ffff000003530800 x19: 000000000000000c x18: 0000000000000000

[   70.702760] x17: 0000000000000000 x16: 0000000000000000 x15: 0000000000000000

[   70.703006] x14: 0000000000000000 x13: 0000000000000000 x12: 0000000000000000

[   70.703256] x11: 0000000000000000 x10: 0000000000000000 x9 : 0000000000000000

[   70.703547] x8 : 0000000000000000 x7 : 0000000000000000 x6 : 0000000000000000

[   70.703812] x5 : 0000000000000001 x4 : ffff800000f92000 x3 : ffff80000a36bdf0

[   70.704095] x2 : 000000000000000c x1 : 0000000000000000 x0 : 0000000000000000

[   70.704565] Call trace:
```diff
- ##`[   70.704862]  faulty_write+0x14/0x20 [faulty]`
```
[   70.705105]  ksys_write+0x68/0x100

[   70.705279]  __arm64_sys_write+0x20/0x30

[   70.705443]  invoke_syscall+0x48/0x120

[   70.705600]  el0_svc_common.constprop.0+0x44/0xf0

[   70.705826]  do_el0_svc+0x28/0x90

[   70.705967]  el0_svc+0x20/0x60

[   70.706108]  el0t_64_sync_handler+0xec/0xf0

[   70.706276]  el0t_64_sync+0x1a0/0x1a4

[   70.706676] Code: d2800001 d2800000 d503233f d50323bf (b900003f) 

[   70.707319] ---[ end trace ba962ec0dbffac55 ]---


Welcome to Buildroot

buildroot login:


